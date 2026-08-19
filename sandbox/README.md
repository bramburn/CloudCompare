# CloudCompare Rust Sandbox

**Throwaway experimental workspace.** All code here is temporary.
Nothing is committed to git. When something works, it graduates to `../cc-rust/`.

## Quick Start

```powershell
# Step 1: Configure CCCoreLib for the sandbox (once)
& 'C:\dev\CloudCompare\tools\cc-configure.cmd'   # run main build first

# Step 2: Copy / symlink CCCoreLib headers to sandbox
Copy-Item C:\dev\CloudCompare\libs\qCC_db\extern\CCCoreLib\include\* `
         C:\dev\CloudCompare\sandbox\external\CCCoreLib_include\ -Recurse -Force
New-Item -ItemType Directory -Force -Path C:\dev\CloudCompare\sandbox\external\CCCoreLib_build\lib
Copy-Item C:\dev\CloudCompare\build\libs\qCC_db\extern\CCCoreLib\*.lib `
         C:\dev\CloudCompare\sandbox\external\CCCoreLib_build\lib\

# Step 3: Try a Rust-only build first (no C++ needed)
cd C:\dev\CloudCompare\sandbox
cargo build --release   # pure-Rust modules should compile

# Step 4: Add C++ loader (requires CCCoreLib headers + lib)
# See CONFIGURE_CCCORELIB.md for full steps
```

## Directory Layout

```
sandbox/
├── src/
│   ├── main.rs              ← CLI entry point
│   ├── lib.rs               ← CXX bridge + module declarations
│   ├── scalar_field.rs      ← Phase 1: pure-Rust ScalarField (✓ builds standalone)
│   ├── registration.rs      ← Phase 2: pure-Rust ICP
│   ├── octree.rs            ← Phase 3: pure-Rust KdTree + Octree
│   └── io.rs                ← Phase 4: CSV loader (pure Rust)
├── cpp/
│   └── las_loader.cc        ← C++: LAS loader via FileIOFilter (needs CCCoreLib)
├── external/
│   ├── CCCoreLib_include/   ← copy of CCCoreLib headers
│   └── CCCoreLib_build/    ← built CCCoreLib (from main build)
│       └── lib/
│           └── CCCoreLib.lib ← static import lib
├── tests/
│   ├── test_scalar_field_parity.rs   ← Rust vs C++ comparison
│   └── test_icp_real_data.rs        ← ICP on real .las files
└── data/
    └── D82_BROOK/           ← symlinks to survey data
```

## Running Tests

```powershell
# Pure-Rust unit tests (no C++ needed)
cd C:\dev\CloudCompare\sandbox
cargo test

# With CXX FFI (requires external/ to be set up)
cargo test --features ffi

# Benchmarks
cargo bench
```

## CLI Commands

```powershell
# Load a LAS file and print stats
cargo run --release -- las-load --file "D:\82 BROOK AVENUE\output\scan.las"

# ICP between two scans
cargo run --release -- icp-bench `
    --model "D:\82 BROOK AVENUE\output\scan1.las" `
    --data "D:\82 BROOK AVENUE\output\scan2.las" `
    --iterations 50 `
    --output "results/icp.json"
```

## Test Data

| File | Size | Format |
|------|------|--------|
| `D:\82 BROOK AVENUE\output\2026-08-13-09-46-35_82 brook avenue.splice.las` | 253 MB | Open LAS |
| `D:\82 BROOK AVENUE\output\2026-08-13-09-57-20_82 brook avenue -Continue scanning1.optimized1.las` | 166 MB | Open LAS |

Use `.las` files (open format). Do not use `.rcs` (proprietary, 165 MB).
The `.las` files are the same scan data exported from Faro Scene.

## Sentry

Sandbox uses the same Sentry DSN as the main app:
`https://ac001120bfa96ba91d2ed97c62e632ad@o494653.ingest.us.sentry.io/4511938553053184`

Release name: `cloudcompare-sandbox@0.1.0`
Filter in Sentry dashboard by `release:cloudcompare-sandbox@*`.

## Graduating Code

When a module passes all tests:

1. characterisation tests pass (Rust = C++)
2. benchmarks are within ±10% of C++
3. Sentry is clean

→ Copy the Rust module to `../cc-rust/src/` and add CMake integration per `PRD/rust/05-roadmap.md`.

## Troubleshooting

| Error | Fix |
|-------|-----|
| `cannot find -lCCCoreLib` | Set up `external/CCCoreLib_build/lib/CCCoreLib.lib` |
| `cannot find ScalarField.h` | Copy headers to `external/CCCoreLib_include/` |
| `error: unknown type name 'ccPointCloud'` | C++ headers not on include path |
| CXX "unrecognized token" | C++ standard mismatch — set `/std:c++17` in `build.rs` |
