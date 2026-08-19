---
title: Build on Windows
sidebar_label: Windows
sidebar_position: 2
---

# Build on Windows

The local toolchain is pinned to a known-good configuration. The wrapper
scripts at `C:\dev\tools\` are the only thing you should need to touch.

## The wrapper scripts

### `cc-configure.cmd`

```bat
@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
"C:\dev\tools\cmake-4.3.0\bin\cmake.exe" -S C:\dev\CloudCompare -B C:\dev\CloudCompare\build -G Ninja --fresh ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64 ^
  -DCMAKE_TOOLCHAIN_FILE=C:/dev/CloudCompare/vcpkg/scripts/buildsystems/vcpkg.cmake ^
  -DCMAKE_MAKE_PROGRAM=C:/ProgramData/chocolatey/bin/ninja.exe ^
  -DBUILD_TESTING=ON ^
  -DPLUGIN_STANDARD_QCSF=ON ^
  -DPLUGIN_STANDARD_QM3C2=ON
```

The script does four things in order:

1. `vcvars64.bat` puts MSVC, the Windows SDK, and the CMake 3.31.6 shim on
   `PATH`. The 3.31.6 is fine for configure (it satisfies `>= 3.10` and is
   `<= 4.3`), but the build script needs the pinned 4.3.0 because the
   build generates files that 3.31.6 doesn't quite understand.
2. `-DCMAKE_TOOLCHAIN_FILE` integrates vcpkg (at `vcpkg/` inside the repo).
   This is how plugin dependencies (LASzip, Xerces-C++, PCL, etc.) are
   found. The vcpkg tree lives inside the repo at `C:\dev\CloudCompare\vcpkg\`
   and is gitignored.
3. `cmake -S … -B … --fresh` writes a fresh configure cache. **Always
   re-run with `--fresh` when toggling plugin options** — otherwise a stale
   `PLUGIN_IO_QE57=ON` (etc.) sticks and the plugin's `find_package(X)`
   runs even though you think it's off.
4. `CMAKE_MAKE_PROGRAM` is set explicitly to dodge the `depot_tools\ninja`
   bash shim that's earlier on `PATH`.

### `cc-build.cmd`

```bat
@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
"C:\dev\tools\cmake-4.3.0\bin\cmake.exe" --build C:\dev\CloudCompare\build --config Release --parallel 16
```

Adds `--parallel 16` to use all logical cores. Warm builds finish in under
30&nbsp;s; cold builds are 6-15 minutes depending on plugin count.

## Run the result

```powershell
& 'C:\dev\CloudCompare\build\qCC\deployqt\CloudCompare.exe'
```

`deployqt\` is a self-contained bundle — `windeployqt` ran during build
(see `cmake/DeployQt.cmake`) and copied every Qt 6 runtime DLL and plugin
DLL alongside the executable. The directory is ~70&nbsp;MB. Copy it to
another Windows box and it just runs.

## Manual invocation (no wrapper)

If you need to bypass the wrapper — for example, to add a `PLUGIN_*=ON`
flag or change the build type:

```powershell
# From an *x64 Native Tools Command Prompt for VS 2022*
cmake -S C:\dev\CloudCompare -B C:\dev\CloudCompare\build -G Ninja --fresh `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64 `
  -DCMAKE_MAKE_PROGRAM=C:/ProgramData/chocolatey/bin/ninja.exe `
  -DPLUGIN_IO_QLAS=ON

cmake --build C:\dev\CloudCompare\build --config Release --parallel 16
```

The first time you enable a plugin, you'll also need its external
dependency installed (see
[Plugins / Disabled priority](/docs/plugins/disabled-priority) for the vcpkg
recipe).

## Plugin deployment

When you enable a new plugin that has external DLLs (e.g. a vcpkg-installed
SDK), copy the plugin's `.dll` files into `deployqt/plugins/<type>/` so
`windeployqt` picks them up automatically:

```bat
tools\cc-deploy-plugins.cmd
```

This script mirrors every built plugin `.dll` from `build/plugins/` into
`build/qCC/deployqt/plugins/`, and also copies the CC core DLLs
(`CCCoreLib.dll`, `QCC_DB_LIB.dll`, etc.) to the deployqt root so the
bundle is self-contained.

## Gotchas

The five errors that always show up on a clean checkout. Full narratives
in [`BUILD-LOCAL.md`](https://github.com/bramburn/CloudCompare/blob/master/BUILD-LOCAL.md).

1. **CMake must be ≤ 4.3** for the bundled `hidapi` submodule. The wrapper
   script pins 4.3.0.
2. **Pass `CMAKE_MAKE_PROGRAM` explicitly** — see the script.
3. **Always use `--fresh`** when toggling plugin options.
4. **`vcvars64.bat` shadows the pinned CMake** with its own 3.31.6. That's
   fine for configure; the wrapper uses the pinned 4.3.0 for the build.
5. **The first build is slow** because the windeployqt step has to copy
   the entire Qt 6 runtime. Subsequent builds skip it.
