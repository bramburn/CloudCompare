---
title: Build
sidebar_label: Overview
sidebar_position: 1
---

# Build

The fork is **Windows-first** and the local toolchain is the primary
build environment. Linux is a slim smoke test in CI. macOS community
builds are supported but not CI-validated (see the deprecation note in
[CI overview](/docs/ci/overview#macos-support-dropped-2026-08-24)).

The wrapper scripts in `C:\dev\tools\` pin the Windows toolchain to
known-good versions; the Linux path follows the upstream `BUILD.md` with
the same plugin set; the macOS path is community-maintained.

## Pages in this section

- [Upstream build guide](/docs/build/upstream) — the upstream `BUILD.md` summary; where to
  go for the canonical cross-platform instructions.
- [Build on Windows](/docs/build/windows) — the local toolchain, wrapper scripts, and
  the gotchas.
- [Build on Linux](/docs/build/linux) — apt packages and a fresh-Docker recipe.
- [Build on macOS](/docs/build/macos) — brew packages and the macOS bundle script. **Community-supported, not CI-validated.**
- [Troubleshooting](/docs/build/troubleshooting) — the four or five errors that always
  show up on a clean checkout.

## What the build produces

| Path | What it is |
|---|---|
| `build/qCC/CloudCompare.exe` | The GUI app, unbundled. Needs Qt 6 DLLs on `PATH`. |
| `build/qCC/deployqt/CloudCompare.exe` | Self-contained ~70&nbsp;MB bundle with all Qt DLLs. |
| `build/ccViewer/ccViewer.exe` | The read-only viewer. |
| `build/plugins/*/Release/*.dll` | One DLL per enabled plugin. |
| `build/CMakeCache.txt` | The configure cache. Reused on warm builds. |

## What goes wrong on a clean checkout

Five errors account for ~95% of the failed clean builds. All of them are
documented in [Troubleshooting](/docs/build/troubleshooting) and in the local narrative
[`BUILD-LOCAL.md`](https://github.com/bramburn/CloudCompare/blob/master/BUILD-LOCAL.md):

1. CMake > 4.3 fails the `hidapi` submodule.
2. `ninja` from `depot_tools` shadows the choco `ninja.exe`.
3. Stale `PLUGIN_*` cache after toggling plugin options.
4. `vcvars64.bat`'s CMake 3.31.6 shadowing our pinned 4.3.0 (still ≤ 4.3, so
   it's fine — but PATH order matters).
5. Missing `deployqt\` bundle on a manual `cmake --install` — `windeployqt`
   must run as a post-build step.
