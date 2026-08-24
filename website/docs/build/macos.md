---
title: Build on macOS (community-supported)
sidebar_label: macOS
sidebar_position: 4
---

# Build on macOS (community-supported)

> ⚠️ **The fork dropped macOS CI support on 2026-08-24.** This page is
> retained for contributors who want to build locally on macOS. The
> instructions below were last verified against macOS 13 (Ventura) and
> 14 (Sonoma) on Apple Silicon. They have **not** been re-verified
> since the macOS support was dropped, so expect rough edges.

The bundle sources (`qCC/Mac/`, `ccViewer/Mac/`) and the macOS CI
helpers (`.ci/conda-macos.yml`,
`.ci/verify_macos_bundle_identifiers.py`) are kept in tree precisely
so that this workflow remains viable. Pull requests that fix macOS
builds are welcome; they just won't be exercised in CI.

## Required packages

```bash
# Homebrew
brew install cmake ninja qt@6 eigen boost

# Qt 6 PATH
export CMAKE_PREFIX_PATH="$(brew --prefix qt@6):$CMAKE_PREFIX_PATH"
```

For the full self-contained plugin set the upstream tests:

```bash
brew install xerces-c laszip pcl pdal draco
```

## Configure and build

```bash
git clone --recursive https://github.com/bramburn/CloudCompare.git
cd CloudCompare

cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0

cmake --build build --config Release --parallel $(sysctl -n hw.ncpu)
```

## Run

```bash
open build/qCC/CloudCompare.app
```

The macOS bundle script (referenced from `CMakeLists.txt`) produces a
`.app` with the right `Info.plist` and a `MacOS/CloudCompare` binary.
The Qt 6 frameworks are bundled inside the `.app` via `macdeployqt`,
so the app runs from anywhere.

## Differences from the Windows build

- No `deployqt\` folder — instead, everything is inside
  `CloudCompare.app/`.
- The bundle has an Apple Silicon-only and an Intel-only variant; the
  fork builds for the host arch by default. Use
  `-DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"` for a universal binary
  (much longer build).
- The macOS bundle script writes a `macos_bundle_dependencies.json`
  and `macos_bundle_warnings.json` to the build directory; both are
  `.gitignore`d.

## Reporting issues

If you hit a macOS-only bug, open an issue on
[bramburn/CloudCompare](https://github.com/bramburn/CloudCompare/issues)
with the `macos` label. Be aware that no CI will catch it for you, so
include a `.app` bundle or a stack trace.
