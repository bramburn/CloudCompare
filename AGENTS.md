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
| **Human-facing docs site** | [`website/`](website/) | Docusaurus site at [`bramburn.github.io/CloudCompare`](https://bramburn.github.io/CloudCompare/), deployed by `.github/workflows/deploy-docs.yml`. Start there for the build cookbook, plugin inventory, and architecture map. |
| **Active feature goals** (in-flight projects with their own context) | [`AGENTS_REGISTRATION.md`](AGENTS_REGISTRATION.md) | Manual dual-screen point-cloud registration (Faro Scene Classic-style). Includes §11 with five copy-pasteable agent prompts (recon → PRD → AGENTS → README → milestones). One of N such docs; add more as goals emerge. |

Each sub-folder has its own short `AGENTS.md` pointing at the right topical file. Read **AGENTS-architecture.md first** if you're new to the codebase; jump to **AGENTS-plugin-dev.md** if you're here to add a feature. The active goals at the top level are themselves a kind of per-feature index — see **AGENTS_REGISTRATION.md** for the manual-registration work.

---

## 0b. Active feature goals

Goal-level docs (`AGENTS_*.md` at the repo root) describe **in-flight projects** that span multiple subsystems and don't fit cleanly into the topical files. They each come with a [`docs/context/`](docs/) folder of layered context docs.

| Goal | Doc | Status | Subsystems touched |
|---|---|---|---|
| **Manual dual-screen point-cloud registration** (Faro Scene Classic-style: two synchronized 3D viewports, manual point-pair picking, live transform preview) | [`AGENTS_REGISTRATION.md`](AGENTS_REGISTRATION.md) + [`docs/context/registration/`](docs/context/registration/) | Goal & design only — no plugin yet. Planned as a Standard plugin at `plugins/core/Standard/qManualRegistration/`. | Plugin system (`CCPluginAPI`, `ccMainAppInterface`), GL viewport (`qCC_glWindow`, `ccGLWindow`), picking (`ccPickingHub`, `ccPickingListener`), registration math (`CCCoreLib::HornRegistrationTools` + Umeyama), transform application (`ccDrawableObject::setGLTransformation` vs `ccPointCloud::applyRigidTransformation`). |

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

Use the wrapper scripts at `C:\dev\tools\` (they call `vcvars64.bat` and pin the right toolchain):

```powershell
# Configure (re-run when changing plugins, with --fresh)
& C:\dev\tools\cc-configure.cmd

# Build (incremental, ~6-15 min cold, <30 s warm)
& C:\dev\tools\cc-build.cmd
```

Or manually from *x64 Native Tools Command Prompt for VS 2022*:

```powershell
cmake -S C:\dev\CloudCompare -B C:\dev\CloudCompare\build -G Ninja --fresh `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64 `
  -DCMAKE_MAKE_PROGRAM=C:/ProgramData/chocolatey/bin/ninja.exe

cmake --build C:\dev\CloudCompare\build --config Release --parallel 16
```

**Run the build:**

```powershell
& 'C:\dev\CloudCompare\build\qCC\deployqt\CloudCompare.exe'
```

The `deployqt\` subfolders are self-contained (~70 MB) — `windeployqt` ran during build and bundled Qt6 runtime + all plugin DLLs alongside the exe. No PATH manipulation needed.

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

## Plugin set in this build (18 enabled)

I/O: `qCoreIO` (OBJ/PLY/BIN) — default.
GL: `qEDL`, `qSSAO`.
Standard: `qAnimation`, `qBroom`, `qCSF`, `qM3C2`, `qPoissonRecon`, `qRANSAC_SD`, `qSRA`, `qHPR`, `qPCV`, `qColorimetricSegmenter`, `qMPlane`, `qVoxFall`, `qCompass`, `qCanupo`, `3DFin`.

**Disabled — need external libs (priority order for surveying):**

| Plugin | Needs | Notes |
|---|---|---|
| `qLASIO` | LASzip | **#1 priority** — LAS/LAZ (LiDAR) |
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

Option flag names are e.g. `PLUGIN_IO_QLAS`, `PLUGIN_STANDARD_QPCL`, `PLUGIN_STANDARD_3DFIN` — first line of each plugin's `CMakeLists.txt`.

## Adding a plugin (vcpkg recipe)

```powershell
git clone https://github.com/microsoft/vcpkg C:\dev\vcpkg
C:\dev\vcpkg\bootstrap-vcpkg.bat
C:\dev\vcpkg\vcpkg integrate install

# Surveying priorities
C:\dev\vcpkg\vcpkg install laszip:x64-windows     # → qLASIO
C:\dev\vcpkg\vcpkg install xerces-c:x64-windows   # → qE57IO
C:\dev\vcpkg\vcpkg install draco:x64-windows      # → qDracoIO
C:\dev\vcpkg\vcpkg install pdal:x64-windows       # → qPDALIO
C:\dev\vcpkg\vcpkg install pcl:x64-windows        # → qPCL (~30-60 min build)

# Always edit C:\dev\tools\cc-configure.cmd and add:
#   -DCMAKE_PREFIX_PATH=C:/dev/vcpkg/installed/x64-windows
#   -DPLUGIN_IO_QLAS=ON  (or whichever plugin)
# then re-run the configure + build.
```

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
4. **Need to change a core lib (rare)?** Read [`AGENTS-architecture.md`](AGENTS-architecture.md) first to understand the layer you're crossing, then read [`AGENTS-libs.md`](AGENTS-libs.md) to see who links against it.

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

GitHub Actions on `bramburn/CloudCompare` (the fork). Three workflows are relevant:

- **`.github/workflows/windows.yml`** — our slim Windows build, mirrors the local plugin set (18 self-contained plugins, no LAS/E57/PCL). Triggers on push to `master`, on PR, and on `workflow_dispatch`. Uploads the `deployqt\` bundle as artifact `cloudcompare-windows-x64`. ~15 min cold, <5 min warm (ccache + build-dir cache).
- **`.github/workflows/build.yml`** — inherited from upstream. Runs the **full** Windows + macOS + Ubuntu matrix with the upstream plugin set (LAS, E57, Photoscan, RDB, qFacets, qHoughNormals, qCloudLayers). Useful as a sanity check before upstreaming a PR.
- **`.github/workflows/deploy-docs.yml`** — builds the Docusaurus site under `website/` and publishes to GitHub Pages on the `gh-pages` branch. Triggers on push to `master` when `website/**` or this workflow file change, plus `workflow_dispatch`. Uses the official `actions/deploy-pages@v4`. The site lands at <https://bramburn.github.io/CloudCompare/> within ~1-2 minutes.

To trigger a build manually: GitHub → Actions → pick the workflow → "Run workflow".
To download the Windows build artifact: GitHub → Actions → "Windows Build" → pick a run → scroll to Artifacts → `cloudcompare-windows-x64`.
To see a docs deploy: GitHub → Actions → "Deploy docs site to GitHub Pages" → pick a run → check the environment link.

**Workflow file edits**: when you change any of these three workflow files, also update this section and the relevant build commands above.

## Security

- No secrets in this repo. The bundled dependencies are all open-source.
- vcpkg download cache: `C:\dev\vcpkg\downloads\` — keep it, avoids re-downloading on clean rebuilds.
- The auto-built bundle (`build\qCC\deployqt\`) is ~70 MB and contains Qt6 + plugins — do not commit `build/` to git.

## Pointers

- [`BUILD.md`](BUILD.md) — upstream generic build instructions (cross-platform; supersedes this file's role for the project itself).
- [`BUILD-LOCAL.md`](BUILD-LOCAL.md) — this machine's full build narrative (issues hit, exact paths, every workaround).
- `cmake/CMakeExternalLibs.cmake` — Qt 6 `find_package()` + component list (any new plugin that needs Qt modules should respect this).
- `cmake/DeployQt.cmake` — windeployqt invocation that produces the `deployqt\` bundle.
- `plugins/core/CMakeLists.txt` — plugin enumeration; add new plugin subdirs here.
