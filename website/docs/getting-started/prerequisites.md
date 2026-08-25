---
title: Prerequisites
sidebar_label: Prerequisites
sidebar_position: 2
---

# Prerequisites

The fork targets the toolchain in the table below. The `cc-configure.cmd` /
`cc-build.cmd` wrapper scripts at `C:\dev\tools\` pin everything; the per-
platform pages in [Build](/docs/build/) cover manual install.

## Toolchain (this Windows machine)

| Tool | Version | Path / source |
|---|---|---|
| MSVC | 14.44.35207 (VS 2022 Community 17.14) | `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\` |
| Windows SDK | 10.0.26100.0 | `C:\Program Files (x86)\Windows Kits\10\` |
| CMake (primary) | **4.3.0** | `C:\dev\tools\cmake-4.3.0\bin\cmake.exe` |
| CMake (vcvars) | 3.31.6 | brought in by `vcvars64.bat` (also OK) |
| Ninja | 1.13.2 | `C:\ProgramData\chocolatey\bin\ninja.exe` |
| Qt | 6.8.3 MSVC 2022 64-bit | `C:\dev\tools\Qt\6.8.3\msvc2022_64\` |
| Git | 2.51.0 | system PATH |
| Python | 3.12.10 | `C:\Python312\` (used for `aqtinstall` and CI helpers) |

## Why the CMake cap

The vendored `hidapi` submodule (in
`libs/CCAppCommon/devices/3dConnexion/extern/hidapi/CMakeLists.txt`) declares:

```cmake
cmake_minimum_required(VERSION 3.6.3...4.3 FATAL_ERROR)
```

CMake 4.4+ will reject the project. Do **not** `choco upgrade cmake` without
re-checking this.

## Why `CMAKE_MAKE_PROGRAM` is explicit

`C:\dev\depot_tools\ninja` on this box is a bash shim, not a binary. It
sits earlier on `PATH` than `C:\ProgramData\chocolatey\bin\ninja.exe` and
CMake will pick it up and fail with "unknown error" if you don't override
the make program. The configure wrapper sets
`-DCMAKE_MAKE_PROGRAM=C:/ProgramData/chocolatey/bin/ninja.exe`.

## Linux (community-supported)

The fork dropped Linux CI support on 2026-08-24. The build
instructions are still documented in
[Build on Linux](/docs/build/linux) for contributors who want to
build locally.

## macOS (community-supported)

The fork dropped macOS CI support on 2026-08-24. The build instructions
are still documented in [Build on macOS](/docs/build/macos) for
contributors who want to build locally.

## What you do NOT need to install

- **vcpkg packages** for the default build — the 18 enabled plugins are
  self-contained. You only need vcpkg when you start enabling
  `PLUGIN_IO_QLAS`, `PLUGIN_IO_QE57`, etc. See
  [Plugins / Disabled priority](/docs/plugins/disabled-priority) for the exact
  package list.
- **A custom Qt build** — the binary `aqtinstall` distribution is enough.
- **Visual Studio Code or another editor** — the build is CMake-driven and
  has no editor lock-in.
