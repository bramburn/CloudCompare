# AGENTS.md — CloudCompare

3D point cloud & mesh processing software (open-source, **GPLv2-or-later** for `qCC`/`ccViewer`, **LGPLv2-or-later** for `CCCoreLib`/`qCC_db`/`qCC_io`/`qCC_gl`). C++17 / Qt 6 / CMake 3.10+ / OpenGL 2.1+.
Upstream: https://github.com/CloudCompare/CloudCompare — read [`BUILD.md`](BUILD.md) for upstream build instructions; this file is the **canonical entry point** for AI coding agents and humans working on this repo.

> **Local context**: this checkout is the user's surveying-company (Icelabz) dev environment. Machine is Windows Server 2019, VS 2022 Community 17.14, MSVC 14.44. See [`BUILD-LOCAL.md`](BUILD-LOCAL.md) for the longer-form narrative (every issue hit, every path, every workaround). The shortest path to "rebuild after a pull" is below.

---

## 0. How to use this file

This is the **root index**. Deep dives live in topic-split siblings so each one stays short and cognitive load is low:

| Topic | File | What it covers |
|---|---|---|
| Layered architecture, plugin model, data model | [`AGENTS-architecture.md`](AGENTS-architecture.md) | Why the repo is split the way it is; how `ccHObject`, `ccMainAppInterface`, `ccPluginManager`, `FileIOFilter` fit together. |
| **Adding/editing/removing features & plugins** (the most common workflow) | [`AGENTS-plugin-dev.md`](AGENTS-plugin-dev.md) | Step-by-step recipes for the three plugin types (Standard / IO / GL), renaming, wiring to the build, removing, CI. |
| The 7 vendored libraries & their ownership boundaries | [`AGENTS-libs.md`](AGENTS-libs.md) | `CCCoreLib`, `qCC_db`, `qCC_io`, `qCC_glWindow`, `CCAppCommon`, `CCPluginAPI`, `CCPluginStub`, `CCFbo` — what each owns, what each exposes, who links to whom. |
| UI patterns (dialogs, MDI, console, db-tree) | [`AGENTS-ui.md`](AGENTS-ui.md) | Qt conventions used in `qCC/`, where `.ui` files live, how the db-tree works, how `ccOverlayDialog` plugs in. |
| Coding standards (naming, headers, formatting, includes, clang-format) | [`AGENTS-coding-standards.md`](AGENTS-coding-standards.md) | The rules from `CONTRIBUTING.md` + the .clang-format rules + the unwritten conventions in the codebase. |
| **Unit testing with Qt Test** | [`BUILD-LOCAL.md` §9](BUILD-LOCAL.md#9-unit-tests-qt-test) | Running tests, PATH setup, known issues (SDK link path, DLL exports, ccPointCloud API gotchas, fixture ordering). |
| **Human-facing docs site** | [`website/`](website/) | Docusaurus site at [`bramburn.github.io/CloudCompare`](https://bramburn.github.io/CloudCompare/), deployed by `.github/workflows/deploy-docs.yml`. Start there for the build cookbook, plugin inventory, and architecture map. |
| **Active feature goals** (in-flight projects with their own context) | [`AGENTS_REGISTRATION.md`](AGENTS_REGISTRATION.md) | Manual dual-screen point-cloud registration (Faro Scene Classic-style). Includes §11 with five copy-pasteable agent prompts (recon → PRD → AGENTS → README → milestones). One of N such docs; add more as goals emerge. |
| **Test coverage plan** | [`test-coverage-action-list.md`](test-coverage-action-list.md) | T1–T4 roadmap: 30% → 90% coverage with Qt Test. T1 (T1-A–H) is the current sprint. |
| **`experimental/` workspace** (Rust migration, Qt prototypes, scenario A/B/C) | [`experimental/AGENTS.md`](experimental/AGENTS.md) | Throwaway build area for LLM-driven experiments: `templates/`, `sessions/`, `scenarios/`, `docs/`. Use for any non-trivial new idea **before** touching `libs/`, `qCC/`, `ccViewer/`, or `plugins/`. Tracks decisions via `experimental/docs/decisions.md`. |

Each sub-folder has its own short `AGENTS.md` pointing at the right topical file. Read **AGENTS-architecture.md first** if you're new to the codebase; jump to **AGENTS-plugin-dev.md** if you're here to add a feature. The active goals at the top level are themselves a kind of per-feature index — see **AGENTS_REGISTRATION.md** for the manual-registration work.

---

## 0b. Active feature goals

Goal-level docs (`AGENTS_*.md` at the repo root) describe **in-flight projects** that span multiple subsystems and don't fit cleanly into the topical files. They each come with a [`docs/context/`](docs/) folder of layered context docs.

| Goal | Doc | Status | Subsystems touched |
|---|---|---|---|
| **Sentry error monitoring** (crash reporting + event capture) | [`qCC/main.cpp`](qCC/main.cpp) + [`qCC/test/TestSentry.cpp`](qCC/test/TestSentry.cpp) | **Done.** Sentry SDK integrated, app starts, TestSentry passes. | Sentry DSN `https://ac001120bfa96ba91d2ed97c62e632ad@o494653.ingest.us.sentry.io/4511938553053184`, release `cloudcompare@2.14.0`. `sentry_init()` at top of `main()` before anything else. `sentry_close()` on `QCoreApplication::aboutToQuit`. CMake POST_BUILD in `qCC/CMakeLists.txt` copies sentry.dll + all 8 CC core DLLs into `deployqt/`. `CC_USE_SENTRY=ON` by default; disable with `-DCC_USE_SENTRY=OFF`. |
| **Manual dual-screen point-cloud registration** (Faro Scene Classic-style: two synchronized 3D viewports, manual point-pair picking, live transform preview) | [`AGENTS_REGISTRATION.md`](AGENTS_REGISTRATION.md) + [`docs/context/registration/`](docs/context/registration/) | Goal & design only — no plugin yet. Planned as a Standard plugin at `plugins/core/Standard/qManualRegistration/`. | Plugin system (`CCPluginAPI`, `ccMainAppInterface`), GL viewport (`qCC_glWindow`, `ccGLWindow`), picking (`ccPickingHub`, `ccPickingListener`), registration math (`CCCoreLib::HornRegistrationTools` + Umeyama), transform application (`ccDrawableObject::setGLTransformation` vs `ccPointCloud::applyRigidTransformation`). |
| **Spherical scan browser** (Google Street View for ReCap RCS/RCP: 360° spherical panorama view from each registered scan station, station-jump navigation, click-to-measure) | [`PRD/scan-view/`](PRD/scan-view/) | PRD complete, implementation not started. Planned as a Standard plugin at `plugins/core/Standard/qScanBrowser/`. Phase 1 = colour panorama skybox + bubble-view drag navigation. Phase 2 = range-image depth + measurements. Phase 3 = correspondence view + annotations. | ReCap SDK (`RCStructuredScan`, `RCSphericalModel`, `getColor`, `getRange`), `ccOverlayDialog`, `ccGLWindowInterface::bubbleViewMode`, `ccGBLSensor` spherical depth map, OpenGL sphere texture rendering. |
| **Rust migration** (port ScalarField / RegistrationTools / DgmOctree to Rust, expose via CXX FFI) | [`PRD/rust/05-roadmap.md`](PRD/rust/05-roadmap.md) + [`cc-rust/docs/PHASES.md`](cc-rust/docs/PHASES.md) + [`experimental/AGENTS.md`](experimental/AGENTS.md) | **Phase 0 done** — `cc-rust/` Cargo workspace created, 28/28 tests pass. **Phase 1 done** (ScalarField, hybrid seq+rayon gives 15x at 1M). **Phase 2 in progress** (ICP, hand-rolled octree provisional winner). **Phase 4 strategy selected** (pure-Rust `las` crate, 7.2M pts/s, drops LASzip dep). Decisions: [`experimental/docs/decisions.md`](experimental/docs/decisions.md) (D1, D4, D5, D6, D7). CXX FFI opt-in via `cargo build --features cxx-ffi` (requires MSVC). | `CCCoreLib::ScalarField` (population variance, IEEE NaN semantics — see `experimental/docs/decisions/2026-08-19-rust-scalarfield-formulas.md`), `CCCoreLib::RegistrationTools` (Horn 1987 SVD), `DgmOctree` (octree subdivision), `kiddo` (KD-tree crate, API v6 differs from earlier), `las` crate (pure-Rust LAS parser), CXX 1.0 (`unsafe extern "C++"` block, `staticlib` + `rlib` crate-type). |

**Convention:** if you start a new goal that touches 3+ subsystems and deserves its own context folder, name the doc `AGENTS_<GOAL>.md` and put layered context under `docs/context/<goal>/`. Add a row to the table above.

---

## 1. Local setup (this Windows machine)

```powershell
# First-time clone (recursive — there are 17 submodules)
cd C:\dev
git clone --recursive https://github.com/CloudCompare/CloudCompare.git

# Update later
cd C:\dev\CloudCompare
git pull --recurse-submodules
```

## Build commands

Use the wrapper scripts at `tools/` inside the repo (they call `vcvars64.bat` and pin the right toolchain):

```powershell
# Configure (re-run when changing plugins, with --fresh)
& 'C:\dev\CloudCompare\tools\cc-configure.cmd'

# Build (incremental, ~6-15 min cold, <30 s warm)
& 'C:\dev\CloudCompare\tools\cc-build.cmd'
```

Or manually from *x64 Native Tools Command Prompt for VS 2022*:

```powershell
cmake -S C:\dev\CloudCompare -B C:\dev\CloudCompare\build -G Ninja --fresh `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64 `
  -DCMAKE_MAKE_PROGRAM=C:/ProgramData/chocolatey/bin/ninja.exe

cmake --build C:\dev\CloudCompare\build --config Release --parallel 16
```

**Run the build — no install needed:**

```powershell
& 'C:\dev\CloudCompare\build\qCC\deployqt\CloudCompare.exe'
```

The `deployqt\` folder is a fully self-contained ~70 MB bundle. After a build, it contains:
- `CloudCompare.exe`
- **CC core DLLs** (CCCoreLib, CCAppCommon, CCPluginAPI, QCC_DB_LIB, QCC_IO_LIB, QCC_GL_LIB, CC_FBO_LIB, hidapi) — copied automatically by CMake POST_BUILD in `qCC/CMakeLists.txt`
- **sentry.dll** (Sentry crash reporter) — also copied by that POST_BUILD
- **Qt runtime DLLs** (Qt6Core, Qt6Gui, Qt6Widgets, etc.) — copied by `windeployqt`
- **Qt plugins** (platforms/, imageformats/, styles/, etc.) — copied by `windeployqt`

No PATH, no registry, no system install. Copy the folder to any other Windows x64 machine and it runs. **Do not run the .exe from `build/qCC/CloudCompare.exe`** — that path has no DLLs alongside it; always run from `build/qCC/deployqt/CloudCompare.exe`.

**Iteration loop:**

```powershell
& 'C:\dev\CloudCompare\tools\cc-configure.cmd'   # only when changing plugins or CMake options
& 'C:\dev\CloudCompare\tools\cc-build.cmd'       # incremental: < 30 s for source-only changes
& 'C:\dev\CloudCompare\build\qCC\deployqt\CloudCompare.exe'
```

- **Rebuild only** (no re-configure) for: source edits, UI changes, plugin logic changes.
- **Re-configure + rebuild** for: toggling plugins, changing CMake flags, switching Qt version.

## Test commands

```powershell
# Unit tests (BUILD_TESTING=ON in cmake, currently OFF by default)
cmake -S . -B build -DBUILD_TESTING=ON -G Ninja --fresh
cmake --build build --target cc-test-lib

# Or just run the ctest harness
ctest --test-dir build --output-on-failure
```

## Toolchain on this machine

| Tool | Version | Path | Notes |
|---|---|---|---|
| MSVC | 14.44.35207 | `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\` | Bundled with VS 2022 Community 17.14. |
| Windows SDK | 10.0.26100.0 | `C:\Program Files (x86)\Windows Kits\10\` | |
| CMake (primary) | 4.3.0 | `C:\dev\tools\cmake-4.3.0\bin\cmake.exe` | **NOT 4.4.2** — the bundled `hidapi` submodule caps at 4.3. |
| CMake (vcvars) | 3.31.6 | `…\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\` | What `vcvars64.bat` brings onto PATH. Also OK. |
| Ninja | 1.13.2 | `C:\ProgramData\chocolatey/bin\ninja.exe` | Called explicitly via `CMAKE_MAKE_PROGRAM`. |
| Qt | 6.8.3 MSVC 2022 64-bit | `C:\dev\tools\Qt\6.8.3\msvc2022_64\` | Installed via `aqtinstall` (no Qt account needed). |
| Git | 2.51.0 | | |
| Python | 3.12.10 | `C:\Python312\` | Has `aqtinstall` 3.3.0. |

## 2. The 60-second project tour

CloudCompare is a 3D point-cloud & triangular-mesh processing tool. It ships as **two executables** plus **~30 loadable plugins**:

- **`qCC/CloudCompare.exe`** — full GUI app (db-tree, 3D view, console, plugin menu, command-line parser).
- **`ccViewer/ccViewer.exe`** — read-only viewer with its own (separate) plugin path.
- **Plugins** — three kinds: **Standard** (UI actions like qM3C2, qCSF, qRANSAC_SD), **I/O** (file format filters like qLASIO, qE57IO, qCoreIO), **GL** (OpenGL post-filters like qEDL, qSSAO).

Everything else is a **library** (`libs/`) or **CMake glue** (`cmake/`). New features almost always land as a **plugin**; the core repos change rarely. See [`AGENTS-plugin-dev.md`](AGENTS-plugin-dev.md) for the recipe.

### Documentation site

The fork publishes a human-facing Docusaurus site to GitHub Pages at
**[bramburn.github.io/CloudCompare](https://bramburn.github.io/CloudCompare/)**.
It is built from the [`website/`](website/) directory by
[`.github/workflows/deploy-docs.yml`](.github/workflows/deploy-docs.yml) on
every push to `master` that touches `website/**` or the workflow file
itself. The site mirrors this fork's configuration: the local toolchain
pins, the 18-plugin local set, the disabled-priority recipe, the slim CI
matrix, and the architecture map.

The **agent-facing** reference (this file + `AGENTS-*.md`) and the
**human-facing** docs site are intentionally different audiences:

- This file and `AGENTS-*.md` are for AI coding agents and humans
  reading source code. They live in the repo and are read in-tree.
- The docs site is for end users (and humans who want the rendered
  version of the agent-facing material). It is built from `website/`
  and lives at a public URL.

If you are **writing code** in this repo, read `AGENTS-architecture.md`
first and `AGENTS-plugin-dev.md` for anything that touches `plugins/`.
If you are **deploying or building**, the docs site's
[Build on Windows](https://bramburn.github.io/CloudCompare/docs/build/windows)
is the most up-to-date reference for the local toolchain.

To preview docs changes locally:

```bash
cd website
npm install
npm start          # http://localhost:3000/CloudCompare/
```

### Project layout

- `libs/` — vendored dependencies (`CCCoreLib`, `qCC_db`, `qCC_io`, `qCC_gl`, `CCAppCommon`, `CCPluginAPI`, `CC_FBO_LIB`) — see [`libs/AGENTS.md`](libs/AGENTS.md) and [`AGENTS-libs.md`](AGENTS-libs.md)
- `qCC/` — main `CloudCompare.exe` app + UI — see [`qCC/AGENTS.md`](qCC/AGENTS.md) and [`AGENTS-ui.md`](AGENTS-ui.md)
- `ccViewer/` — minimal viewer (`ccViewer.exe`) — see [`ccViewer/AGENTS.md`](ccViewer/AGENTS.md)
- `plugins/` — loadable plugin shared libraries (Standard / I/O / GL) — see [`plugins/AGENTS.md`](plugins/AGENTS.md) and [`AGENTS-plugin-dev.md`](AGENTS-plugin-dev.md)
  - `plugins/core/IO/` — file-format I/O plugins (LAS, E57, OBJ, PLY, …) — see [`plugins/core/AGENTS.md`](plugins/core/AGENTS.md)
  - `plugins/core/Standard/` — analysis tools (qCSF, qM3C2, qPCL, qPoissonRecon, …)
  - `plugins/core/GL/` — OpenGL shaders (qEDL, qSSAO)
  - `plugins/example/` — copy-paste templates for new plugins — see [`plugins/example/AGENTS.md`](plugins/example/AGENTS.md)
- `cmake/` — CMake helpers (Qt detection, compiler options, deployqt logic) — see [`cmake/AGENTS.md`](cmake/AGENTS.md)
- `.ci/` — CI scripts (reference for canonical build invocations)

## Plugin set in this build (16 enabled)

I/O: `qCoreIO` (OBJ/PLY/BIN) — default. **`qLASIO`** (LAS/LAZ via LASzip).
GL: `qEDL`, `qSSAO`.
Standard: `qAnimation`, `qBroom`, `qCSF`, `qM3C2`, `qPoissonRecon`, `qHPR`, `qPCV`, `qColorimetricSegmenter`, `qMPlane`, `qVoxFall`, `qCanupo`, `3DFin`.

**Disabled — need external libs (priority order for surveying):**

| Plugin | Needs | Notes |
|---|---|---|
| `qE57IO` | Xerces-C++ | E57 format |
| `qPDALIO` | PDAL | LAS + DEMs |
| `qPCL` | PCL | **Big one** — ICP, registration, filters, segmentation |
| `qDracoIO` | Draco | Google Draco compressed |
| `qPhotoscanIO` | zlib + quazip | Agisoft Photoscan |
| `qFBXIO` | FBX SDK | Autodesk FBX |
| `qStepCADImport` | OpenCASCADE | STEP CAD |
| `qMeshBoolean` | libigl + Eigen + CGAL | Mesh boolean |
| `qHoughNormals` | Eigen | Hough normals |
| `qFacets` | ShapeLib | Facet extraction |
| `qCork` | Cork + MPIR | Mesh CSG |
| `qMasonry` | (none extra) | qAutoSeg / qManualSeg — verify before re-enabling |
| `q3DMASC` | OpenCV | Multi-scale segmentation |
| `qG3Point` | Open3D | Geological analysis |
| `qJSonRPCPlugin` | Qt6 Network+WebSockets | JSON-RPC server |
| `qLASFWFIO` | LASLib | Old LAS reader (deprecated; prefer qLASIO) |
| `qRDBIO` | — | Riegl RDB |
| `qAdditionalIO` | various | Extra formats |
| `qCSVMatrixIO` | — | CSV matrix |
| `qTreeIso` | Eigen3 (optional) | Works as-is |

**Disabled — Qt 6.8.3 incompatible (requires fix upstream):**

| Plugin | Issue |
|---|---|
| `qCompass` | `ccTrace` class name collides with Qt 6.8.3's internal `ccTrace` struct in `qvectornd.h` — causes cascading header parse failures across all translation units. |
| `qRANSAC_SD` | Same `ccTrace`/`ccMapWindow` namespace pollution. |
| `qSRA` | Same root cause. |
| `qReCapIO` | Missing `info.json` (renamed to `qReCapIO.json`); also needs Autodesk ReCap SDK v26. |

Option flag names are e.g. `PLUGIN_IO_QLAS`, `PLUGIN_STANDARD_QPCL`, `PLUGIN_STANDARD_3DFIN`, `PLUGIN_IO_QRECAP` — first line of each plugin's `CMakeLists.txt`.

## Local toolchain locations

This machine keeps several tools **inside or right next to the repo** rather
than in `Program Files`, so the build is fully self-contained. Always reference
these from the repo, not from a system path:

| Path | What | Notes |
|---|---|---|
| `C:\dev\CloudCompare\` | This repo (cloned fork) | All source, build, plugins, deploy bundle live here. |
| `C:\dev\CloudCompare\vcpkg\` | vcpkg install (cloned from `microsoft/vcpkg`) | **By design** — the vcpkg tree lives inside the repo. `.gitignore` excludes it from git; don't `git add vcpkg/`. The toolchain file is `C:\dev\CloudCompare\vcpkg\scripts\buildsystems\vcpkg.cmake`. Installed ports go under `vcpkg\installed\x64-windows\`. |
| `C:\dev\CloudCompare\tools\` | Build wrapper scripts (`cc-configure.cmd`, `cc-build.cmd`) | These call `vcvars64.bat` and pin the right toolchain. They live in the repo so they track the local plugin set. |
| `C:\ReCapSDK_v26.0.2\` | Autodesk ReCap SDK (RCS/RCP import) | Only needed for `qReCapIO`. Not currently enabled (broken + SDK not installed). |
| `C:\dev\tools\cmake-4.3.0\` | CMake 4.3.0 (pinned) | **NOT 4.4.2** — bundled `hidapi` submodule caps at 4.3. The wrapper scripts invoke `cmake` via PATH. |
| `C:\dev\tools\Qt\6.8.3\msvc2022_64\` | Qt 6.8.3 (installed via aqtinstall) | System-style install but kept under `C:\dev\tools\` for clarity. |

## Adding a plugin (vcpkg)

vcpkg is installed at `C:\dev\CloudCompare\vcpkg` and integrated with MSVC.
The in-repo configure script `C:\dev\CloudCompare\tools\cc-configure.cmd` already includes
`-DCMAKE_TOOLCHAIN_FILE=C:/dev/CloudCompare/vcpkg/scripts/buildsystems/vcpkg.cmake`
and `-DCMAKE_PREFIX_PATH=…;C:/dev/CloudCompare/vcpkg/installed/x64-windows;…`.

To enable a plugin that needs a vcpkg library:

```powershell
# Install the library
C:\dev\CloudCompare\vcpkg\vcpkg.exe install laszip:x64-windows     # → qLASIO (already enabled)
C:\dev\CloudCompare\vcpkg\vcpkg.exe install xerces-c:x64-windows   # → qE57IO
C:\dev\CloudCompare\vcpkg\vcpkg.exe install draco:x64-windows      # → qDracoIO
C:\dev\CloudCompare\vcpkg\vcpkg.exe install pdal:x64-windows       # → qPDALIO
C:\dev\CloudCompare\vcpkg\vcpkg.exe install pcl:x64-windows        # → qPCL (~30-60 min build)

# Edit C:\dev\CloudCompare\tools\cc-configure.cmd and add:
#   -DPLUGIN_IO_QLAS=ON  (or whichever plugin)
# then re-run configure + build.
```

## Adding a plugin with a locally-installed SDK (non-vcpkg)

Some SDKs (Autodesk ReCap SDK, FARO SDK, etc.) cannot be installed via vcpkg
and must be downloaded manually from the vendor under an NDA/subscription.
These are called **locally-installed SDK plugins**. The guardrails are:

1. **SDK binaries are never committed to the repo.** The SDK lives in a local path
   outside the repo (e.g. `C:\ReCapSDK_v26.0.2`). The plugin's `CMakeLists.txt`
   references it via a `RECAP_SDK_ROOT` variable that defaults to the local path.
2. **DLLs are copied at build time.** The plugin's CMake has a `POST_BUILD` custom
   command that copies `.dll` files from the local SDK `Bin/` into the plugin's
   output directory. `windeployqt` then bundles them automatically.
3. **CI does not build these plugins.** They are gated behind an explicit
   `-DPLUGIN_IO_QRECAP=ON` flag. The slim Windows CI workflow does not enable
   them, so the plugin's CMake must not fail if the SDK is absent (use
   `find_library` with a clear `FATAL_ERROR` message, not a hard hard-coded path).

### qReCapIO example (Autodesk ReCap SDK v26 — RCS/RCP import)

> ⚠️ **Currently disabled.** The plugin folder has `qReCapIO.json` but the CMake macro
> expects `info.json` — it was renamed at some point but the CMakeLists was not updated.
> Additionally, the Autodesk ReCap SDK v26 must be installed at `C:\ReCapSDK_v26.0.2` with
> all 13 `.lib` files present. Until both are fixed, the plugin is disabled.

**SDK location:** `C:\ReCapSDK_v26.0.2` (set by `RECAP_SDK_ROOT` in the plugin's CMakeLists)

**Plugin folder:** `plugins/core/IO/qReCapIO/`

**Files created:**

```
qReCapIO/
├── CMakeLists.txt          ← finds 13 *.lib files; POST_BUILD copies *.dll
├── qReCapIO.json           ← plugin metadata
├── include/
│   ├── CMakeLists.txt      ← public includes + RECAP_SDK_ROOT/Include
│   ├── qReCapIO.h          ← plugin class (Q_OBJECT, addFileIORFilter)
│   └── ReCapFilter.h       ← FileIOFilter subclass
└── src/
    ├── CMakeLists.txt      ← sources + target_link_libraries(${RECAP_LIB_LIST})
    ├── qReCapIO.cpp        ← registerCommands() adds ReCapFilter to app
    └── ReCapFilter.cpp     ← RCScan::loadFile → IRCPointIterator → ccPointCloud
```

**Configure:**
```powershell
& 'C:\dev\CloudCompare\tools\cc-configure.cmd'   # already has -DPLUGIN_IO_QRECAP=ON
```

**How the data flows:**
```
RCScan::loadFile()
  → RCScan::createPointIterator(settings)   # RCPointIteratorSettings
    → IRCPointIterator::getPoint().getPosition()   # RCVector3d → CCVector3
    → IRCPointIterator::getPoint().getColor()      # RCVector4ub → RGB888 SF
    → IRCPointIterator::getPoint().getIntensity()   # float → [0,255] SF
    → IRCPointIterator::getPoint().getNormal()      # → ccNormalVectors table
    → iter->close()
  → ccPointCloud added to container
```

**To add another locally-installed SDK plugin:** follow the same pattern —
`RECAP_SDK_ROOT` variable in CMake, `find_library` loop, `POST_BUILD` DLL copy,
`FileIOFilter` subclass, `addFileIORFilter` in `registerCommands()`.

**Known limitation:** the `RCTransform → ccGLMatrix` conversion in the current
`ReCapFilter.cpp` applies only the translation. The 3×3 rotation component needs
verification against a real `.rcs` file before being used on production survey data.

## ⚠️ Critical gotchas (read before re-configuring)

1. **CMake must be ≤ 4.3** for the bundled `hidapi` submodule. We've pinned 4.3.0 at `C:\dev\tools\cmake-4.3.0\bin\`. Don't `choco upgrade cmake` without re-checking this. Upstream `libs/CCAppCommon/devices/3dConnexion/extern/hidapi/CMakeLists.txt` has `cmake_minimum_required(VERSION 3.6.3...4.3 FATAL_ERROR)`.
2. **Pass `CMAKE_MAKE_PROGRAM` explicitly.** `C:\dev\depot_tools\ninja` is a bash script (depot_tools shim) and comes ahead of the choco `ninja.exe` on PATH. Without the explicit path, CMake auto-detects the script and fails with "unknown error".
3. **Always use `cmake --fresh` when toggling plugin options.** Otherwise stale `PLUGIN_IO_QE57=ON` (etc.) sticks in the cache and the plugin's `find_package(X)` runs even though you think it's off.
4. **vcvars64.bat shadows our pinned CMake** by putting its own CMake 3.31.6 on PATH. That's fine for configure (it's still ≥ 3.10 and ≤ 4.3), but the build script's PATH order matters.

## 3. Code style (short form)

The full rules live in [`AGENTS-coding-standards.md`](AGENTS-coding-standards.md) and [`CONTRIBUTING.md`](CONTRIBUTING.md). The minimum any agent must follow:

- **C++17**, **Qt 6.8** conventions (`Q_OBJECT`, `QString::fromStdString`, `tr()` for user-visible strings, etc.).
- **Indentation**: tabs of width 4 (`.editorconfig`); clang-format enforced in CI.
- **Naming**: `ccCamelCase` for classes, `lowerCamelCase` for methods/variables, `m_lowerCamelCase` for members, `s_` prefix for static variables, `CC_` prefix for enumerators, `ALL_CAPS` for constants, `MACRO_` prefix for macros.
- **File naming** matches the class: `ccConsole.h/.cpp` for `class ccConsole`.
- **Headers**: every new source file gets the official LGPL/GPL header (templates in `CONTRIBUTING.md`).
- **Plugin `CMakeLists.txt`** follows a fixed pattern — see `plugins/core/IO/qCoreIO/CMakeLists.txt` (the canonical shape: `option(PLUGIN_* …)`, `if() project(); AddPlugin(NAME ${PROJECT_NAME} TYPE io); add_subdirectory(...); endif()`).
- **Forward declarations** in headers; **include only what you use** in `.cpp` files.
- **Run `cmake --build build --target check-format`** before any commit that touches `.cpp`/`.h`.

### How to add / edit / remove a feature (cross-ref)

Every change should follow the **Feature Workflow** documented in [`AGENTS-plugin-dev.md`](AGENTS-plugin-dev.md):

1. **Is the change a plugin?** Almost always yes — copy `plugins/example/ExamplePlugin` (Standard), `ExampleIOPlugin` (I/O) or `ExampleGLPlugin` (GL) and follow the rename recipe.
2. **Edit an existing plugin?** Edit in place; the build picks it up next configure. No new wiring needed.
3. **Remove a plugin?** Delete its folder, drop its `option()` and `add_subdirectory()` lines from the parent `CMakeLists.txt`, drop the `-DPLUGIN_*=ON` from `cc-configure.cmd`.
4. **Plugin needs a vendor SDK that can't go in vcpkg?** Follow the **locally-installed SDK pattern** (see §"Adding a plugin with a locally-installed SDK"). The SDK stays on the local machine; CMake copies `.dll`s at build time; the plugin's `CMakeLists.txt` fails gracefully with a clear `FATAL_ERROR` if the SDK is absent.
5. **Need to change a core lib (rare)?** Read [`AGENTS-architecture.md`](AGENTS-architecture.md) first to understand the layer you're crossing, then read [`AGENTS-libs.md`](AGENTS-libs.md) to see who links against it.

The short rule: **don't modify `qCC/`, `ccViewer/`, `libs/qCC_db`, `libs/qCC_io`, `libs/qCC_glWindow` for a new feature**. Put it in `plugins/`.

## PR & commit conventions

### Git remotes — IMPORTANT

- `origin` = upstream `https://github.com/CloudCompare/CloudCompare.git` — read-only reference.
- `bramburn` = our fork `https://github.com/bramburn/CloudCompare.git` — **default for push**, owns the CI.
- `git config remote.pushDefault bramburn` is set in this checkout, so `git push` (no args) lands on the fork. `git pull` (no args) still pulls from `origin/master` for upstream syncs.
- **Workflow for fork-internal work**: commit on local `master`, `git push`. The fork's `master` is fast-forwarded from `origin/master` on every sync, so direct commits on local `master` are safe and routinely used (Doxygen pass, plugin dev, docs site updates).
- **Workflow for upstreaming**: branch from local `master` (`git checkout -b fix/foo origin/master`), commit, `git push bramburn <branch>`, open a PR on GitHub from `bramburn/<branch>` → `CloudCompare/CloudCompare/<branch>`.
- To sync local with upstream: `git pull --ff-only origin master` (the fork is fast-forwarded from origin in the same step).
- To push to the fork: `git push` (defaults to `bramburn` via `remote.pushDefault`) or `git push bramburn master` explicitly.
- **Never push to `origin`** — we don't have upstream write access. If you see `Permission to CloudCompare/CloudCompare.git denied`, it means `remote.pushDefault` got reset; fix it with `git config remote.pushDefault bramburn` and re-push.
- The fork's `master` is fast-forwarded from `origin/master` on every sync — feature work for upstreaming lives on its own branches, not on `master`.

### Commit & PR style

- For **fork-internal work** (Doxygen, plugin dev, docs site, build scripts): commit on local `master`, `git push`. The fork's `master` is fast-forwarded from `origin/master` on every sync, and `remote.pushDefault = bramburn` means `git push` lands on the fork. Local `master` is the right place for these.
- For **upstreaming** (changes that should go into the upstream `CloudCompare/CloudCompare` repo): branch from local `master` (`git checkout -b fix/foo origin/master`), commit on the feature branch, `git push bramburn <branch>`, open a PR on GitHub from `bramburn/<branch>` → `CloudCompare/CloudCompare/<branch>`. **Never** push to `origin/master` directly — we don't have upstream write access.
- The upstream project uses squash-merge with conventional-style commit messages (`feat:`, `fix:`, `docs:`, `refactor:`, `chore:`). The fork follows the same convention.
- Submodules should remain at the SHAs that upstream tracks; if you need a newer submodule, PR upstream first.
- `.pi/` is a local pi-coding-agent scratch dir — gitignored, don't commit it.

## CI

GitHub Actions on `bramburn/CloudCompare` (the fork). Two workflows remain:

- **`.github/workflows/build.yml`** — inherited from upstream. Runs the **full** Windows + macOS + Ubuntu matrix with the upstream plugin set (LAS, E57, Photoscan, RDB, qFacets, qHoughNormals, qCloudLayers). Useful as a sanity check before upstreaming a PR.
- **`.github/workflows/deploy-docs.yml`** — builds the Docusaurus site under `website/` and publishes to GitHub Pages on the `gh-pages` branch. Triggers on push to `master` when `website/**` or this workflow file change, plus `workflow_dispatch`. Uses the official `actions/deploy-pages@v4`. The site lands at <https://bramburn.github.io/CloudCompare/> within ~1-2 minutes.

### Windows build CI: removed (best-effort, lean on local)

The fork's slim Windows CI workflow (`.github/workflows/windows.yml`) was removed on 2026-08-19 after a structural incompatibility with the GitHub-hosted runner surfaced. Three independent fix attempts (YAML escaping, line-ending normalization, header truncation) all hit the same `ninja: error: CMakeFiles\rules.ninja:31: expected newline, got lexing error` — the runner's pre-installed **cmake 4.4.2** emits unparseable `rules.ninja` for this codebase. Our local **cmake 4.3.0** (pinned at `C:\dev\tools\cmake-4.3.0\`) does not have the problem. Fixing CI would require pinning cmake ≤ 4.3 via the `lukka/get-cmake` action, switching to the Visual Studio generator, or self-hosted runners — none worth the maintenance cost for a fork that already builds cleanly via the local `cc-build.cmd`.

**For verification, use the local build:**

```powershell
& 'C:\dev\CloudCompare\tools\cc-configure.cmd'
& 'C:\dev\CloudCompare\tools\cc-build.cmd'
```

The local build is faster (cached, no checkout), uses the same Qt/CMake/Ninja versions, and produces the same `deployqt\` bundle. Upstream CI (`build.yml`) still runs the full matrix on every push to `master` — that's the cross-platform safety net.

If you want to re-enable Windows CI later, the recommended approach is the **Visual Studio generator** (sidesteps the rules.ninja emission bug entirely) or pinning cmake 4.3 explicitly with `lukka/get-cmake@latest` + `version: '4.3.0'`. See [`BUILD-LOCAL.md`](BUILD-LOCAL.md) for the full failure timeline.

To trigger a build manually: GitHub → Actions → pick the workflow → "Run workflow".
To see a docs deploy: GitHub → Actions → "Deploy docs site to GitHub Pages" → pick a run → check the environment link.

## Security

- No secrets in this repo. The bundled dependencies are all open-source.
- vcpkg download cache: `C:\dev\vcpkg\downloads\` — keep it, avoids re-downloading on clean rebuilds.
- The auto-built bundle (`build\qCC\deployqt\`) is ~70 MB and contains Qt6 + plugins — do not commit `build/` to git.

## Pointers

- [`BUILD.md`](BUILD.md) — upstream generic build instructions (cross-platform; supersedes this file's role for the project itself).
- [`BUILD-LOCAL.md`](BUILD-LOCAL.md) — this machine's full build narrative (issues hit, exact paths, every workaround).
- [`test-coverage-action-list.md`](test-coverage-action-list.md) — T1–T4 Qt Test roadmap with evidence references and CMake wiring patterns.
- `cmake/CMakeExternalLibs.cmake` — Qt 6 `find_package()` + component list + Windows SDK auto-detect (`CC_WINDOWS_SDK_LIB_DIR`).
- `cmake/DeployQt.cmake` — windeployqt invocation that produces the `deployqt\` bundle. **Note:** DeployQt only copies Qt runtime DLLs. CC core DLLs (CCCoreLib, CCAppCommon, etc.) and sentry.dll are copied separately by CMake POST_BUILD in `qCC/CMakeLists.txt` — this is intentional and no separate deploy script is needed for the main app.
- `qCC/test/CMakeLists.txt` — test binary wiring; if you add a new test target, copy the existing `add_executable` + `target_link_libraries` + `add_test` pattern.
- `plugins/core/CMakeLists.txt` — plugin enumeration; add new plugin subdirs here.

## Qt 6 Migration (Qt 6.8.3)

Four tracking issues on upstream: [#2367](https://github.com/CloudCompare/CloudCompare/issues/2367) (critical API removals), [#2368](https://github.com/CloudCompare/CloudCompare/issues/2368) (deprecated patterns), [#2369](https://github.com/CloudCompare/CloudCompare/issues/2369) (build config), [#2370](https://github.com/CloudCompare/CloudCompare/issues/2370) (signals/slot MOC). Label `qt6:migration` should be applied once org admin creates it.

**FIXED — already migrated (no action needed):**
- CMake build system: `find_package(Qt6)`, `qt6_wrap_ui()`, `Qt6::Widgets`, `Qt6::OpenGLWidgets` — all correct
- `QOpenGLWidget` / `QOpenGLVersionFunctionsFactory` — canonical Qt 6 pattern throughout (except GL plugins — now fixed)
- `QCustomPlot` vendored lib — has dual-mode `Q_ENUM_NS` / `Q_ENUMS` guards ✅
- `QRegExp`, `QSignalMapper`, `Q_FOREACH` in project code — none found
- `QDesktopWidget`, `QWebEngine`, `QProcess::startDetached` — none found

**FIXED — all critical Qt 6 migration fixes applied (2026-08-19):**
- `QKeySequence::Cancel` → `QKeySequence(Qt::Key_Escape)` — `libs/CCPluginAPI/src/ccOverlayDialog.cpp:40`
- `QColorDialog::getColor` 4-arg → dialog-object pattern — 10 occurrences across `CCAppCommon`, `qCC`, `qCloudLayers`
- `QItemSelectionModel::clear()` → `clearSelection()` — `qCC/db_tree/ccDBRoot.cpp:954,1007`
- `qCC/translations/CMakeLists.txt` → derives Qt6 path from `Qt6::lconvert` target (translations now bundling)
- `QHeaderView::setResizeMode()` → `setSectionResizeMode()` — all 5 files already migrated (confirmed clean)
- `QScopedPointer` → `std::unique_ptr` — 11 files across `qCC_io`, `qCC_db`, plugins (all fixed)
- `Q_SIGNALS`/`Q_EMIT`/`Q_SLOTS` → Qt6 style — 6 `.h` + 5 `.cpp` files (all fixed)
- `qRegisterMetaType<T>("T")` string-arg → `qRegisterMetaType<T>()` — `Mouse3DInput.cpp`
- Metatype declarations added: `ccGLMatrixd`, `CCVector3d`, `CCVector3`, `std::unordered_set<int>`
- `qEDL` / `qSSAO` GL plugins → `QOpenGLVersionFunctionsFactory::get<>()` pattern (all 4 files fixed)
- Dead `QGLFormat` in `#if 0` block removed — `ccGLWindowInterface.cpp`
- `qt5.natvis` → `qt6.natvis` + CMakeLists.txt refs updated
- `BUILD.md` Qt 5 example paths → Qt 6 paths updated

**Remaining warnings (compile but should be cleaned up):**
- `ccGLWindowStereo` uses legacy `QWindow`+manual-context pattern — should mirror `ccGLWindow`'s `QOpenGLWidget` inheritance
- `QOffscreenSurface` construction: format set after construction — defensively fragile in Qt 6
- GL plugins (`qEDL`, `qSSAO`): `initializeOpenGLFunctions()` on stored member → `QOpenGLVersionFunctionsFactory::get<>()`
- `ccGLWindowStereo`: legacy `QWindow`+manual-context pattern → should mirror `ccGLWindow`'s `QOpenGLWidget` inheritance
- `QScopedPointer` → `std::unique_ptr` — ~11 files in `qCC_io`, `qCC_db`, plugins
- `Q_SIGNALS`/`Q_EMIT`/`Q_SLOTS` macros → Qt 6 style (`signals:`/`emit`/`slots:`)
- Metatype registration gaps: `ccGLMatrixd`, `CCVector3d`, `std::vector<float>`, `std::unordered_set<int>` in queued signals

**Cosmetic:** `qt5.natvis` filename → rename to `qt6.natvis`; `BUILD.md` has Qt 5.15.2 example paths that should be Qt 6 paths.
