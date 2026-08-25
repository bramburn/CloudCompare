---
title: Windows build
sidebar_label: Windows build
sidebar_position: 3
---

# Windows build

The `.github/workflows/build.yml` `Windows MSVC` job is the sole
CI check on `bramburn/CloudCompare`. It builds the fork on Windows
using the Conda environment, MSVC 2022, and Ninja.

## What it does

1. Sets up MSVC 2022 via the `ilammy/msvc-dev-cmd@v1` action
   (matches the local toolchain).
2. Sets up a Conda environment with the full plugin-set dependencies
   (LAS, E57, Photoscan, RDB, qFacets, qHoughNormals, qCloudLayers,
   plus all the small standard plugins).
3. Configures with `cmake -S . -B build -G Ninja` and the
   `-DPLUGIN_*=ON/OFF` flags listed in the workflow file.
4. Runs `cmake --build build --target check-format` (clang-format
   check) and `cmake --build build --parallel` (the full build).
5. `cmake --install build` into a prefix (no artifact upload).

## Why Windows-only

- The fork is Icelabz surveying on Windows Server 2019 / VS 2022;
  the dev team does not run macOS or Linux.
- Linux GCC 13.3 in CI was catching many fork-internal `.h`/`.cpp`
  mismatches that Windows MSVC tolerates; time spent on them
  locally was high and the fixes don't unlock any shipping
  workflow.

## What this build does NOT do

- It does **not** upload a downloadable artifact. To get a
  `deployqt\` bundle, run the local build (see
  [AGENTS.md — Local setup](/docs/getting-started/overview)).
- It does **not** run unit tests. `cc-test-lib` is off by default.
- It does **not** build the docs site. That's a
  [separate workflow](/docs/ci/github-pages).
- It does **not** run macOS or Linux. The bundle sources and CI
  helpers for those platforms were dropped on 2026-08-24.

## Local equivalent

The CI build is intentionally a subset of the local build. The
local toolchain is configured by `C:\dev\tools\cc-configure.cmd`
and `C:\dev\tools\cc-build.cmd`; the GitHub Actions workflow
mirrors the same `cmake` invocation with the same plugin flags. If
the local build succeeds, the CI build should succeed for the same
plugin set.
