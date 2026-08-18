---
title: Quick tour
sidebar_label: Quick tour
sidebar_position: 3
---

# Quick tour

A guided walkthrough of the build, run, and verify cycle. About five minutes
on a warm checkout, ~15 minutes cold.

## 1. Configure

```powershell
& C:\dev\tools\cc-configure.cmd
```

This calls `vcvars64.bat`, then runs:

```powershell
cmake -S C:\dev\CloudCompare -B C:\dev\CloudCompare\build -G Ninja --fresh `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64 `
  -DCMAKE_MAKE_PROGRAM=C:/ProgramData/chocolatey/bin/ninja.exe
```

`--fresh` is important when you change plugin options — otherwise a stale
`PLUGIN_IO_QE57=ON` (etc.) will stick in the cache and the plugin's
`find_package(X)` will run even though you thought it was off.

The configure step takes ~30&nbsp;s warm, ~3&nbsp;min cold.

## 2. Build

```powershell
& C:\dev\tools\cc-build.cmd
```

Internally:

```powershell
cmake --build C:\dev\CloudCompare\build --config Release --parallel 16
```

Cold build is ~6-15 minutes depending on which plugins you enabled. Warm
builds (no source changes) finish in under 30&nbsp;s.

## 3. Run

```powershell
& 'C:\dev\CloudCompare\build\qCC\deployqt\CloudCompare.exe'
```

`deployqt\` is the self-contained bundle — Qt 6 runtime DLLs and all plugin
DLLs are alongside `CloudCompare.exe`. The directory is ~70&nbsp;MB.

On first run, CloudCompare creates a `plugins/` folder next to the exe and
auto-discovers every enabled plugin. The list visible in **About > Plugins**
should match [Plugins / Local set](/docs/plugins/local-set).

## 4. Sanity check

1. **File > Open** and load any `.bin` or `.las` from `doc/samples/` (or
   anything you have). The db-tree on the left should populate.
2. **Edit > Cloud > Create random scalar field** — if the cloud turns blue
   and a scalar field appears in the db-tree, the octree is working.
3. **Plugins > Standard Plugins > qM3C2 > M3C2 distance** — if you have
   two clouds loaded, the dialog should open and the comparison should
   produce a coloured cloud. This exercises `CCCoreLib`, `qCC_db`,
   `qCC_io`, `qCC_glWindow`, and one Standard plugin in one shot.

## 5. (Optional) Run the unit tests

The unit tests are off by default. To turn them on:

```powershell
cmake -S C:\dev\CloudCompare -B C:\dev\CloudCompare\build -G Ninja --fresh `
  -DBUILD_TESTING=ON

cmake --build C:\dev\CloudCompare\build --target cc-test-lib
ctest --test-dir C:\dev\CloudCompare\build --output-on-failure
```

`cc-test-lib` is the only test target shipped; the bulk of the
functionality is verified by running CloudCompare against the sample
data in `doc/samples/`.
