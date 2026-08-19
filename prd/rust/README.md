# Rust Migration Analysis for CloudCompare
**PRD / Feasibility Report** — CloudCompare C++ → Rust Incremental Migration
**Date:** 2026-08-19
**Author:** Mavis (MiniMax Code Agent)
**Status:** Analysis Complete — Decision Gate

---

## 1. Executive Summary

CloudCompare is a mature C++17/Qt6 application (~500k lines, 7 core libs, 16 enabled plugins) for 3D point cloud and mesh processing. This report analyses which subsystems are best suited for incremental Rust migration to eliminate memory-safety bugs, data races, and leak classes that C++ cannot enforce at compile time.

**Key finding:** 5 subsystems are high-value Rust migration candidates. The safest migration path is incremental leaf-first via the **CXX FFI crate**, which provides zero-overhead safe interop between Rust and C++. Rust and C++ achieve equivalent performance for the workloads in this codebase (matrix ops: ~1.02x, SpMV: competitive with Eigen, NPB suite: 5.6% faster than C++). The primary motivation is **safety, not raw speed**.

**Recommendation:** Begin with `ccScalarField` (pure computation, no Qt dependencies, isolated) as Phase 1 beachhead. Add `RegistrationTools` (ICP/Horn) as Phase 2. Move to `DgmOctree` and `KdTree` in Phase 3–4. Keep `ccPointCloud` and all Qt-dependent code in C++ permanently.

---

## 2. Table of Contents

```
PRD/rust/
├── README.md                          ← This file (index + executive summary)
├── 01-code-analysis/                  ← Detailed per-subsystem code analysis
│   ├── 01-scalarfield.md             ← ScalarField: T1 migration target
│   ├── 02-registration.md             ← ICP/Horn/FPCS registration: T2 target
│   ├── 03-octree-kdtree.md            ← DgmOctree + KDTree: T3 target
│   ├── 04-pointcloud.md               ← ccPointCloud: deferred (Qt-bound)
│   ├── 05-io-parsers.md               ← qCC_io file parsers: T4 target
│   └── 06-geom-analysis.md            ← GeometricalAnalysisTools, DistanceTools
├── 02-migration/
│   ├── 01-strategy.md                ← Incremental migration methodology
│   ├── 02-phases.md                  ← Phase 1–4 detailed roadmap
│   └── 03-ffi-cxx.md                 ← CXX FFI crate deep-dive
├── 03-research/
│   ├── 01-benchmarks.md              ← Rust vs C++ performance benchmarks
│   └── 02-case-studies.md            ← Real-world C++→Rust migrations
└── 04-swarm-reports/                  ← Parallel agent analysis outputs
    ├── swarm-01-memory.md            ← Memory management analysis
    ├── swarm-02-concurrency.md       ← Concurrency / data race analysis
    ├── swarm-03-fp-math.md           ← Point-cloud geometry math analysis
    ├── swarm-04-io-parse.md          ← File I/O and parser analysis
    └── swarm-05-plugin-api.md        ← Plugin system analysis
```

---

## 3. Migration Priority Matrix

| Priority | Subsystem | Files | Rust Win | FFI Complexity | Qt Deps | Phase |
|----------|-----------|-------|---------|---------------|---------|-------|
| **T1** | `ScalarField` | `ScalarField.cpp/.h`, `ScalarFieldTools.cpp` | Ownership safety, overflow guards | Low | None | Phase 1 |
| **T2** | `RegistrationTools` | `RegistrationTools.cpp/.h`, `PointProjectionTools.cpp` | Data race elimination, ICP iteration safety | Medium | None | Phase 2 |
| **T3** | `DgmOctree` + `KdTree` | `DgmOctree.cpp/.h`, `KdTree.cpp/.h`, `TrueKdTree.cpp` | Memory leaks, dangling ptrs, MT safety | Medium-High | None | Phase 3 |
| **T4** | `GeometricalAnalysisTools` | `GeometricalAnalysisTools.cpp` | MT race elimination, buffer safety | Medium | None | Phase 3 |
| **T5** | File parsers (qCC_io) | `FileIOFilter.cpp`, format plugins | Input validation, buffer overflow | High | Qt6 | Phase 4 |
| **DEF** | `ccPointCloud` | `ccPointCloud.cpp/.h` | Memory chunking, visibility | Very High | Qt6/GUI | Deferred |
| **NO** | `ccGLWindow` / UI | All `qCC/` and `ccViewer/` | N/A | Extreme | Full Qt6 | Never |

---

## 4. Why Rust for This Codebase

### 4.1 Memory Safety Classes in CloudCompare Today

Based on codebase analysis, the following bug classes are present or latent in the identified subsystems:

| Bug Class | Present In | Rust Prevention |
|-----------|-----------|----------------|
| Use-after-free on `ccOctree` rebuild | `DgmOctree.cpp` | Borrow checker enforces lifetimes |
| Dangling `CCVector3*` in `PointDescriptor` | `DgmOctree.h:98` | Lifetimes verified at compile time |
| Buffer overflow in scalar field resize | `ScalarField.cpp` | `Vec<T>` bounds-checked by default; `unsafe` only in explicit FFI |
| Data race on shared `octreeCellDesc` vector | `DgmOctree.cpp:3064` | `Send`/`Sync` enforced at compile time |
| Memory leak on exception in `ICP::Register` | `RegistrationTools.cpp:165` | `Garbage<>` collector replaced by RAII `Vec<T>` |
| Iterator invalidation on cloud resize | `ccPointCloud.cpp` | Borrow checker prevents aliasing mutation |
| Null pointer deref on failed `buildFromCloud` | `KdTree.cpp:72` | `Option<T>` makes absence explicit |
| Thread-unsafe `m_buildInProgress` atomic | `DgmOctree.h:1254` | `std::sync::atomic` is the only path |

### 4.2 Rust Performance Reality (2024–2025 Benchmarks)

Rust matches or exceeds C++ for the compute workloads in this codebase:

```
+ Matrix multiplication (SIMD):  Rust = 1.02x C++ (CXX FFI 2025)
+ SpMV (CSR/CSC sparse):        Rust = Eigen-equivalent (Moonlight review 2024)
+ NPB sequential suite:          Rust = 5.6% FASTER than C++ (arxiv 2502.15536)
+ JSON/binary parsing:          Rust = 5–15% faster (reintech 2026)
+ Merge sort:                   Rust beats C++ in balanced benchmarks
+ Memory bandwidth ops:         Rust = C++ (bounds-check elision via iterators)
```

C++ retains marginal advantage (~5–10%) in micro-benchmarks for cache-sensitive matrix multiplication at small sizes, but this disappears in real-world workloads. The NPB results (NASA benchmark suite) on 40-thread parallelism: Rust/Rayon matched Fortran/OpenMP and beat C++/OpenMP in EP (embarrassingly parallel).

---

## 5. FFI Strategy: CXX

### 5.1 Why CXX over Bindgen

`CXX` (by David Tolnay, `cxx.rs`) is the recommended FFI crate because:
- Generates **zero-copy** FFI bindings with negligible overhead
- Provides **safe Rust wrappers** around C++ types (not raw `unsafe extern "C"`)
- Static analysis at the language boundary catches type mismatches at compile time
- Works with CMake via `cxx-build` crate as a Cargo build dependency
- Supports `Box<T>`, `Vec<T>`, `String`, C++ `std::vector`, `std::string`, `Eigen::MatrixXd` (with helper)
- 2025 stable: `rustc 1.73+`, C++11+

### 5.2 FFI Boundary Design Rules

```
Golden Rule: Keep FFI boundary boring. Pass plain data. Never leak Rust lifetimes.
```

Every value crossing the FFI boundary must answer:
1. **Who owns it?** → C++ or Rust? Use `Box<T>` (Rust→C++ ownership transfer) or `&T`/`&mut T`
2. **Who can mutate it?** → Exclusive (`&mut`) or shared (`&`) access enforced by type system
3. **How long is it valid?** → Lifetime annotations on Rust side; C++ RAII on C++ side
4. **Can it alias?** → No aliased mutation in Rust; `Pin<&mut T>` for C++ objects across the bridge
5. **Who frees it?** → Explicit `Drop` on Rust side; C++ `delete` for C++-owned data

### 5.3 CXX Thread Safety Note

CXX does **not** assume `Send`/`Sync` on `extern "C++"` types. For thread-safe C++ objects accessed from Rust threads, explicit unsafe impls are required:

```rust
unsafe impl Send for ffi::MyType {}  // only if C++ MyType is thread-safe
unsafe impl Sync for ffi::MyType {}
```

This is a critical safety checkpoint: the ICP `DataCloud` struct contains raw pointers that must be audited before marking as thread-safe.

---

## 6. Build System Integration

### 6.1 Recommended Layout

```
CloudCompare/
├── CCCoreLib/                    ← Existing C++ lib (no changes)
├── cc-rust/                     ← NEW: Rust library (Cargo workspace)
│   ├── Cargo.toml
│   ├── src/
│   │   ├── lib.rs
│   │   ├── scalar_field.rs      ← Phase 1: scalar field
│   │   ├── registration.rs       ← Phase 2: ICP/Horn
│   │   └── octree/
│   │       ├── mod.rs
│   │       ├── dgm_octree.rs   ← Phase 3: octree
│   │       └── kdtree.rs       ← Phase 3: KD-tree
│   └── build.rs                 ← cxx_build::bridge("src/lib.rs").compile()
└── tools/cc-configure.cmd        ← Add: CMake find_package(Rust) + cargo invocation
```

### 6.2 CMake + Cargo Integration

```cmake
# In CCCoreLib/CMakeLists.txt (additions):
find_package(Rust REQUIRED)

# Build Rust library before CCCoreLib
add_custom_target(cc_rust_lib ALL
    COMMAND cargo build --manifest-path ${CMAKE_SOURCE_DIR}/cc-rust/Cargo.toml --release
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/cc-rust
    COMMENT "Building cc-rust library"
)
add_dependencies(CCCoreLib cc_rust_lib)

# Link Rust archive into CCCoreLib
target_link_libraries(CCCoreLib PRIVATE
    ${CMAKE_SOURCE_DIR}/cc-rust/target/release/libcc_rust.a
)
```

### 6.3 CI Integration

- MSVC + Rust toolchain on Windows: `rustup default stable-msvc` (or `x86_64-pc-windows-msvc`)
- Sanitizers: `ASAN`/`MSAN`/`UBsan` on C++ side still work across the FFI boundary
- Format checking: `cargo fmt` + `clang-format` on C++ side independently
- Tests: C++ unit tests unchanged; add Rust `cargo test` to CI

---

## 7. Decision Gate

**Should CloudCompare migrate to Rust?** Answer: **Partially, incrementally.**

| Question | Assessment |
|----------|------------|
| Is the codebase large enough to benefit? | Yes — 500k lines, 7 libs, active development |
| Is memory safety a real problem today? | Yes — manual pointer management in hot paths (octree, KD-tree) |
| Is a full rewrite feasible? | **No** — GPL licensing, active plugin ecosystem, Qt6 GUI dependency |
| Is incremental migration feasible? | **Yes** — CXX FFI is production-proven; ClickHouse uses same pattern |
| What's the realistic timeline? | Phase 1 (ScalarField): 3–6 months; Phase 4 (I/O): 18–36 months |

**Next action:** Authorise Phase 1 pilot — `ccScalarField` → Rust migration with CXX FFI, shipped behind a feature flag with before/after benchmarks and characterisation tests.

---

## 8. Research Sources

- Rust/C++ FFI: [CXX official docs](https://cxx.rs/) — safe interop, zero-copy bridge
- CXX 2025 update: [Tyler Weaver — Rust/C++ Interop in 2025](https://tylerjw.dev/posts/20251003-rust-cpp-interop-2025-update/)
- Benchmarks matrix mult: [dev.to — Rust in 2025](https://dev.to/code_2/rust-in-2025-did-it-finally-overtake-c-10ko)
- NPB suite Rust vs C++/Fortran: [arxiv 2502.15536](https://arxiv.org/html/2502.15536v1)
- Sparse matrix kernels: [Moonlight review](https://www.themoonlight.io/en/review/evaluating-rust-for-sparse-matrix-kernels-in-scientific-computing)
- C++ to Rust migration strategy: [JetBrains Blog 2026](https://blog.jetbrains.com/rust/2026/07/27/cpp-to-rust-migration/)
- Incremental migration patterns: [booleanbeyond.com](https://www.booleanbeyond.com/en/insights/c-cpp-to-rust-migration-guide)
- ClickHouse/Discord case studies: [SoftWare Seni](https://www.softwareseni.com/how-to-build-the-business-case-and-migration-plan-for-moving-legacy-c-to-rust/)
- OpenSSF Memory Safety: [github.com/ossf/Memory-Safety](https://github.com/ossf/Memory-Safety/blob/main/docs/best-practice-interfacing.md)
