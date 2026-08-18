# CloudCompare — Local Build Notes (this machine only)

Local development environment for [CloudCompare](https://github.com/CloudCompare/CloudCompare) (master branch, 2026-08-18).

This file is local-only — not part of the upstream repo. (The repo's own `BUILD.md` is upstream documentation; this file is `BUILD-LOCAL.md` so they don't collide.)

---

## 1. Source

| | |
|---|---|
| Location | `C:\dev\CloudCompare\` |
| Branch | `master` |
| HEAD | `da62b8e0155cee4237335476477cb1088c54c2f3` |
| Commit | "Make the ASCII open dialog strings translatable (#2365)" — 2026-08-19 01:50:37 +0900 |
| Synced | `git fetch` confirms 0 commits behind `origin/master` (fresh `--recursive` clone) |
| Submodules | All 17 submodules checked out |

To re-sync:
```powershell
cd C:\dev\CloudCompare
git pull --recurse-submodules
```

---

## 2. Toolchain installed (this machine)

| Tool | Version | Path | Why this version |
|---|---|---|---|
| MSVC (cl.exe) | 14.44.35207 | `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\HostX64\x64\cl.exe` | Bundled with VS 2022 Community 17.14. CloudCompare's primary Windows toolchain. |
| Windows 10 SDK | 10.0.26100.0 | `C:\Program Files (x86)\Windows Kits\10\` | Current. |
| CMake (primary) | **4.3.0** | `C:\dev\tools\cmake-4.3.0\bin\cmake.exe` | Downgraded from 4.4.2 — see §7 issue #1. |
| CMake (also installed) | 4.4.2 | `C:\Program Files\CMake\bin\cmake.exe` | From chocolatey. Not used. |
| Ninja | 1.13.2 | `C:\ProgramData\chocolatey\bin\ninja.exe` | Called explicitly via `CMAKE_MAKE_PROGRAM` — see §7 issue #2. |
| Qt | **6.8.3 MSVC 2022 64-bit** | `C:\dev\tools\Qt\6.8.3\msvc2022_64\` | Installed via `aqtinstall` (no Qt account needed for open-source LTS). |
| Git | 2.51.0 | — | For clone / submodule updates. |
| Python | 3.12.10 | `C:\Python312\` | Carries `aqtinstall` 3.3.0. |

PATH adjustments: `C:\dev\tools\cmake-4.3.0\bin` was prepended to user PATH so `cmake` resolves to 4.3.0 (ahead of the choco 4.4.2 install).

> **Note:** The `vcvars64.bat` brings its own `cmake 3.31.6` onto PATH first, so the *configure* step actually runs 3.31.6, not 4.3.0. That's fine — 3.31.6 is above CloudCompare's `cmake_minimum_required(3.10)` and below the hidapi cap of 4.3. We pin 4.3.0 for direct (no-vcvars) invocations only.

---

## 3. Build outputs (self-contained)

The build runs `windeployqt` automatically, which copies the Qt runtime DLLs and platform plugins alongside the executables. The `deployqt\` subfolders are the **ready-to-run bundles** — no PATH manipulation needed.

```
C:\dev\CloudCompare\build\
├── qCC\
│   ├── CloudCompare.exe          ← raw build (3.5 MB; needs Qt on PATH)
│   └── deployqt\                 ← ✅ self-contained bundle (~70 MB)
│       ├── CloudCompare.exe      ← RUN THIS
│       ├── Qt6Core.dll, Qt6Gui.dll, Qt6Widgets.dll, Qt6OpenGL*.dll, Qt6Svg.dll, …
│       ├── platforms\qwindows.dll
│       ├── imageformats\q*.dll
│       ├── styles\qmodernwindowsstyle.dll
│       ├── CCAppCommon.dll, CCCoreLib.dll, QCC_DB_LIB.dll, QCC_IO_LIB.dll, QCC_GL_LIB.dll
│       ├── hidapi.dll
│       ├── QCORE_IO_PLUGIN.dll
│       ├── QEDL_GL_PLUGIN.dll, QSSAO_GL_PLUGIN.dll
│       └── 3DFIN_/QANIMATION_/QBROOM_/QCANUPO_/QCOLORIMETRIC_/QCOMPASS_/QCSF_/QHPR_/QM3C2_/QMPLANE_/QPCV_/QPOISSON_RECON_/QRANSAC_SD_/QSRA_/QVOXFALL_PLUGIN.dll
└── ccViewer\
    ├── ccViewer.exe
    └── deployqt\ccViewer.exe     ← ✅ self-contained
```

Run the bundle:
```powershell
& 'C:\dev\CloudCompare\build\qCC\deployqt\CloudCompare.exe'
```

If you'd rather run the raw build (not the bundle), point PATH at Qt's bin first:
```powershell
$env:Path = 'C:\dev\tools\Qt\6.8.3\msvc2022_64\bin;' + $env:Path
& 'C:\dev\CloudCompare\build\qCC\CloudCompare.exe'
```

---

## 4. How to re-configure and rebuild

Two wrapper scripts live at `C:\dev\tools\`:

| Script | What it does |
|---|---|
| `C:\dev\tools\cc-configure.cmd` | Calls `vcvars64.bat`, then `cmake -G Ninja --fresh …` with the plugin set below. |
| `C:\dev\tools\cc-build.cmd` | Calls `vcvars64.bat`, then `cmake --build C:\dev\CloudCompare\build --config Release --parallel 16`. |

To re-configure (e.g. after adding/removing plugins):
```powershell
& C:\dev\tools\cc-configure.cmd
```

To rebuild (incremental):
```powershell
& C:\dev\tools\cc-build.cmd
```

Or manually from an *x64 Native Tools Command Prompt for VS 2022*:
```powershell
cmake -S C:\dev\CloudCompare -B C:\dev\CloudCompare\build -G Ninja --fresh `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64 `
  -DCMAKE_MAKE_PROGRAM=C:/ProgramData/chocolatey/bin/ninja.exe
cmake --build C:\dev\CloudCompare\build --config Release --parallel 16
```

---

## 5. Plugin set

**Enabled (18 total):**

| Type | Plugin | Use |
|---|---|---|
| IO | `qCoreIO` | OBJ / PLY / BIN (always on by default) |
| GL | `qEDL` | Eye-Dome Lighting shader |
| GL | `qSSAO` | Screen-Space Ambient Occlusion shader |
| Standard | `qAnimation` | Camera animation exporter |
| Standard | `qBroom` | Point cloud cleaning tool |
| Standard | `qCSF` | Cloth Simulation Filter (ground filtering — surveying) |
| Standard | `qM3C2` | Multiscale Model-to-Model Cloud Comparison — surveying |
| Standard | `qPoissonRecon` | Poisson surface reconstruction |
| Standard | `qRANSAC_SD` | RANSAC shape detection (planes, spheres, cylinders) |
| Standard | `qSRA` | Surface of Revolution Analysis |
| Standard | `qHPR` | Hidden Point Removal (visibility) |
| Standard | `qPCV` | Point Cloud Virtual Scanner |
| Standard | `qColorimetricSegmenter` | Colorimetric segmentation |
| Standard | `qMPlane` | MPlane plugin |
| Standard | `qVoxFall` | VoxFall plugin |
| Standard | `qCompass` | Compass strike/dip measurement |
| Standard | `qCanupo` | CANUPO classifier |
| Standard | `3DFin` | 3DFin individual tree detection |

**Disabled — need external libraries (install via vcpkg, see §6):**

| Plugin | Missing dep | Why you want it |
|---|---|---|
| `qDracoIO` | Draco | Read/write Google Draco compressed .drc |
| `qLASIO` | LASzip | **LAS/LAZ (LiDAR) — top priority for surveying** |
| `qLASFWFIO` | LASLib | Old LAS reader (deprecated; prefer qLASIO) |
| `qPhotoscanIO` | zlib + quazip | Agisoft Photoscan project files |
| `qE57IO` | Xerces-C++ | E57 format |
| `qPDALIO` | PDAL | LAS + DEMs via PDAL |
| `qFBXIO` | FBX SDK | Autodesk FBX import |
| `qStepCADImport` | OpenCASCADE | STEP CAD files |
| `qRDBIO` | — | Riegl RDB format |
| `qAdditionalIO` | various | Extra formats |
| `qCSVMatrixIO` | — | CSV matrix I/O |
| `qMeshBoolean` | libigl + Eigen + CGAL | Mesh boolean ops |
| `qHoughNormals` | Eigen | Hough transform normal estimation |
| `qFacets` | ShapeLib | Facet extraction |
| `qCork` | Cork + MPIR | Mesh CSG (alt) |
| `qMasonry` | — | Masonry analysis (qAutoSeg / qManualSeg) |
| `q3DMASC` | OpenCV | Multi-scale shape/appearance segmentation |
| `qG3Point` | Open3D | Geological point analysis |
| `qJSonRPCPlugin` | Qt6 Network+WebSockets | JSON-RPC server |
| `qPCL` | PCL | **The big one — ICP, registration, filters, segmentation** |
| `qTreeIso` | Eigen3 (optional) | Tree isolation — works as-is |

The names of the `option()` flags are e.g. `PLUGIN_IO_QLAS`, `PLUGIN_STANDARD_QPCL`, `PLUGIN_STANDARD_3DFIN` — every plugin's first line in its `CMakeLists.txt`.

---

## 6. Adding more plugins (vcpkg recipe)

vcpkg is the cleanest dependency manager on Windows. Once installed, dependencies land at `C:\dev\vcpkg\installed\x64-windows\` and are found by `find_package()` automatically.

```powershell
git clone https://github.com/microsoft/vcpkg C:\dev\vcpkg
C:\dev\vcpkg\bootstrap-vcpkg.bat
C:\dev\vcpkg\vcpkg integrate install

# Surveying priorities
C:\dev\vcpkg\vcpkg install laszip:x64-windows        # → enables qLASIO (LAS/LAZ)
C:\dev\vcpkg\vcpkg install eigen3:x64-windows        # → enables qHoughNormals, qTreeIso
C:\dev\vcpkg\vcpkg install draco:x64-windows         # → enables qDracoIO
C:\dev\vcpkg\vcpkg install xerces-c:x64-windows      # → enables qE57IO
C:\dev\vcpkg\vcpkg install pdal:x64-windows          # → enables qPDALIO

# The big ones (slow)
C:\dev\vcpkg\vcpkg install pcl:x64-windows           # → enables qPCL (~30-60 min)
C:\dev\vcpkg\vcpkg install libigl cgal:x64-windows   # → enables qMeshBoolean
C:\dev\vcpkg\vcpkg install opencv:x64-windows        # → enables q3DMASC
```

Then re-run the configure with `CMAKE_PREFIX_PATH` updated:
```powershell
& C:\dev\tools\cc-configure.cmd   # edit the script to add -DCMAKE_PREFIX_PATH=C:/dev/vcpkg/installed/x64-windows and the new plugin flag
```

---

## 7. Issues hit during the initial build

### #1 — `hidapi` submodule caps CMake at 4.3

`libs/CCAppCommon/devices/3dConnexion/extern/hidapi/CMakeLists.txt` has:

```cmake
cmake_minimum_required(VERSION 3.6.3...4.3 FATAL_ERROR)
```

With **CMake 4.4.2** (the choco current), this fails with `unknown error` and `CMAKE_C_COMPILER not set` cascades. **Fix:** install CMake 4.3.0 from the GitHub release ZIP into `C:\dev\tools\cmake-4.3.0\` and put it on PATH first.

### #2 — `C:\dev\depot_tools\ninja` breaks CMake's auto-detection

`depot_tools` ships a `ninja` shell script (a Python wrapper) that comes earlier on PATH than the choco `ninja.exe`. When CMake auto-detects the build tool, it picks the script and fails with `unknown error`.

**Fix:** pass `CMAKE_MAKE_PROGRAM=C:/ProgramData/chocolatey/bin/ninja.exe` explicitly to `cmake -G Ninja`.

### #3 — `vcvars64.bat` shadows the pinned CMake

`vcvars64.bat` adds `…\CommonExtensions\Microsoft\CMake\CMake\bin` to PATH, which contains **CMake 3.31.6**, ahead of our `C:\dev\tools\cmake-4.3.0`. So when invoked through `cc-configure.cmd` (which calls vcvars), the configure actually uses 3.31.6. That's still ≥ the 3.10 minimum and ≤ the 4.3 hidapi cap, so it works — but if you call `cmake` directly without vcvars, the 4.3.0 pin takes effect.

### #4 — Stale CMakeCache from a previous configure

If you change a plugin flag and re-run without `--fresh`, the old cached value (`PLUGIN_IO_QE57=ON` from the first failed run, for example) sticks. Always use `cmake --fresh` when toggling plugins, or set every option explicitly.

### #5 — `Remove-Item` blocked by the sandbox

Wiping the build directory with `Remove-Item -Recurse -Force` was blocked by the agent's safety guard. CMake `--fresh` is the workaround — it empties the cache without removing the build tree.

---

## 8. Files & logs created

| Path | What |
|---|---|
| `C:\dev\tools\cmake-4.3.0\` | Portable CMake 4.3.0 (extracted from GitHub release zip) |
| `C:\dev\tools\Qt\6.8.3\msvc2022_64\` | Qt 6.8.3 MSVC 2022 64-bit |
| `C:\dev\tools\cc-configure.cmd` | Configure wrapper |
| `C:\dev\tools\cc-build.cmd` | Build wrapper |
| `C:\dev\tools\cc-configure.log` | Last configure run log |
| `C:\dev\tools\cc-build.log` | Last build run log |
| `C:\dev\CloudCompare\BUILD-LOCAL.md` | This file (local-only, untracked by git) |
| `C:\dev\CloudCompare\build\` | All build outputs (~3-5 GB when complete; the `deployqt\` subfolders are the runnable bundles) |
