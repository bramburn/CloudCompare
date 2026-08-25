---
title: Upstream vs fork
sidebar_label: Upstream vs fork
sidebar_position: 4
---

# Upstream vs fork

A feature-by-feature comparison. Upstream column links to the
relevant upstream wiki / repo. Fork column links to the
fork-specific docs / code.

| Concern | Upstream `CloudCompare/CloudCompare` | Fork `bramburn/CloudCompare` |
|---|---|---|
| **C++/Qt codebase** (`qCC/`, `ccViewer/`, `libs/`, `plugins/`) | ✓ canonical, full plugin set, sparse Doxygen | identical (synced regularly), + Doxygen pass (~400 commits), slim 16-plugin local set |
| **Build on Windows** | generic CMake, any modern MSVC | pinned MSVC 14.44, CMake 4.3, Qt 6.8.3, Ninja, vcpkg, all under `C:\dev\tools\`; reproducible via `tools/cc-configure.cmd` and `tools/cc-build.cmd` |
| **Build on Linux** | generic CMake, any modern GCC | identical to upstream, documented in [Build on Linux](/docs/build/linux) |
| **Build on macOS** | generic CMake, any modern Clang | **Community-supported, not CI-validated** since 2026-08-24. Documented in [Build on macOS](/docs/build/macos) for local builds. |
| **Self-contained Windows bundle** (`build/qCC/deployqt/CloudCompare.exe`) | no | yes — Qt 6 runtime DLLs, plugin DLLs, and `sentry.dll` all alongside the `.exe`; copy to any Windows x64 box, runs |
| **Docusaurus docs site** | no (Wiki only) | yes — this site, deployed to GitHub Pages, mirrors the fork's configuration |
| **Doxygen pass** | sparse `///` comments | extensive — approaching 400 Doxygen-clean commits across `qCC/`, `ccViewer/`, `libs/`, `plugins/` |
| **CI** (`.github/workflows/`) | full upstream matrix (Windows + macOS + Ubuntu) | fork-owned slim matrix: Windows MSVC only; macOS and Linux dropped 2026-08-24 |
| **Rust migration** | — (no Rust) | ✓ `cc-rust/` Cargo workspace with Phase 0–4 work, 43 unit tests, end-to-end validated on the brook-avenue 7.5M-point `.las` |
| **Experimental workspace** | — (no sandbox) | ✓ `experimental/` with 4 templates, ~15 sessions, 4 scenarios, an 8-state lifecycle, a promotion gate |
| **Sentry crash reporting** | — | ✓ opt-in via `qCC/CC_USE_SENTRY=ON` + `SENTRY_DSN` env var; `sentry.dll` copied to the deploy bundle by CMake POST_BUILD |
| **Plugin set** | 30+ (default catalogue) | 16 enabled (local set) + 20+ disabled (recipe per plugin in [Disabled priority](/docs/plugins/disabled-priority)) |
| **LAS support** | `qLASIO` plugin (uses `LASzip` C++) | `qLASIO` + a pure-Rust `las` crate in `cc-rust/` (D6: 7.2 M pts/s on real data; LASzip-free CXX FFI planned) |
| **ICP / registration** | `RegistrationTools` in CCCoreLib (C++) | CCCoreLib (C++, unchanged) + a corrected Rust ICP in `cc-rust/` with trimmed ICP, multi-resolution, and PCA coarse pre-alignment (D4 fixes; 8 ICP tests pass on the asymmetric-9 fixture) |
| **Octree / NN** | `DgmOctree` in CCCoreLib (C++) | CCCoreLib (C++, unchanged) + cell-code core in `cc-rust/src/dgm_octree.rs` (D3, 5 tests) + the D8 trait + `icp_with_nn` entry point that lets any NN plug in |
| **Surveying workflows** (topo, monitoring, stockpile, as-built) | not in upstream Wiki | ✓ the four canonical pipelines are documented in [`/docs/workflows/`](/docs/workflows/) |

## What the fork does NOT add

To keep the diff against upstream small and the sync clean, the
fork explicitly **does not** add:

- New C++/Qt features in `qCC/`, `ccViewer/`, or `libs/`. All
  C++/Qt code is upstream-tracked.
- A new plugin model. The plugin contract (`ccPluginInterface`,
  `IStandardPlugin`, `IFileIOFilter`, `IGLPlugin`) is identical
  to upstream. The fork just exercises the contract with a
  16-plugin local set.
- A different C++/Qt coding standard. The fork follows the
  upstream [CONTRIBUTING.md](https://github.com/CloudCompare/CloudCompare/blob/master/CONTRIBUTING.md)
  for any C++/Qt change.
- A mandatory Rust dependency. `cc-rust/` is built and tested
  separately. The CXX FFI bridge to call it from C++ is
  opt-in via `cargo build --features cxx-ffi`. The default
  build of the fork is pure upstream C++/Qt.
- A redistribution of upstream binary artifacts. The Windows
  bundle is the output of the documented local build, not a
  redistributable.

## What the fork asks of upstream

The fork pushes back to upstream via PR:

- Any general C++/Qt fix that lands in `bramburn/master` and is
  not fork-specific.
- Doxygen pass commits (per the fork's Doxygen initiative).
- ICP / ScalarField bug findings from the Rust port (the D4
  series of fixes, the parallel-statistics strategy, the LAS
  parser selection).
- Disabled-plugin fixes (e.g. when a Qt 6.8.3 incompatibility
  in `qCompass`/`qRANSAC_SD`/`qSRA` is resolved upstream).

The fork **never** upstream anything Rust-specific, anything
experimental, or anything documentation-site-specific.

## Sync cadence

`git pull --ff-only origin master` brings in upstream changes
on a regular cadence (typically weekly). Conflicts in
`qCC/`, `ccViewer/`, `libs/`, or `plugins/` are resolved by
taking upstream. Conflicts in `cc-rust/`, `experimental/`,
`website/`, `AGENTS*.md`, `BUILD-LOCAL.md`, or
`tools/cc-*.cmd` are resolved by taking the fork.
