# Migration Phases — Detailed Implementation Plan
**Date:** 2026-08-19
**Scope:** CloudCompare C++ → Rust Incremental Migration

---

## Phase 0: Infrastructure (Month 1–2)

### Goal: Build system + FFI + CI working before writing any Rust logic

### Deliverables

- [ ] Rust toolchain installed: `x86_64-pc-windows-msvc` target active
- [ ] `cc-rust/` Cargo workspace created with `Cargo.toml` + `build.rs`
- [ ] CXX bridge compiles: `#[cxx::bridge]` generates C++ glue code
- [ ] CMake integration: `cc_rust_lib` custom target runs `cargo build`
- [ ] CCCoreLib links `libcc_rust.lib`
- [ ] CloudCompare.exe launches without crash (basic smoke test)
- [ ] Sanitizers (ASAN) run across FFI boundary without false positives
- [ ] `cargo test` added to CI alongside existing C++ tests
- [ ] Format checking: `cargo fmt` + `clang-format` configured independently

### Key Files to Create

```
cc-rust/
├── Cargo.toml           # workspace root
├── build.rs             # cxx_build::bridge("src/lib.rs").compile()
├── src/
│   ├── lib.rs           # empty bridge, compiles and links
│   └── cc_rust.cc      # CXX-generated C++ glue (do not edit)
└── .cargo/
    └── config.toml     # target x86_64-pc-windows-msvc
```

### CMake Changes Required

```cmake
# tools/cc-configure.cmd additions:
set RUST_TARGET=x86_64-pc-windows-msvc
set CC_RUST_DIR=%SRC_DIR%\cc-rust
set CC_RUST_TARGET_DIR=%BUILD_DIR%\cc_rust

# In CCCoreLib/CMakeLists.txt:
add_subdirectory(%CC_RUST_DIR% cc_rust)
target_link_libraries(CCCoreLib PRIVATE cc_rust)
```

### Verification

```bash
# After Phase 0:
cmake --build build --target CloudCompare
# → builds without error
# → binary launches, shows main window
# → no crashes
```

---

## Phase 1: ScalarField (Month 3–6)

### Rationale
`ccScalarField` / `ScalarFieldTools` is the ideal first Rust component:
- ✅ No Qt dependencies (inherits from `std::vector<float>`, no GUI coupling)
- ✅ No threading (single-threaded, no FFI concurrency concerns)
- ✅ Well-defined I/O: `Vec<f32>` in, statistics out
- ✅ Existing test coverage: `qCC/test/TestScalarField.cpp`
- ✅ Isolated — no other CCCoreLib code depends on ScalarField internals

### What to Migrate

| C++ File | → | Rust Module | Lines |
|----------|---|-------------|-------|
| `ScalarField.cpp/.h` | → | `cc-rust/src/scalar_field.rs` | ~500 |
| `ScalarFieldTools.cpp/.h` | → | `cc-rust/src/scalar_field_tools.rs` | ~800 |
| Characterisation tests | → | `cc-rust/tests/scalar_field_parity.rs` | ~300 |

### CXX Bridge for Phase 1

```rust
// cc-rust/src/lib.rs additions for Phase 1
#[cxx::bridge]
mod ffi {
    extern "Rust" {
        fn rust_scalar_mean(values: &[f32]) -> f64;
        fn rust_scalar_std(values: &[f32], mean: f64) -> f64;
        fn rust_scalar_min_max(values: &[f32]) -> (f32, f32);
        fn rust_scalar_valid_count(values: &[f32]) -> usize;
        fn rust_scalar_apply_offset(values: &mut [f32], offset: f64);
        fn rust_scalar_compare(a: &[f32], b: &[f32], tol: f64) -> bool;
    }
}
```

### C++ Wrapper (Gradual Cutover)

```cpp
// In ScalarField.cpp, replace individual methods one at a time:
// BEFORE (all C++):
double ScalarField::computeMean() const {
    double sum = 0;
    for (size_t i = 0; i < size(); ++i)
        if (isfinite(at(i))) sum += at(i);
    return sum / countValidValues();
}

// AFTER (Rust call):
double ScalarField::computeMean() const {
    return rust_scalar_mean(data(), size());  // CXX converts std::vector → &[f32]
}
```

### Characterisation Test

```rust
// cc-rust/tests/scalar_field_parity.rs
#[test]
fn test_mean_parity_with_cpp() {
    let values: Vec<f32> = (0..10000).map(|i| i as f32 * 0.001).collect();
    let rust_mean = rust_scalar_mean(&values);
    let cpp_mean = unsafe { CxxScalarFieldTools_ComputeMean(values.as_ptr(), values.len()) };
    assert!((rust_mean - cpp_mean).abs() < 1e-6);
}
```

### Success Criteria

- [ ] All 12 existing `TestScalarField` tests pass with Rust implementation
- [ ] Rust mean/std/min/max matches C++ to within `1e-6` on characterisation corpus
- [ ] Performance: Rust implementation ≤ C++ implementation (±10%)
- [ ] No ASAN/TSAN/MSAN violations across FFI boundary
- [ ] Feature flag: `ENABLE_RUST_SCALAR_FIELD=ON/OFF` toggles between implementations

---

## Phase 2: Registration (Month 7–12)

### Rationale
`RegistrationTools` (Horn + ICP) is the highest-value Rust target:
- 🔴 ICP iterations are the primary user-visible bottleneck
- 🔴 Data race risk in `m_MT_wrapper` (see Swarm Report 02)
- 🔴 Manual `Garbage<>` collectors replaceable with Rust's automatic drop
- ✅ No Qt dependencies — pure compute
- ✅ Well-defined input/output contracts
- ✅ Heavy enough that Rust's safety guarantees have real engineering value

### What to Migrate

| C++ File | → | Rust Module | Risk |
|----------|---|-------------|------|
| `RegistrationTools.cpp` | → | `cc-rust/src/registration.rs` | MEDIUM |
| `PointProjectionTools.cpp` | → | `cc-rust/src/transform.rs` | LOW |
| `ccOctree.cpp` (build only) | → | `cc-rust/src/octree_build.rs` | MEDIUM |

### ICP Architecture in Rust

```rust
// cc-rust/src/registration.rs

/// ICP registration state machine
pub struct IcprContext {
    model_kdtree: kdtree::KdTree<3, f32, ()>,  // KD-tree of model cloud
    data_points: Vec<[f32; 3]>,                  // Working copy of data cloud
    params: IcprParams,
    iteration: u32,
    last_rms: f64,
    transform: nalgebra::Isometry3<f64>,
}

impl IcprContext {
    pub fn new(model_points: &[[f32; 3]], params: IcprParams) -> Self {
        let kdtree = kdtree::KdTree::new_infer();
        for (i, pt) in model_points.iter().enumerate() {
            kdtree.add(*pt, ()).unwrap();
        }
        IcprContext {
            model_kdtree: kdtree,
            data_points: model_points.to_vec(),
            params,
            iteration: 0,
            last_rms: f64::MAX,
            transform: nalgebra::Isometry3::identity(),
        }
    }

    pub fn step(&mut self) -> IcprIterationResult {
        // 1. Find nearest neighbours via kdtree
        let neighbours = self.find_neighbours();

        // 2. Compute optimal transformation (Horn or SVD)
        let delta_trans = self.compute_transform(&neighbours);

        // 3. Apply transformation to data points
        self.apply_transform(&delta_trans);

        // 4. Compute RMS
        let rms = self.compute_rms();

        // 5. Check convergence
        let converged = (self.last_rms - rms).abs() < self.params.min_rms_decrease
            || self.iteration >= self.params.max_iterations;

        self.last_rms = rms;
        self.iteration += 1;

        IcprIterationResult { rms, converged }
    }
}
```

### Thread Safety — ICP Context

```rust
// ICP context is NOT Send/Sync by default (raw pointers in params)
// Only allow single-threaded access
// When moving to multi-threaded ICP:
//   - Use Arc<RwLock<IcprContext>> for shared state
//   - Each worker thread gets its own data_points copy
//   - Model kdtree is read-only → Arc<KdTree> shared across threads

use std::sync::{Arc, RwLock};

pub struct ThreadSafeIcprContext {
    inner: Arc<RwLock<IcprContext>>,
}

unsafe impl Send for ThreadSafeIcprContext {}
unsafe impl Sync for ThreadSafeIcprContext {}
```

### Success Criteria

- [ ] ICP produces identical results to C++ implementation (characterisation tests)
- [ ] ICP performance: Rust ≤ C++ (±10%)
- [ ] ICP with `useC2MSignedDistances=true` works correctly
- [ ] Normals matching (`NORMALS_MATCHING` enum) handled correctly
- [ ] `Garbage<>` collector in `ICPRegistrationTools::Register` replaced by Rust's `Drop`

---

## Phase 3: Spatial Indexing (Month 13–24)

### Rationale
`DgmOctree` + `KDTree` are the most complex Rust targets:
- 🔴 Manual `new`/`delete` in KD-tree → `Box<KdTreeCell>` in Rust
- 🔴 `NeighboursSet` allocation patterns → `SmallVec` in Rust
- 🔴 `MultiThreadingWrapper` data races → `Arc<Mutex<T>>` in Rust
- ⚠️ High FFI complexity — must handle `void**` user params
- ⚠️ Qt/TBB dual-backend threading — Rayon replacement needs validation

### Octree Rust Architecture

```rust
// cc-rust/src/octree/mod.rs

pub struct DgmOctree {
    points: Vec<[f32; 3]>,
    bounding_box: (CCVector3, CCVector3),
    cells: Vec<OctreeCell>,
    level: u8,
}

pub struct OctreeCell {
    truncated_code: u64,
    points: Vec<usize>,     // indices into points array
    neighbours: Vec<usize>,  // neighbour cell indices
    children: Option<[usize; 8]>,  // child indices (None = leaf)
}

impl DgmOctree {
    pub fn build(points: &[[f32; 3]], max_level: u8) -> Self {
        let bb = compute_bounding_box(points);
        let mut cells = vec![OctreeCell::root(bb)];
        let mut worklist: Vec<usize> = vec![0];

        while let Some(cell_idx) = worklist.pop() {
            let cell = &mut cells[cell_idx];
            if cell.points.len() > 1 && cell.level < max_level {
                let children = cell.subdivide();
                worklist.extend(children.iter().copied());
                cell.children = Some(children);
            }
        }

        DgmOctree { points: points.to_vec(), cells, level: max_level }
    }
}
```

### Rayon vs TBB for Octree

```rust
// Replace QtConcurrent::blockingMap with rayon::ParallelIterator
use rayon::prelude::*;

let cell_indices: Vec<usize> = (0..cells.len()).collect();
cell_indices.par_iter().for_each(|&idx| {
    compute_octree_cell(&mut cells[idx], &params);
});

// Equivalent to TBB:
// tbb::parallel_for(tbb::blocked_range(0, cells.size()), |range| {
//     for i in range { compute_octree_cell(&mut cells[i]); }
// });
```

### Success Criteria

- [ ] Octree build produces identical structure to C++ (structure + Morton codes)
- [ ] Octree build performance: Rust ≤ C++ (±10%)
- [ ] Nearest neighbour query results: identical to `KDTree::findNearestNeighbours`
- [ ] Multi-threading via Rayon validated against TBB (no divergence in results)
- [ ] `m_buildInProgress` atomic state machine replaced by Rust enum (Idle | Building | Ready | Error)

---

## Phase 4: File I/O Parsers (Month 18–36)

### Rationale
File format parsers are high-value Rust targets:
- 🔴 Input validation: parser receives untrusted external data
- 🔴 Buffer overflow: most historical CVEs are in file parsers
- ✅ Complete isolation: no Qt dependencies in the parsing layer
- ✅ Pure Rust crates exist for LAS, PLY, CSV, OBJ
- ✅ Can eliminate C++ dependencies (LASzip, shapelib)

### Migration Path

```
Phase 4a: CSV scalar field import/export (1 month)
  → Rust: csv crate
  → No FFI needed — pure Rust, integrated via subprocess IPC

Phase 4b: PLY parser replacement (3 months)
  → Rust: ply-rs crate
  → CXX bridge: Vec<u8> → PointCloudView
  → Eliminates: C++ PLY parser maintenance burden

Phase 4c: LAS/LAZ parser (6 months)
  → Rust: las-rs or laz-perf crate
  → Eliminates: LASzip C++ dependency
  → CXX bridge: raw LAS bytes → ccPointCloud
```

### FFI Contract for Parser Results

```rust
// cc-rust/src/io/las.rs
#[cxx::bridge]
mod las_ffi {
    extern "Rust" {
        fn parse_las_file(path: &str) -> Result<LasParseResult, LasError>;
        fn parse_las_bytes(data: &[u8]) -> Result<LasParseResult, LasError>;
    }

    extern "C++" {
        type ccPointCloud;
    }
}

pub struct LasParseResult {
    pub points: Vec<[f32; 3]>,
    pub colors: Option<Vec<[u8; 3]>>,
    pub intensities: Option<Vec<f32>>,
    pub classification: Option<Vec<u8>>,
    pub gps_time: Option<Vec<f64>>,
}

// C++ wrapper (in qCC_io):
ccPointCloud* LasFilter::loadFromFile(QString filename) {
    auto result = parse_las_file(filename.toStdString());  // CXX bridge
    auto* cloud = new ccPointCloud("LAS import");
    cloud->reserve(result->points.size());
    for (auto& p : result->points) {
        cloud->addPoint(CCVector3(p[0], p[1], p[2]));
    }
    return cloud;
}
```

---

## Rollback Strategy

Every phase must be deployable with a **feature flag toggle** between C++ and Rust:

```cpp
// In CCCoreLib build options:
option(ENABLE_RUST_SCALAR_FIELD "Use Rust ScalarField implementation" OFF)
option(ENABLE_RUST_REGISTRATION "Use Rust RegistrationTools implementation" OFF)
option(ENABLE_RUST_OCTREE "Use Rust DgmOctree implementation" OFF)

# In code:
#ifdef ENABLE_RUST_SCALAR_FIELD
    return rust_scalar_mean(data(), size());
#else
    return cpp_scalar_mean(data(), size());
#endif
```

**In CI:** Test both ON and OFF configurations. Rust implementation is only enabled after parity tests pass.
