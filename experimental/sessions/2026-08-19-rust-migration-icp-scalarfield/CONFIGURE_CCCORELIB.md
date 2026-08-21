# Configuring CCCoreLib for the CXX FFI (D10 — August 2026)

> **Historical doc, updated 2026-08-21.** This file was originally
> written for the `cc-sandbox` crate at
> `experimental/sessions/2026-08-19-rust-migration-icp-scalarfield/`
> (D0–D6, the original Rust work). The CXX FFI was **never
> actually wired** into that crate — the work moved to the
> canonical crate at [`../../../cc-rust/`](../../../cc-rust/)
> in D7 and the CXX FFI was added there in D10.
>
> The approach described in the original **Option A** (copy
> `CCCoreLib.lib` and `.dll` into a sandbox-side directory) was
> abandoned because the binary copy fell out of sync with the
> source. The original **Option B** (standalone CCCoreLib CMake
> build) was **never shipped** — it turned out to be unnecessary
> because the main CloudCompare build already produces the
> artifacts we need.
>
> **What actually works (D10):** link the Rust crate **directly**
> against the existing CloudCompare build's
> `build/libs/qCC_db/extern/CCCoreLib/CCCoreLib.{lib,dll}`.
> No copy step, no standalone CMake target. See the
> `cc-rust/build.rs` for the exact link lines.

This document is kept here as a historical record of the **paths
that were tried** and **why they were abandoned**. For the
working setup, see [`../../../cc-rust/AGENTS.md`](../../../cc-rust/AGENTS.md)
and [`../../../cc-rust/build.rs`](../../../cc-rust/build.rs).

## Quick start (D10 — the working path)

```powershell
# 1. Build CloudCompare once (produces the CCCoreLib artifacts
#    we'll link against). The configure step is the same one
#    used for the GUI; it lives in tools/.
& 'C:\dev\CloudCompare\tools\cc-configure.cmd'
& 'C:\dev\CloudCompare\tools\cc-build.cmd'

# 2. Build the Rust crate with the cxx_ffi feature. The build
#    script picks up CCCoreLib.lib from
#    <repo>/build/libs/qCC_db/extern/CCCoreLib/ and copies
#    CCCoreLib.dll next to the test binary at build time.
Set-Location C:\dev\CloudCompare\cc-rust
cargo test --release --features cxx_ffi

# Expected: 57/57 tests pass (54 pure-Rust + 3 CXX parity tests).
```

The CXX bridge itself is in
[`cc-rust/src/ffi.rs`](../../../cc-rust/src/ffi.rs) (the
`#[cxx::bridge]` module) and the C++ shim in
[`cc-rust/src/cpp/icp_shim.{h,cc}`](../../../cc-rust/src/cpp/icp_shim.h)
(constructs `CCCoreLib::PointCloud` from flat f32 arrays,
calls `ICPRegistrationTools::Register`, packs the result).
The build script at
[`cc-rust/build.rs`](../../../cc-rust/build.rs) wires it all
together.

Override the CCCoreLib path with `CCCORELIB_DIR=<path>` env var
if your build is in a non-standard location.

## Why the original "copy into a sandbox-side dir" approach was abandoned

The D0–D6 `cc-sandbox` crate used a per-sandbox `external/`
directory that received copies of `CCCoreLib.lib`,
`CCCoreLib.dll`, and the headers. The problems:

1. **Headers drifted.** A `git pull` that changed
   `libs/qCC_db/extern/CCCoreLib/include/` left the sandbox
   headers out of date until someone remembered to re-copy.
2. **The DLL was sometimes rebuilt with a different MSVC toolchain
   than the one Rust was using.** The C++ static lib and the Rust
   `staticlib` both had to be built with the same `/MT` vs `/MD`
   flag and the same Windows SDK version; the copy step made that
   easy to get wrong.
3. **The standalone CCCoreLib CMake target (Option B) was a
   30-minute detour** for the same end result — `CCCoreLib.lib`
   and `.dll` exist in the main build's output tree.

The D10 approach avoids all three:

- **No copy step** — the Rust build script reads the
  CloudCompare build's `build/` tree directly.
- **No drift** — `cargo:rerun-if-changed=...` lines in
  `build.rs` make cargo rebuild the FFI artifacts when
  CCCoreLib's headers or `CCCoreLib.lib` change.
- **No toolchain mismatch** — the same MSVC build that produces
  `CCCoreLib.lib` is the one the CXX glue runs in.

## What about `qCC_io.lib` and `QCC_DB_LIB.lib`?

The D0–D6 sandbox doc recommended linking all three
(CCCoreLib + QCC_DB_LIB + QCC_IO_LIB) so the LAS loader
stub (`cpp/las_loader.cc`) could call `FileIOFilter::Open()`
in `qCC_io.lib`. The D10 work **does not need either of those**:

- D10 calls `CCCoreLib::ICPRegistrationTools::Register` directly
  with `CCCoreLib::PointCloud` (not the qCC_db `ccPointCloud`).
  `PointCloud` is in `CCCoreLib.lib`, full stop.
- The LAS loader is out of scope for D10. The D6 decision
  (pure-Rust `las` crate, 7.2M pts/s) replaced the C++ LAS
  loader with a Rust implementation that doesn't need
  `qCC_io.lib` at all.

If a future task does need `qCC_io.lib` (e.g. wiring the
Rust ICP into a CloudCompare plugin that loads .las files
through the existing pipeline), the build script in
`cc-rust/build.rs` is the right place to add the extra
`rustc-link-lib` line. The CCCoreLib link line is the model:

```rust
println!("cargo:rustc-link-search=native={}", ccbuild.display());
println!("cargo:rustc-link-lib=CCCoreLib");
// For qCC_io:
println!("cargo:rustc-link-search=native={}", qcc_io_build.display());
println!("cargo:rustc-link-lib=QCC_IO_LIB");
```

## Verifying the setup (D10)

```powershell
# CCCoreLib.lib should exist (built by the main CloudCompare build)
Test-Path "C:\dev\CloudCompare\build\libs\qCC_db\extern\CCCoreLib\CCCoreLib.lib"

# CCCoreLib.dll should exist (for runtime)
Test-Path "C:\dev\CloudCompare\build\libs\qCC_db\extern\CCCoreLib\CCCoreLib.dll"

# Header <RegistrationTools.h> should be at:
Test-Path "C:\dev\CloudCompare\libs\qCC_db\extern\CCCoreLib\include\RegistrationTools.h"

# Build script output dir (the CXX static lib) should be writable:
Test-Path "C:\dev\CloudCompare\cc-rust\target\release\build"
```

## If linking fails (D10)

| Error | Fix |
|-------|-----|
| `'cc_rust' is not a member of 'global namespace'` | The `#[cxx::bridge]` module is being dropped silently. Check the cargo feature name (P19): use `cxx_ffi` not `cxx-ffi` — cxx-build's `CargoEnvCfgEvaluator` doesn't normalise `-` to `_`. |
| `cxxbridge: no #[cxx::bridge] module found in src/ffi.rs` | The bridge file is at `src/ffi.rs` but `cxx_build::bridge("src/lib.rs")` is the wrong path. Fix in `build.rs`. |
| `error: pointer argument requires that the function be marked unsafe` | Mark the CXX FFI function as `unsafe fn` (P — pointer args are unsafe by default in cxx 1.0.199). |
| `'CCCoreLib.h': No such file or directory` | Add `.include("../libs/qCC_db/extern/CCCoreLib/include")` to the `cxx_build::bridge()` builder in `build.rs`. |
| `'CCCoreLibExport.h': No such file or directory` | Add the generated-exports include dir: `.include("../build/libs/qCC_db/extern/CCCoreLib/exports")`. |
| `'CCVector3' is not a member of 'CCCoreLib'` / `'SquareMatrixTpl': no operator()` | `CCVector3` is at the **global namespace** (P22), not `CCCoreLib::`. `SquareMatrixTpl` uses `getValue(row, col)` and `setValue(row, col, val)` (P23), not `operator()`. |
| `PointCloud::PointCloud`: no overloaded function takes a string | `CCCoreLib::PointCloud` has only a default ctor (P24). Drop the name argument. |
| STATUS_ACCESS_VIOLATION (0xC0000005) reading `R` or `T` of a returned `Transformation` | The C++ `Transformation()` ctor only inits `s=1.0`; `R` and `T` are uninitialised. Pre-init them in the shim (P21). |
| Test binary STATUS_DLL_NOT_FOUND (0xC0000135) at startup | Build script link directives not reaching test target. Add `links = "cc_rust_ffi"` to `[package]` in `Cargo.toml` (P20). |
| Cannot open `kernel32.lib` | Need vcvars64.bat on PATH. Run `& 'C:\dev\CloudCompare\tools\cc-configure.cmd'` first, or use the `MSYS2` MinGW shell only for `cargo check` (which doesn't link). |
| MSVC runtime mismatch | Don't mix `CCCoreLib.dll` from one build with `cc_rust_ffi.lib` from another. Both must be built with the same MSVC toolchain. |

## See also

- [`cc-rust/AGENTS.md`](../../../cc-rust/AGENTS.md) — canonical
  build instructions for the `cc-rust` crate.
- [`cc-rust/build.rs`](../../../cc-rust/build.rs) — the actual
  CXX build glue.
- [`cc-rust/src/ffi.rs`](../../../cc-rust/src/ffi.rs) — the
  `#[cxx::bridge]` module.
- [`cc-rust/src/cpp/icp_shim.{h,cc}`](../../../cc-rust/src/cpp/icp_shim.h) — the C++ shim.
- [`../../docs/SUMMARY.md`](../../docs/SUMMARY.md) — Phase 0
  marked done (D10); CXX FFI parity tests added.
- [`../../docs/decisions.md`](../../docs/decisions.md) — D10.
- [`../../docs/patterns.md`](../../docs/patterns.md) — P19–P24.
