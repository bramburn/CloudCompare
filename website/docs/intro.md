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

- A pinned, opinionated build toolchain (CMake 4.3, Ninja, Qt 6.8, MSVC 14.44,
  vcpkg for plugin dependencies).
- A self-contained Windows build artifact (`build/qCC/deployqt/CloudCompare.exe`
  with all Qt 6 runtime DLLs bundled) — no PATH manipulation needed.
- A slim GitHub Actions matrix that mirrors the local plugin set so the
  downloadable artifact always matches the docs.
- This docs site, published to GitHub Pages.

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
