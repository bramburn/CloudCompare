CloudCompare
============

Homepage: https://cloudcompare.org

[![GitHub release](https://img.shields.io/github/release/cloudcompare/trunk.svg)](https://github.com/cloudcompare/trunk/releases)

[![Build](https://github.com/bramburn/CloudCompare/workflows/Windows%20Build/badge.svg?branch=master)](https://github.com/bramburn/CloudCompare/actions?query=workflow%3A%22Windows+Build%22)

[![Docs](https://img.shields.io/badge/docs-bramburn.github.io-blue)](https://bramburn.github.io/CloudCompare/)

> **This is the Icelabz Surveying fork** at
> [`bramburn/CloudCompare`](https://github.com/bramburn/CloudCompare).
> It tracks the upstream
> [`CloudCompare/CloudCompare`](https://github.com/CloudCompare/CloudCompare)
> and adds a pinned toolchain, a self-contained Windows bundle, a
> slim CI matrix, and a Docusaurus docs site.

## Why this fork?

The fork exists for one specific reason: **to add Rust to CloudCompare,
incrementally, without breaking the upstream sync**. Everything else flows
from that.

| Fork adds | Upstream has | Why the fork does it |
|---|---|---|
| **`cc-rust/`** Cargo workspace — Rust ports of ScalarField, ICP, DgmOctree, coarse pre-alignment, multi-resolution ICP, LAS reader, with 40+ Rust unit tests | — (no Rust in upstream) | The Rust migration is the headline. Phase-by-phase ports in pure Rust, validated against CCCoreLib on real `.las` data, with a CXX FFI bridge planned to call the Rust from the C++ side. |
| **`experimental/`** workspace with templates, sessions, scenarios, an 8-state lifecycle, and a `promotion.md` gate | — (no sandbox) | New features (including all Rust work) are prototyped in isolation before touching `qCC/`, `ccViewer/`, or the vendored libs. The status state machine forces measurement over optimism. |
| **Sentry crash reporting** (`qCC/CC_USE_SENTRY=ON`, opt-in via `SENTRY_DSN`) | — | Field-deployable crash diagnostics for the surveying workflow. |
| **Pinned toolchain**: CMake 4.3, Ninja, Qt 6.8.3, MSVC 14.44, vcpkg for plugin deps, all under `C:\dev\tools\` | Generic "any modern toolchain" | The Windows build is reproducible. No "works on my machine". The wrapper scripts at `tools/cc-configure.cmd` and `tools/cc-build.cmd` pin the exact CMake invocation. |
| **Self-contained Windows bundle** at `build/qCC/deployqt/CloudCompare.exe` (Qt 6 + plugin DLLs + sentry.dll all alongside) | Generic CMake build | Copy the folder to any Windows x64 box; it runs. No PATH, no registry, no install. |
| **16-plugin local set** (deliberately slim) | Full 30+ plugin catalogue | The fork opts out of plugins that need heavy external deps (PCL, FBX, OpenCASCADE, OpenCV) and three Qt 6.8.3-incompatible plugins (qCompass, qRANSAC_SD, qSRA). Each disabled plugin has a documented recipe in [`docs/fork/disabled-priority.md`](docs/fork/disabled-priority.md). |
| **Docusaurus docs site** (this site, deployed to GitHub Pages) | Wiki only | The fork has a typed architecture map, a per-OS build guide, a cookbook of task recipes, and the four canonical surveying-company workflows (topo, monitoring, stockpile, as-built). |
| **Extensive Doxygen pass** (the upstream-default `///` comments are sparse) | Sparse Doxygen | The fork is approaching 400 Doxygen-clean commits for an AI-readable API surface. |
| **Slim GitHub Actions matrix** that mirrors the local plugin set | Full matrix | CI builds the same 16 plugins that ship locally, so the downloadable artifact matches the docs. |

**The fork syncs with upstream regularly** (`git pull --ff-only origin master`),
keeps the C++/Qt code unmodified in spirit, and pushes any general C++/Qt
fixes back via PR. The Rust migration, the experimental workspace, and
the docs site stay on `bramburn/master` only.

## Fork-only deep-dives

- [Why this fork](docs/fork/index.md) — the central page on fork vs upstream
- [Rust migration](docs/fork/rust-migration.md) — what's ported, what's tested, what's next
- [Experimental workspace](docs/fork/experimental-workspace.md) — the sandbox for new features
- [Disabled-plugin priority](docs/fork/disabled-priority.md) — which plugins are off by default and how to re-enable
- [Upstream vs fork](docs/fork/upstream-vs-fork.md) — feature-by-feature comparison table

## Documentation

The full docs site is at
**[bramburn.github.io/CloudCompare](https://bramburn.github.io/CloudCompare/)**.
It is built from the [`website/`](website/) directory by
[`.github/workflows/deploy-docs.yml`](.github/workflows/deploy-docs.yml)
on every push to `master`.

Start here:

- [Getting started](https://bramburn.github.io/CloudCompare/docs/getting-started/overview)
  — clone, build, run, verify.
- [Build on Windows](https://bramburn.github.io/CloudCompare/docs/build/windows)
  — the local toolchain and the wrapper scripts at `C:\dev\tools\`.
- [Plugin overview](https://bramburn.github.io/CloudCompare/docs/plugins/overview)
  — the model, the local 18-plugin set, the disabled-priority list.
- [Architecture](https://bramburn.github.io/CloudCompare/docs/architecture/overview)
  — `ccHObject`, `ccPluginManager`, the layer map.
- [CI & deployment](https://bramburn.github.io/CloudCompare/docs/ci/overview)
  — the GitHub Actions workflows and how to trigger them.
- [FAQ](https://bramburn.github.io/CloudCompare/docs/reference/faq)
  — quick answers to the questions that show up repeatedly.

For the **agent-facing** reference (the canonical entry point for AI
coding agents working in this repo), read
[`AGENTS.md`](AGENTS.md). It points at the topical deep-dives in
`AGENTS-*.md`.

For the **long-form** local-build narrative (every issue hit, every
path, every workaround), read
[`BUILD-LOCAL.md`](BUILD-LOCAL.md).

## Introduction

CloudCompare is a 3D point cloud (and triangular mesh) processing software.
It was originally designed to perform comparison between two 3D points clouds
(such as the ones obtained with a laser scanner) or between a point cloud and a
triangular mesh. It relies on an octree structure that is highly optimized for
this particular use-case. It was also meant to deal with huge point
clouds (typically more than 10 million points, and up to 120 million with 2 GB
of memory).

More on CloudCompare [here](http://en.wikipedia.org/wiki/CloudCompare)

## License
------------

This project is under the GPL license: https://www.gnu.org/licenses/gpl-3.0.html

This means that you can use it as is for any purpose. But if you want to distribute
it, or if you want to reuse its code or part of its code in a project you distribute,
you have to comply with the GPL license. In effect, all the code you mix or link with
CloudCompare's code must be made public as well. **This code cannot be used in a
closed source software**.

## Installation
------------

Linux:
- Flathub: https://flathub.org/apps/details/org.cloudcompare.CloudCompare
  ```
  flatpak install flathub org.cloudcompare.CloudCompare
  ```


## Compilation
-----------

Supports: Windows, Linux, and macOS

Refer to the [BUILD.md file](BUILD.md) for up-to-date information.

Basically, you have to:
- clone this repository
- install mandatory dependencies (OpenGL,  etc.) and optional ones if you really need them
(mainly to support particular file formats, or for some plugins)
- launch CMake (from the trunk root)
- enjoy!

## Contributing to CloudCompare
----------------------------

If you want to help us improve CloudCompare or create a new plugin you can start by reading this [guide](CONTRIBUTING.md)

## Supporting the project
--------------------

If you want to help us in another way, you can make donations via <a href='https://donorbox.org/support-cloudcompare' target="_blank"><img src="https://donorbox.org/images/red_logo.png"></a> [donorbox](https://donorbox.org/support-cloudcompare)

Thanks!
