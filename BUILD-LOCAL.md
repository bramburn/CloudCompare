# CloudCompare — Local Build Notes (this machine only)

Local development environment for [CloudCompare](https://github.com/CloudCompare/CloudCompare) (master branch, 2026-08-18).

This file is local-only — not part of the upstream repo. (The repo's own `BUILD.md` is upstream documentation; this file is `BUILD-LOCAL.md` so they don't collide.)

---

## 1. Source

| | |
|---|---|
| Location | `C:\dev\CloudCompare\` |
| Branch | `master` |
| HEAD | `5a63db7d` |
| Commit | "build: fix RC compilation (cmcldeps.exe rc.exe flag issue) + clean SDK path propagation" — 2026-08-19 |
| Synced | Fork's `master` is fast-forwarded from `origin/master`; feature branches used for upstream PRs. |
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

Build scripts live at `C:\dev\CloudCompare\tools\`:

| Script | What it does |
|---|---|
| `tools\cc-configure.cmd` | From a VS 2022 x64 native prompt: calls `cmake -G Ninja …` with the plugin set below. |
| `tools\cc-build.py` | Runs Ninja directly (`ninja -C build -j16`). Handles `--clean` to wipe CMakeFiles first. |

**Recommended workflow (from PowerShell):**
```powershell
# Configure (re-run when changing plugins)
python C:\dev\CloudCompare\tools\cc-configure.py

# Build (incremental — ~30 s for source-only changes)
python C:\dev\CloudCompare\tools\cc-build.py

# Clean + rebuild (forces full recompile)
python C:\dev\CloudCompare\tools\cc-build.py --clean
```

**Or from an x64 Native Tools Command Prompt for VS 2022:**
```powershell
cd C:\dev\CloudCompare
tools\cc-configure.cmd
tools\cc-build.cmd
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

Then re-run the configure:
```powershell
python C:\dev\CloudCompare\tools\cc-configure.py
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

### #6 — `fatal error RC1106: invalid option: -3` (CMake 3.31 + Ninja + MSVC)

CMake 3.31's Ninja generator passes `-D` and `-I` flags (Unix style) to ALL compilers, including `rc.exe`. The Windows resource compiler only understands `/D` and `/I` (Windows style). When `rc.exe` sees `-D SYM`, it interprets `D SYM` as a single argument and then sees `-3` (tail of `/EHsc` from `CMAKE_CXX_FLAGS`) as an invalid option.

**Fix (in `cmake/CMakeExternalLibs.cmake`):** `CMAKE_RC_COMPILER_LAUNCHER` is set to a Python wrapper (`rc_wrapper.py`) that converts `-D` → `/D` and `-I` → `/I` before forwarding to `rc.exe`. This is generated at configure time into the build directory.

Additionally, `/EHsc` is stripped from `CMAKE_RC_FLAGS` via `string(REGEX REPLACE)` so it never reaches `rc.exe` via that route.

---

## 9. Unit tests (Qt Test)

Qt Test (`Qt6::Test`) is used for unit tests. `BUILD_TESTING=ON` is set in `tools/cc-configure.cmd`.

### Test binaries (T1 — all passing as of 2026-08-19)

| Binary | Tests | Status |
|---|---|---|
| `TestArgumentParser.exe` | 60 | ✅ PASS |
| `TestFileIOFilter.exe` | 24 | ✅ PASS |
| `TestPointCloud.exe` | 23 | ✅ PASS |
| `TestShiftedObject.exe` | 20 | ✅ PASS |
| `TestRegistration.exe` | 31 | ✅ PASS |
| **Total** | **158** | **0 failed** |

```
C:\dev\CloudCompare\build\qCC\test\TestArgumentParser.exe
C:\dev\CloudCompare\build\qCC\test\TestFileIOFilter.exe
C:\dev\CloudCompare\build\qCC\test\TestPointCloud.exe
C:\dev\CloudCompare\build\qCC\test\TestShiftedObject.exe
C:\dev\CloudCompare\build\qCC\test\TestRegistration.exe
```

### Running tests

**Never run Qt test executables directly from PowerShell without a DLL PATH.** The DLLs are spread across `build/libs/` subdirectories — the system `PATH` does not include them. Use Python `subprocess` to set `PATH` explicitly:

```python
import subprocess, os

dll_dirs = [
    'C:/dev/CloudCompare/build/libs/CCPluginAPI',
    'C:/dev/CloudCompare/build/libs/qCC_db',
    'C:/dev/CloudCompare/build/libs/qCC_db/extern/CCCoreLib',
    'C:/dev/CloudCompare/build/libs/CCFbo',
    'C:/dev/CloudCompare/build/libs/qCC_glWindow',
    'C:/dev/CloudCompare/build/libs/qCC_io',
    'C:/dev/CloudCompare/build/libs/qCC_io/extern/shapelib',
    'C:/dev/tools/Qt/6.8.3/msvc2022_64/bin',
]
env = os.environ.copy()
env['PATH'] = ';'.join(dll_dirs) + ';' + env.get('PATH', '')
result = subprocess.run(
    ['C:/dev/CloudCompare/build/qCC/test/TestRegistration.exe',
     '-txt', '-o', 'C:/dev/CloudCompare/test_registration_result.txt'],
    capture_output=True, text=True, env=env, timeout=120
)
with open('C:/dev/CloudCompare/test_registration_result.txt') as f:
    print(f.read())
```

The `TestFileIOFilter.exe` also needs `qCC_io` and `shapelib` DLL dirs.

### Issues hit and fixed (do not repeat)

**`d3d11.lib not found` on link** — `cmake/CMakeExternalLibs.cmake` hardcoded Windows Kits **8.0** (doesn't exist on this machine) and added it to `CMAKE_PREFIX_PATH` (not used by the MSVC linker). Fixed: replaced with `CC_WINDOWS_SDK_LIB_DIR` that auto-detects the newest installed SDK (10.0.26100.0) and adds `/LIBPATH:...` to all test targets via `add_link_options()` in `qCC/test/CMakeLists.txt`. **If you add new test targets, add the same `add_link_options` guard.**

**`FileIOFilter::GetRealFilename` unresolved external** — the method had no `QCC_IO_LIB_API` export macro. `libs/qCC_io/include/FileIOFilter.h` now includes `qCC_io.h` and declares `QCC_IO_LIB_API static QString GetRealFilename(...)`. **Any static method in a shared library class that is defined in the `.cpp` and called from outside the library needs the `*_LIB_API` macro.**

**`ccPointCloud::addColor()` crashes with access violation** — `addColor()` calls `assert(m_rgbaColors->isAllocated())`. The color table is a **separate allocation** from the point table. Must call `reserveTheRGBTable()` before `addColor()`. Same pattern for normals: call `reserveTheNormsTable()` before `addNorm()`.

**Qt Test `cleanup()` slot ordering** — `cleanup()` runs **after** every test function. If `cleanup()` calls `InitInternalFilters()`, any test that calls `UnregisterAll()` without re-initialising first will see filters restored before it can assert on the empty state. Structure tests accordingly.

**Wrong filter string assertions** — `FileIOFilter::ImportFilterList()` uses `QObject::tr("All (*.*)")` (returns the translation string), not `"All Files (*.*)"`. The PLY filter registers as `"PLY mesh (*.ply)"`, not `"Stanford ply (*.ply)"`. **Always grep the actual source to get the exact string before writing assertions.**

**MSVC template `>>` parsing in Qt Test macros** — `QTest::addColumn<std::vector<std::vector<double>>>("name")` causes `C2947` ("expecting '>'") in MSVC because the preprocessor misparses `>>` inside angle brackets. Fix: use `Mat3 = std::array<double, 9>` (flat array) instead, or use a typedef.

**`M_SQRT2_2` undefined on MSVC** — `<cmath>` doesn't define `M_SQRT2_2` on MSVC. Define manually: `#define M_SQRT2_2 0.707106781186547524400844362104849`.

**`loadIdentity()` → `toIdentity()`** — `SquareMatrixd::loadIdentity()` was renamed to `toIdentity()` in recent CloudCompare.

**`SquareMatrixd::inverse()` → `inv()`** — `inverse()` was renamed to `inv()`.

**`MakeIdentity3()` helper** — `SquareMatrixd::Identity(3)` static factory doesn't exist. Use: `SquareMatrixd I(3); I.toIdentity();`

**Horn `FindAbsoluteOrientation` returns `T = centroid_P - centroid_Q`** — The translation returned maps REFERENCE → MOVING, not the reverse. For a cloud Q = P + offset, Horn returns T = -(offset). This is consistent but non-obvious.

**Horn scale formula** — `trans.s = RMS(aligned_cloud) / sqrt(RMS(ref_cloud))`, not `s_new / s_ref`. For a 2× scaled cloud, this gives ~0.5, not 2.0.

### Writing new tests

- All test files live in `qCC/test/`
- Each new binary needs an entry in `qCC/test/CMakeLists.txt`
- Use `QTEST_GUILESS_MAIN(ClassName)` for headless tests
- `CCVector3` / `CCVector3d` have no `operator==` — compare `.x/.y/.z` individually
- Scalar field: `addScalarField()` returns an index, `sf->setValue(i, val)`, `sf->computeMinAndMax()`, then `sf->getMin()`/`getMax()`

---

## 8. Files & logs created

| Path | What |
|---|---|
| `C:\dev\tools\cmake-4.3.0\` | Portable CMake 4.3.0 (extracted from GitHub release zip) |
| `C:\dev\tools\Qt\6.8.3\msvc2022_64\` | Qt 6.8.3 MSVC 2022 64-bit |
| `C:\dev\CloudCompare\tools\cc-configure.cmd` | Configure batch wrapper (from VS x64 prompt) |
| `C:\dev\CloudCompare\tools\cc-build.py` | Python build script (Ninja wrapper, handles `--clean`) |
| `C:\dev\CloudCompare\BUILD-LOCAL.md` | This file (local-only, untracked by git) |
| `C:\dev\CloudCompare\build\` | All build outputs; `qCC/deployqt/` = runnable CloudCompare bundle (~70 MB) |
