---
title: Windows build
sidebar_label: Windows build
sidebar_position: 3
---

# Windows build

The `.github/workflows/windows.yml` workflow builds the fork's slim
plugin set on Windows and uploads the `deployqt\` bundle as a
downloadable artifact.

## Triggering

The workflow triggers on:

- Push to `master` (any change to the C++ source, the `windows.yml`
  file, or the `AGENTS.md`).
- Pull request targeting `master`.
- Manual `workflow_dispatch` from the GitHub UI.

## What it does

1. Sets up MSVC 2022 via the `ilammy/msvc-dev-cmd@v1` action
   (matches the local toolchain).
2. Sets up Qt 6.8.3 MSVC 2022 64-bit via `jurplel/install-qt-action@v3`.
3. Configures with `cmake -S . -B build -G Ninja` and the same
   `-DPLUGIN_*=OFF` flags the local wrapper script uses (no LAS / E57
   / PCL — keep the slim matrix slim).
4. Builds with `cmake --build build --config Release --parallel 8`.
5. Uploads `build/qCC/deployqt/` as the
   `cloudcompare-windows-x64` artifact.

Cold runs are ~15 minutes; warm runs (ccache hit) are \<5 minutes.

## Downloading the artifact

1. Open the workflow run from **GitHub > Actions > Windows Build**.
2. Scroll to **Artifacts** at the bottom of the run summary.
3. Download `cloudcompare-windows-x64` (a `.zip`).
4. Unzip; the `CloudCompare.exe` inside is the self-contained bundle
   — no install, no PATH manipulation.

The artifact is retained for 90 days. There's no signature, so
treat it as a "daily driver" build, not a release artifact.

## What this build does NOT do

- It does **not** build the LAS / E57 / PCL plugins. For those, run
  the [upstream matrix](/docs/ci/upstream-matrix).
- It does **not** run unit tests. `cc-test-lib` is off by default.
- It does **not** build the docs site. That's a
  [separate workflow](/docs/ci/github-pages).

## Local equivalent

The CI build is intentionally a subset of the local build. The
local toolchain is configured by `C:\dev\tools\cc-configure.cmd`
and `C:\dev\tools\cc-build.cmd`; the GitHub Actions workflow
mirrors the same `cmake` invocation with the same plugin flags. If
the local build succeeds, the CI build should succeed for the same
plugin set.
