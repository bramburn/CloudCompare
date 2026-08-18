---
title: Troubleshooting
sidebar_label: Troubleshooting
sidebar_position: 5
---

# Troubleshooting

The five errors that always show up on a clean checkout. Each is fixed in
the wrapper scripts at `C:\dev\tools\`; this page explains the *why*.

## 1. "CMake 4.4 or higher is required"

```
CMake Error at libs/CCAppCommon/devices/3dConnexion/extern/hidapi/CMakeLists.txt:1 (cmake_minimum_required):
  Compatibility with CMake < 4.4 will be removed from a future version of upstream CMake.
```

**Cause:** the bundled `hidapi` submodule declares
`cmake_minimum_required(VERSION 3.6.3...4.3 FATAL_ERROR)`. CMake 4.4+ rejects
the project.

**Fix:** use the pinned `C:\dev\tools\cmake-4.3.0\bin\cmake.exe`. If you
upgraded CMake system-wide, uninstall or unshadow it. The wrapper scripts
call the pinned binary directly.

## 2. "unknown error" with no context

**Cause:** `C:\dev\depot_tools\ninja` is a bash shim, not a binary. It's
earlier on `PATH` than `C:\ProgramData\chocolatey\bin\ninja.exe`, and
CMake auto-detected it.

**Fix:** pass `-DCMAKE_MAKE_PROGRAM=C:/ProgramData/chocolatey/bin/ninja.exe`
explicitly. The wrapper script does this.

## 3. Plugin `find_package` failed even though I turned it off

**Cause:** stale `PLUGIN_*` cache. The first configure cached
`PLUGIN_IO_QE57=ON`; the next configure reused the cache and the plugin's
`find_package(XercesC)` ran anyway.

**Fix:** always pass `--fresh` when toggling plugin options:

```powershell
cmake -S . -B build -G Ninja --fresh -DPLUGIN_IO_QE57=OFF
```

## 4. `vcvars64.bat` shadows the pinned CMake

**Cause:** `vcvars64.bat` puts its own CMake 3.31.6 on `PATH` before the
user's pinned 4.3.0.

**Effect:** configure works (3.31.6 is `>= 3.10` and `<= 4.3`), but the
build script must use the pinned 4.3.0 explicitly. The `cc-build.cmd`
script does this with a fully-qualified path.

## 5. "Missing `deployqt\` folder after install"

**Cause:** `cmake --install` only copies the executable and the plugin
DLLs. The `windeployqt` step is a post-build custom target (see
`cmake/DeployQt.cmake`) that runs `windeployqt` against the install
directory.

**Fix:** build with the full target set:

```powershell
cmake --build build --target install
```

Or just use the wrapper script — it builds the install target by default.

## Other symptoms

| Symptom | Likely cause | Fix |
|---|---|---|
| `qmake: could not find a Qt installation` | `CMAKE_PREFIX_PATH` not set | Pass `-DCMAKE_PREFIX_PATH=...` or set it before `cmake` |
| `undeclared identifier 'QChar'` in a third-party plugin | Old plugin (Qt 5 era) | Update the plugin, or disable with `PLUGIN_*=OFF` |
| `cannot find -lOpenGL32` | MSVC environment not active | Run from *x64 Native Tools Command Prompt* or call `vcvars64.bat` first |
| Build dies with `cc1plus.exe: out of memory` | `/MP` too aggressive with low RAM | Drop `--parallel 16` to `--parallel 4` for 8&nbsp;GB machines |
| `obj/libqCC_io.so: undefined reference to 'vtable for ccHObject'` | Old `CCCoreLibExport.h` left over from a previous build | Delete `CCCoreLibExport.h` at the repo root (it's `.gitignore`d) |
