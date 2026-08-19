# Experimental Sandbox Plan — CloudCompare Rust Migration
**Date:** 2026-08-19
**Goal:** Build a throwaway Rust/C++ FFI workspace that mimics CCCoreLib's environment,
loads real survey data, and lets you iterate Rust implementations without touching
the main CloudCompare codebase.
**Rule: everything here is throwaway.** If it breaks, delete it and start again.

---

## What Goes Where

```
C:\dev\CloudCompare\
├── PRD/rust/                    ← analysis + roadmap (permanent, git-tracked)
└── sandbox/                     ← experimental Rust workspace (gitignored)
    ├── Cargo.toml
    ├── build.rs
    ├── src/
    │   ├── main.rs               ← entry point + CLI
    │   ├── lib.rs                ← CXX bridge declarations
    │   ├── scalar_field.rs       ← Phase 1: Rust ScalarField impl
    │   ├── registration.rs       ← Phase 2: Rust ICP impl
    │   ├── las_loader.rs         ← load .las via C++ FileIOFilter
    │   └── bench.rs              ← benchmarks (criterion)
    ├── cpp/
    │   └── las_loader.cc         ← C++ LAS loader (Qt-free, CXX-safe)
    ├── external/                  ← links against built CCCoreLib DLLs
    │   ├── include/              ← CCCoreLib headers (symlink or copy)
    │   ├── CCCoreLib.dll
    │   └── CCCoreLib.lib
    ├── tests/
    │   ├── test_scalar_field.rs  ← parity tests against C++
    │   ├── test_registration.rs  ← ICP characterisation
    │   └── test_icp_real_data.rs ← load .las, run ICP, check RMS
    └── data/
        └── D82_BROOK/           ← symlinks to real survey data
            ├── scan1.las        ← 253 MB open-format export
            └── scan2.las
```

---

## Two-Track Approach (choose one)

### Track A — Link Against Built DLLs (fastest setup)
Use the CCCoreLib DLLs already built in `build/libs/CCCoreLib/`. No recompile needed.

**Pros:** Instant. Uses exactly the same `.lib` + `.dll` that CloudCompare.exe uses.
**Cons:** You need to match the MSVC runtime version exactly. DLL search path must be set.

### Track B — Build CCCoreLib as a Submodule (cleaner, reproducible)
Clone CCCoreLib into `sandbox/external/CCCoreLib/` and build it as part of the sandbox.
**Pros:** You control the exact build flags. No DLL path hunting. Reproducible.
**Cons:** Extra build step. Slightly slower first run.

**Recommendation: Track B.** One extra configure step but zero DLL hunting forever.

---

## Step-by-Step: Track B (recommended)

### Step 1: Install Rust

```powershell
# Install Rust MSVC target (already default on Windows, but confirm)
rustup default stable
rustup target add x86_64-pc-windows-msvc
```

Verify:
```powershell
cargo --version
rustc --version
# → rustc 1.80+ (stable)
```

---

### Step 2: Create the sandbox directory

```powershell
New-Item -ItemType Directory -Path C:\dev\CloudCompare\sandbox -Force
```

Create `C:\dev\CloudCompare\.gitignore` entries (check if not already there):
```
/sandbox/
```

---

### Step 3: Write `sandbox/Cargo.toml`

```toml
[package]
name = "cc-sandbox"
version = "0.1.0"
edition = "2021"

[lib]
name = "cc_sandbox"
crate-type = ["staticlib", "cdylib"]

[dependencies]
cxx = "1.0"
rayon = "1.10"
nalgebra = "0.34"          # linear algebra (SVD, ICP)
glam = "0.29"              # 3D geometry
smallvec = "1.13"           # small-vector optimisation
thiserror = "1.0"           # error handling
tracing = "0.1"             # structured logging (replaces fmt::print)
tracing-subscriber = "0.3" # console output for tracing
tracing-sentry = "0.1"     # Sentry integration for Rust
csv = "1.3"                # CSV input (for synthetic test data)
criterion = { version = "0.5", features = ["html_reports"] }  # benchmarks

[build-dependencies]
cxx-build = "1.0"

[profile.release]
lto = true
codegen-units = 1
panic = "abort"
```

---

### Step 4: Write `sandbox/build.rs`

```rust
fn main() {
    let manifest_dir = std::env::var("CARGO_MANIFEST_DIR").unwrap();
    let sandbox_root = std::path::Path::new(&manifest_dir);

    // ── Track B: Build CCCoreLib from source ──────────────────────────
    let cc_core_lib_src = sandbox_root.join("external/CCCoreLib");
    let cc_core_build = sandbox_root.join("build/cc_core_lib");

    // Only configure if not already configured
    if !cc_core_build.join("CMakeCache.txt").exists() {
        println!("cargo:warning=CCCoreLib not configured — run: cmake -S {} -B {} -G Ninja",
            cc_core_lib_src.display(), cc_core_build.display());
    }

    // Tell Cargo to link against the built CCCoreLib import library
    println!("cargo:rustc-link-search=native={}", cc_core_build.join("lib").display());
    println!("cargo:rustc-link-lib=CCCoreLib");

    // ── CXX bridge ─────────────────────────────────────────────────────
    let include_dirs = [
        sandbox_root.join("external/CCCoreLib/include"),
        sandbox_root.join("external/CCCoreLib/libs/qCC_db/extern/CCCoreLib/include"),
    ];

    cxx_build::bridge("src/lib.rs")
        .files(["src/lib.rs", "cpp/las_loader.cc"])
        .includes(&include_dirs)
        .flag_if_supported("/std:c++17")
        .compile("cc_sandbox_bridge");

    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=cpp/las_loader.cc");
    println!("cargo:rerun-if-changed=build.rs");
}
```

---

### Step 5: Write `sandbox/src/lib.rs` — CXX bridge

```rust
// src/lib.rs
// CXX bridge: Rust ←→ CCCoreLib C++
// This is the ONLY interface between Rust and C++.
// All other Rust code is pure and testable without C++.

#[cxx::bridge]
mod ffi {
    // ── Types from CCCoreLib we wrap ─────────────────────────────────
    extern "C++" {
        include!("ScalarField.h");
        include!("GenericIndexedCloudPersist.h");
        include!("ScalarFieldTools.h");

        type CxxScalarField = crate::ScalarField;
        type CxxCloud = crate::Cloud;

        // C++ loader: reads a .las file and returns point data
        fn load_las_file(path: &str) -> Result<CloudLoadResult, LoadError>;
        fn cloud_point_count(cloud: &CxxCloud) -> usize;
    }

    // ── Rust functions callable from C++ ──────────────────────────────
    extern "Rust" {
        // ScalarField operations (Phase 1)
        fn rust_scalar_mean(values: &[f32]) -> f64;
        fn rust_scalar_std(values: &[f32], mean: f64) -> f64;
        fn rust_scalar_min_max(values: &[f32]) -> (f32, f32);
        fn rust_scalar_valid_count(values: &[f32]) -> usize;
        fn rust_scalar_apply_offset(values: &mut [f32], offset: f64);

        // ICP / Registration (Phase 2)
        fn rust_icp_step(
            model_points: &[f32],
            data_points: &mut [f32],
            params: &rust::IcprParams,
        ) -> Result<rust::IcprResult, rust::IcprError>;

        // Octree (Phase 3)
        fn rust_build_octree(
            points: &[f32],
            max_level: u8,
        ) -> Result<rust::OctreeData, rust::OctreeError>;

        // Nearest neighbour (Phase 3)
        fn rust_kdtree_nearest(
            points: &[f32],
            query: &[f32; 3],
            k: usize,
        ) -> Vec<rust::NnResult>;
    }

    // ── Plain data structs crossing the FFI boundary ─────────────────
    struct CloudLoadResult {
        points: Vec<f32>,       // interleaved x,y,z,x,y,z,...
        count: usize,
        has_colors: bool,
        has_intensity: bool,
    }

    struct LoadError {
        code: i32,
        message: String,
    }
}

// ── Rust implementations ────────────────────────────────────────────────
// These are pure Rust. They are the actual migration targets.

// Phase 1: ScalarField
pub fn rust_scalar_mean(values: &[f32]) -> f64 {
    let mut sum = 0.0;
    let mut count = 0usize;
    for &v in values {
        if v.is_finite() {
            sum += v as f64;
            count += 1;
        }
    }
    if count == 0 { std::f64::NAN } else { sum / count as f64 }
}

pub fn rust_scalar_std(values: &[f32], mean: f64) -> f64 {
    let mut sum_sq = 0.0;
    let mut count = 0usize;
    for &v in values {
        if v.is_finite() {
            let d = v as f64 - mean;
            sum_sq += d * d;
            count += 1;
        }
    }
    if count < 2 { 0.0 } else { (sum_sq / (count - 1) as f64).sqrt() }
}

pub fn rust_scalar_min_max(values: &[f32]) -> (f32, f32) {
    let mut min = std::f32::INFINITY;
    let mut max = std::f32::NEG_INFINITY;
    for &v in values {
        if v.is_finite() {
            if v < min { min = v; }
            if v > max { max = v; }
        }
    }
    (min, max)
}

pub fn rust_scalar_valid_count(values: &[f32]) -> usize {
    values.iter().filter(|&&v| v.is_finite()).count()
}

pub fn rust_scalar_apply_offset(values: &mut [f32], offset: f64) {
    for v in values.iter_mut() {
        if v.is_finite() {
            *v = (*v as f64 + offset) as f32;
        }
    }
}

// Phase 2: ICP types (defined here, implemented in registration.rs)
pub struct IcprParams {
    pub max_iterations: u32,
    pub min_rms_decrease: f64,
    pub use_c2m_signed_distances: bool,
    pub norm_dist_threshold: f64,
}

pub struct IcprResult {
    pub rms: f64,
    pub delta_rms: f64,
    pub converged: bool,
    pub transform: Vec<f64>,  // 4x4 column-major
}

pub struct IcprError {
    pub code: i32,
    pub message: String,
}

// Phase 3: Octree types
pub struct OctreeData {
    pub levels: u8,
    pub cell_count: usize,
}

pub struct OctreeError {
    pub code: i32,
    pub message: String,
}

// Phase 3: Nearest neighbour
pub struct NnResult {
    pub index: usize,
    pub distance: f32,
}
```

---

### Step 6: Write `sandbox/cpp/las_loader.cc` — C++ LAS loader

This is the C++ side of the FFI bridge. It uses `FileIOFilter` (from `qCC_io`) to
load `.las` files without Qt, and exposes a CXX-safe point buffer to Rust.

```cpp
// cpp/las_loader.cc
// C++: loads a .las file via FileIOFilter and exposes point data to Rust.
// Qt-free: uses only CCCoreLib + qCC_io static libs.
// The .las file is an open-format export of the Faro scan data.

#include "FileIOFilter.h"           // CCCoreLib / qCC_io
#include "ccPointCloud.h"           // qCC_db
#include "ccHObject.h"              // qCC_db

#include "rust/cxx.h"

namespace sandbox {

struct LoadResult {
    std::vector<float> points;    // interleaved x,y,z
    size_t count = 0;
    bool has_colors = false;
    bool has_intensity = false;
    std::string error;
};

LoadResult load_las_file(const std::string& path) {
    LoadResult result;

    // FileIOFilter::Open() returns a ccHObject* tree
    // We cast to ccHObject* and look for ccPointCloud children
    ccHObject* container = FileIOFilter::Open(path);

    if (!container) {
        result.error = "Failed to open file: " + path;
        return result;
    }

    // Find the first point cloud in the loaded object tree
    ccHObject::Container clouds;
    container->filterChildren(clouds, true, "PointCloud");

    if (clouds.empty()) {
        result.error = "No point cloud found in file: " + path;
        delete container;
        return result;
    }

    ccPointCloud* cloud = static_cast<ccPointCloud*>(clouds[0]);
    size_t n = cloud->size();
    result.count = n;

    // Extract xyz into interleaved float buffer
    result.points.resize(n * 3);
    for (size_t i = 0; i < n; ++i) {
        const CCVector3* p = cloud->getPoint(i);
        result.points[i * 3 + 0] = static_cast<float>(p->x);
        result.points[i * 3 + 1] = static_cast<float>(p->y);
        result.points[i * 3 + 2] = static_cast<float>(p->z);
    }

    result.has_colors = cloud->hasColors();
    result.has_intensity = (cloud->getCurrentInScalarField() != nullptr);

    delete container;
    return result;
}

}  // namespace sandbox

// ── CXX bridge ──────────────────────────────────────────────────────────

::sandbox::LoadResult load_las_file(const ::std::string& path) {
    return ::sandbox::load_las_file(path);
}
```

---

### Step 7: Configure and build CCCoreLib for the sandbox

```powershell
# Create external/CCCoreLib as a symlink or copy of the actual sources
# (never modify this — it's read-only for the sandbox)

# Configure CCCoreLib standalone (no Qt, no plugins needed)
cmake -S C:\dev\CloudCompare\libs\qCC_db\extern\CCCoreLib `
      -B C:\dev\CloudCompare\sandbox\external\CCCoreLib_build `
      -G Ninja `
      -DCMAKE_BUILD_TYPE=Release `
      -DCCCORELIB_SHARED=OFF `
      -DCCCORELIB_USE_TBB=OFF `
      -DCCCORELIB_USE_QT_CONCURRENT=ON `
      -DCMAKE_PREFIX_PATH="C:/dev/tools/Qt/6.8.3/msvc2022_64"

# Build static CCCoreLib (this is what the sandbox links against)
cmake --build C:\dev\CloudCompare\sandbox\external\CCCoreLib_build `
      --config Release --parallel 8
```

This produces `sandbox/external/CCCoreLib_build/lib/CCCoreLib.lib` (static import lib).

---

### Step 8: Build the sandbox

```powershell
# From the sandbox directory:
cd C:\dev\CloudCompare\sandbox

# First configure CCCoreLib (Step 7 above must have run)
$env:CCCORELIB_BUILD = "C:\dev\CloudCompare\sandbox\external\CCCoreLib_build"

cargo build --release
```

If you hit linking errors about missing `.lib` files:
```powershell
# Add the Qt lib path if needed
$env:LIB = "C:\dev\tools\Qt\6.8.3\msvc2022_64\lib;" + $env:LIB
cargo build --release
```

---

### Step 9: Run with real data

```powershell
# Copy (or symlink) the .las files into sandbox/data/
New-Item -ItemType Junction -Path C:\dev\CloudCompare\sandbox\data\D82_BROOK `
          -Target "D:\82 BROOK AVENUE\output"

# Run the CLI
cargo run --release -- `
    las-load --file "data/D82_BROOK/2026-08-13-09-46-35_82 brook avenue.splice.las" `
    icp-bench --iterations 50 `
    --output "sandbox/results/scan1_bench.json"
```

The `.las` file is the open-format export (253 MB). It contains:
- XYZ coordinates
- Intensity values
- Possibly RGB colours
All loadable by CCCoreLib's `FileIOFilter`.

---

## Step-by-Step: Track A (link against existing DLLs)

If Track B fails, fall back to this. It uses the DLLs already built in `build/libs/`.

```powershell
# Step 1: Copy DLLs + lib files to sandbox/external/
New-Item -ItemType Directory -Force -Path C:\dev\CloudCompare\sandbox\external
Copy-Item C:\dev\CloudCompare\build\libs\CCCoreLib\*.dll C:\dev\CloudCompare\sandbox\external\
Copy-Item C:\dev\CloudCompare\build\libs\CCCoreLib\*.lib C:\dev\CloudCompare\sandbox\external\

# Step 2: Copy headers
New-Item -ItemType Directory -Force -Path C:\dev\CloudCompare\sandbox\external\include
Copy-Item C:\dev\CloudCompare\libs\qCC_db\extern\CCCoreLib\include\*.h `
          C:\dev\CloudCompare\sandbox\external\include\

# Step 3: Update build.rs to point at these copies
# Change rustc-link-search to sandbox/external
```

---

## Iteration Loop

```
┌─────────────────────────────────────────────────────────────┐
│  1. Edit Rust code in sandbox/src/                          │
│     e.g. sandbox/src/scalar_field.rs                        │
│                                                             │
│  2. cargo build --release                                   │
│     → compiles Rust + CXX bridge + links CCCoreLib.lib      │
│                                                             │
│  3. cargo test                                              │
│     → runs Rust unit tests                                  │
│                                                             │
│  4. cargo run --release -- --bench las-file                 │
│     → runs against real .las data                           │
│                                                             │
│  5. Check output / Sentry events                            │
│     → sentry events appear at https://sentry.io             │
│                                                             │
│  6. If good: copy the Rust impl into cc-rust/             │
│     (Phase 0+ scaffold from roadmap)                        │
│     If bad: delete and rewrite the module                   │
└─────────────────────────────────────────────────────────────┘
```

**Never edit the main CloudCompare code during sandbox iteration.**
Once a Rust module is stable in the sandbox, it graduates to `cc-rust/`
and the FFI integration into `CCCoreLib/CMakeLists.txt`.

---

## Sentry Integration (Rust)

Add `tracing-sentry` to `Cargo.toml` for Rust-side error capture:

```rust
// sandbox/src/main.rs
use tracing_sentry::{Registry, SentryLayer};
use tracing_subscriber::{fmt, prelude::*, EnvFilter};

fn main() {
    // Set up structured logging + Sentry
    let filter = EnvFilter::try_from_default_env()
        .unwrap_or_else(|_| EnvFilter::new("info"));

    let sentry_dsn = std::env::var("SENTRY_DSN")
        .unwrap_or_else(|_| "https://ac001120bfa96ba91d2ed97c62e632ad@o494653.ingest.us.sentry.io/4511938553053184".to_string());

    tracing_subscriber::registry()
        .with(filter)
        .with(fmt::layer())
        .with(SentryLayer::new(&sentry_dsn))
        .init();

    // Your CLI or test harness runs here
    let result = run_cli();

    // Flush Sentry before exit
    sentry::close();
}
```

For the C++ side, the existing `qCC/main.cpp` already has Sentry wired up. The sandbox C++ loader (`las_loader.cc`) can also log via `sentry_capture_message()` if you add sentry linkage to the sandbox build.

---

## Test Data Available

From `D:\82 BROOK AVENUE\output\`:

| File | Size | Format | Use for |
|------|------|--------|---------|
| `2026-08-13-09-46-35_82 brook avenue.splice.las` | 253 MB | Open LAS | **Primary test input** |
| `2026-08-13-09-46-35_82 brook avenue.splice.rcp` | 3 KB | ReCap project | Registration metadata (optional) |
| `2026-08-13-09-46-35_82 brook avenue.splice.rcs` | 165 MB | ReCap scan | Full proprietary scan (use .las instead) |
| `2026-08-13-09-46-35_82 brook avenue.splice.fjdata` | 10 MB | Faro internal | Skip |

**Use the `.las` files.** They are the open-format export of the same scan data.

Multiple scans available for ICP testing:
- `splice.las` — first pass
- `Continue scanning1.optimized1.las` — 166 MB, second pass
- `Continue scanning2.splice.optimized1.las` — 125 MB, third pass

---

## Test Harness Structure

```
sandbox/tests/
├── test_scalar_field_parity.rs   ← compare Rust vs C++ on synthetic data
├── test_icp_parity.rs           ← ICP: Rust vs C++ on synthetic data
├── test_icp_real_data.rs        ← ICP on real .las files
├── test_octree_build.rs         ← octree build correctness
└── test_kdtree_nearest.rs       ← nearest neighbour correctness
```

Example test:

```rust
// sandbox/tests/test_scalar_field_parity.rs
use cc_sandbox::{ffi, rust_scalar_mean};

#[test]
fn test_mean_matches_cpp() {
    let values: Vec<f32> = (0..10_000)
        .map(|i| (i as f32 * 0.001).sin())
        .collect();

    let rust_mean = rust_scalar_mean(&values);

    // Compare against C++ implementation via FFI
    let cpp_mean = unsafe {
        // Call CCCoreLib::ScalarFieldTools::computeMeanScalarValue
        // through the CXX bridge
        ffi::cxx_scalar_mean(values.as_ptr(), values.len())
    };

    assert!((rust_mean - cpp_mean).abs() < 1e-6);
}

#[test]
fn test_mean_nan_handling() {
    let values = vec![1.0f32, std::f32::NAN, 3.0, std::f32::INFINITY];
    let mean = rust_scalar_mean(&values);
    assert!(mean.is_nan() || mean.is_finite());
    // C++ NAN_VALUE handling should be equivalent
}
```

---

## Phase-by-Phase Sandbox Tasks

### Sandbox Phase 1: ScalarField parity (week 1)
- [ ] Set up Track A or Track B
- [ ] `las_loader.cc` compiles and links CCCoreLib
- [ ] Load `.las` file, print point count + bounding box
- [ ] Implement `rust_scalar_mean` in pure Rust
- [ ] Add characterisation test: compare Rust vs C++ `computeMeanScalarValue`
- [ ] Characterise all 4–6 ScalarField methods
- [ ] Log results to `sandbox/results/scalar_field_parity.json`

### Sandbox Phase 2: ICP on real data (weeks 2–3)
- [ ] Implement pure-Rust ICP (SVD-based Horn)
- [ ] Load two `.las` files (scan1 + scan2)
- [ ] Run C++ ICP via FFI, capture RMS curve
- [ ] Run Rust ICP, compare RMS curve
- [ ] Sentry capture: log ICP convergence events
- [ ] Output: `sandbox/results/icp_scan1_vs_scan2.json`

### Sandbox Phase 3: Octree + KdTree (weeks 3–4)
- [ ] Implement Rust octree build
- [ ] Compare tree structure (cell count, Morton codes) with C++ DgmOctree
- [ ] Implement KdTree nearest neighbour
- [ ] Compare query results with C++ KDTree
- [ ] Profile: Rust vs C++ timing on 1M-point dataset

### Sandbox Phase 4: Graduated to cc-rust/ (ongoing)
- Once a module passes characterisation + profiling in sandbox,
  copy it to `cc-rust/src/` and add the CMake integration from `PRD/rust/05-roadmap.md`

---

## Troubleshooting

### "cannot open CCCoreLib.lib"
Track A: the DLLs aren't on the linker's search path.
```powershell
$env:LIB = "C:\dev\CloudCompare\sandbox\external;" + $env:LIB
cargo build --release
```
Track B: run the CMake configure step (Step 7) first.

### "entry point not found" at runtime
The CCCoreLib DLL was built with a different MSVC runtime.
Install the matching VC++ Redistributable, or rebuild CCCoreLib from source (Track B).

### CXX can't find headers
Check the include paths in `build.rs`. The `.include(...)` calls must point to the actual header directories.
```rust
// Must match where the headers actually live:
let include_dirs = [
    sandbox_root.join("external/CCCoreLib/include"),
];
```

### .las file won't load
The LAS reader needs LASzip. Check that `qLASIO` plugin is enabled in the main build, then copy `libLASzip.dll` next to `CCCoreLib.dll` in the sandbox external folder.

---

## Sentry Project

Sandbox uses the **same Sentry DSN** as the main app:
`https://ac001120bfa96ba91d2ed97c62e632ad@o494653.ingest.us.sentry.io/4511938553053184`

Release name for sandbox builds:
```
cloudcompare-sandbox@0.1.0
```
This separates sandbox events from production in the Sentry dashboard.

---

## Summary: What's Needed from You

| # | Task | Time | What you do |
|---|------|------|-------------|
| 1 | Install Rust (MSVC target) | 5 min | `rustup default stable` |
| 2 | Create sandbox dirs | 2 min | Agent creates |
| 3 | Configure CCCoreLib for sandbox | 10 min | Agent writes CMake + runs configure |
| 4 | Write sandbox code | Agent | Agent writes `lib.rs`, `build.rs`, C++ loader |
| 5 | Build + fix errors | Iterative | Agent fixes build errors; you run `cargo build` |
| 6 | Run with real data | 2 min | `cargo run -- las-load ...` |
| 7 | Check Sentry | 1 min | https://sentry.io — filter by `cloudcompare-sandbox` |
| 8 | Graduate to `cc-rust/` | When stable | Agent copies sandbox module → `cc-rust/src/` |

**The entire sandbox is gitignored.** It's yours to break, rewrite, and try wild things in.
