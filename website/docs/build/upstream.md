---
title: Upstream build guide
sidebar_label: Upstream
sidebar_position: 1
---

# Upstream build guide

The fork tracks the upstream project at
[`CloudCompare/CloudCompare`](https://github.com/CloudCompare/CloudCompare).
The canonical, cross-platform build instructions live in the upstream
[`BUILD.md`](https://github.com/CloudCompare/CloudCompare/blob/master/BUILD.md)
file in the repo root.

## Why this fork's build is different

The fork pins a specific toolchain and ships a self-contained Windows bundle.
The differences from a stock upstream build are:

| | Upstream | This fork |
|---|---|---|
| CMake | 3.10+ (latest) | **4.3.0** (pinned, see [Prerequisites](/docs/getting-started/prerequisites)) |
| Qt | 5 or 6 (any LTS) | **6.8.3 MSVC 2022 64-bit** |
| Default plugin set | All enabled | **18 self-contained** (LAS, E57, PCL, FBX, Photoscan, etc. are off by default) |
| Windows bundle | Manual `windeployqt` | **Automatic** in the wrapper script |
| CI | Full matrix | **Slim matrix** (Windows only by default; upstream matrix in `.github/workflows/build.yml`) |

The reason: the fork targets a specific hardware profile (Windows Server
2019, VS 2022, 2&nbsp;GB-class dense scans from a terrestrial laser
scanner) and avoids the upstream's "all plugins on" default because the
external dependencies (PDAL, PCL, FBX SDK, Xerces-C++, OpenCASCADE) need
to be installed by the user.

## Where to find the upstream's full instructions

- [`BUILD.md`](https://github.com/CloudCompare/CloudCompare/blob/master/BUILD.md)
  — cross-platform build (Linux, macOS, Windows, Conda, vcpkg).
- [`CONTRIBUTING.md`](https://github.com/CloudCompare/CloudCompare/blob/master/CONTRIBUTING.md)
  — code style, pull request workflow, the LGPL/GPL header templates.
- [cloudcompare.org](https://cloudcompare.org) — binaries, the Wiki, the
  forum, the changelog.

## When to switch to the upstream build

Switch back to a stock upstream build if you want:

- Every plugin enabled out of the box (you don't have to wrestle vcpkg).
- The latest pre-release features that haven't been merged to the fork yet.
- Compatibility with a colleague's build artifact that uses upstream's
  plugin set.

The fork is **fast-forward-only** with respect to upstream — the workflow
is to `git pull --ff-only origin master` and resolve any conflict in the
`cc-configure.cmd` / `cc-build.cmd` scripts if upstream touched the build
configuration.
