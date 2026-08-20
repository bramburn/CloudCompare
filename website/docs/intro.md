---
id: intro
title: Introduction
slug: /
sidebar_position: 1
---

# CloudCompare

CloudCompare is an open-source 3D point cloud and triangular mesh processing
tool. It was originally designed to perform a **cloud-to-cloud** or
**cloud-to-mesh** distance comparison on the data a terrestrial laser scanner
produces, and to do it efficiently on dense scans (tens to hundreds of millions
of points) by streaming the work through an octree.

This site documents the **Icelabz Surveying fork** at
[`bramburn/CloudCompare`](https://github.com/bramburn/CloudCompare). The fork
tracks the upstream project at
[`CloudCompare/CloudCompare`](https://github.com/CloudCompare/CloudCompare)
and adds:

- **A Rust migration path** — `cc-rust/` is a Cargo workspace
  with ported ScalarField, ICP, DgmOctree, coarse pre-alignment,
  multi-resolution ICP, and a pure-Rust LAS reader. 43 unit tests
  pass, validated end-to-end on the brook-avenue 7.5M-point `.las`.
  The CXX FFI bridge to call the Rust from the C++ side is
  opt-in via `cargo build --features cxx-ffi`. See the
  [Rust migration](fork/rust-migration) page.
- **An experimental workspace** — `experimental/` is a sandboxed
  area for new features (templates, sessions, scenarios, an
  8-state lifecycle, a promotion gate). Every non-trivial idea
  is prototyped here before touching `qCC/`, `ccViewer/`, or
  `libs/`. See the [Experimental workspace](fork/experimental-workspace)
  page.
- **Sentry crash reporting** — opt-in via `qCC/CC_USE_SENTRY=ON`
  and the `SENTRY_DSN` env var. `sentry.dll` is copied to the
  deploy bundle by CMake POST_BUILD.
- **A pinned, opinionated build toolchain** (CMake 4.3, Ninja,
  Qt 6.8.3, MSVC 14.44, vcpkg for plugin dependencies). The
  wrapper scripts `cc-configure.cmd` and `cc-build.cmd` pin the
  exact invocation. The build is reproducible.
- **A self-contained Windows build artifact**
  (`build/qCC/deployqt/CloudCompare.exe` with all Qt 6 runtime
  DLLs and the Sentry DLL bundled) — copy to any Windows x64
  box, runs.
- **A slim GitHub Actions matrix** that mirrors the local
  16-plugin set so the downloadable artifact always matches
  the docs.
- **An extensive Doxygen pass** — approaching 400 Doxygen-clean
  commits for an AI-readable API surface.
- **This docs site**, published to GitHub Pages.

See the full [fork vs upstream comparison](fork/upstream-vs-fork)
and the central [Why this fork?](fork/index) page.

## What this site is and isn't

This is **not** the upstream CloudCompare Wiki. For API reference, command
line flags, the `ccCommandLineParser` contract, and the full plugin catalogue,
go to [cloudcompare.org](https://cloudcompare.org) and the upstream Wiki.

This **is** the place to look up:

- The exact `cmake` invocation used to build the fork on Windows, Linux, and
  macOS.
- Which plugins are enabled in this fork, which are deliberately disabled, and
  what external dependency each disabled plugin needs.
- How to add a new Standard / I/O / GL plugin (the cookbook that mirrors
  `plugins/example/`).
- The architecture map (`ccHObject`, `ccMainAppInterface`,
  `ccPluginManager`, `FileIOFilter`) and which library owns what.
- The CI workflows, how to trigger them, and where their artifacts land.

## Where to start

- [Getting started](/docs/getting-started/overview) — clone, build, run, and verify.
- [Build on Windows](/docs/build/windows) — the local toolchain and the wrapper
  scripts at `C:\dev\tools\`.
- [Plugin overview](/docs/plugins/overview) — the model, the local set, and how to
  add a new one.
- [Architecture](/docs/architecture/overview) — if you're here to read code.
- [CI & deployment](/docs/ci/overview) — the GitHub Actions workflows that ship the
  Windows bundle and this docs site.

## License

The fork is a derivative of the upstream project, so the same licenses apply:

- `qCC/`, `ccViewer/`: **GPLv2-or-later**.
- `CCCoreLib`, `qCC_db`, `qCC_io`, `qCC_gl`: **LGPLv2-or-later**.

See [`license.txt`](https://github.com/CloudCompare/CloudCompare/blob/master/license.txt)
in the upstream repo for the canonical text.
