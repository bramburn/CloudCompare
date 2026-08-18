# AGENTS-libs.md — The vendored libraries

The `libs/` folder ships **seven** libraries (plus one vendor sub-folder) that together form the CloudCompare runtime. This file is the index of **what each one owns**, **what it depends on**, and **who is allowed to link against it**. Read this when you're about to add code to `libs/` or when a plugin's `#include` errors out.

For the layered architecture diagram, see [`AGENTS-architecture.md`](AGENTS-architecture.md). For how to use these libs from a plugin, see [`AGENTS-plugin-dev.md`](AGENTS-plugin-dev.md).

---

## 0. Quick index

| Folder | CMake target | Public headers (consumed by `qCC`/plugins) | Allowed downstream links |
|---|---|---|---|
| `libs/qCC_db/extern/CCCoreLib` | `CCCoreLib` | `<PointCloud.h>`, `<KdTree.h>`, `<MeshSamplingTools.h>`, `<ScalarFieldTools.h>`, … | everyone |
| `libs/qCC_db` | `QCC_DB_LIB` | `<ccHObject.h>`, `<ccPointCloud.h>`, `<ccMesh.h>`, `<ccPolyline.h>`, `<ccColorTypes.h>`, `<ccGLUtils.h>`, … | `qCC`, `qCC_io`, `qCC_glWindow`, `CCAppCommon`, **not** plugins (they link `CCCoreLib` directly) |
| `libs/qCC_io` | `QCC_IO_LIB` | `<FileIOFilter.h>`, `<AsciiFilter.h>`, `<PlyFilter.h>`, `<BinFilter.h>`, … | `qCC`, `ccViewer`, plugins (via headers) |
| `libs/qCC_glWindow` | `QCC_GL_LIB` | `<ccGLWindowInterface.h>`, `<ccGLWindow.h>`, `<ccRenderingTools.h>`, … | `qCC`, `ccViewer` |
| `libs/CCFbo` | `CC_FBO_LIB` | `<ccFBO.h>` | `qCC_db`, `CCAppCommon`, plugins |
| `libs/CCPluginAPI` | `CCPluginAPI` | `<ccMainAppInterface.h>`, `<ccOverlayDialog.h>`, `<ccPickingHub.h>`, `<ccPersistentSettings.h>`, `<ccCommandLineInterface.h>`, `<ccColorScaleEditorDlg.h>`, `<ccRenderToFileDlg.h>`, … | `qCC`, `ccViewer`, plugins, `CCAppCommon` |
| `libs/CCPluginStub` | `CCPluginStub` | `<ccPluginInterface.h>`, `<ccStdPluginInterface.h>`, `<ccIOPluginInterface.h>`, `<ccGLPluginInterface.h>`, `<ccDefaultPluginInterface.h>` | plugins, `qCC/pluginManager/`, `CCAppCommon` |
| `libs/CCAppCommon` | `CCAppCommon` | `<ccApplicationBase.h>`, `<ccPluginManager.h>`, `<ccRecentFiles.h>`, `<cc3DMouseManager.h>` | `qCC`, `ccViewer` |
| `libs/CCAppCommon/devices/3dConnexion/extern/hidapi` | (vendored, submodule) | hidapi C API | `CCAppCommon` only |

The **plugin contract** (must always hold) is: every plugin links **`CCCoreLib` + `CCPluginAPI` + `CCPluginStub`** + its own third-party libs, **plus** `qCC_io` headers (for `FileIOFilter`) when it's an I/O plugin. Plugins never link `qCC`, `qCC_db`, `qCC_glWindow`, or `CCAppCommon` directly.

---

## 1. `CCCoreLib` (vendored) — pure algorithms

**Where:** `libs/qCC_db/extern/CCCoreLib/` (it's a git submodule at `libs/qCC_db/extern/CCCoreLib`).
**CMake target:** `CCCoreLib` (SHARED).
**License:** LGPL-2-or-later.
**Dependencies:** none of ours. Only third-party headers it brings itself (Eigen optional, etc.).
**Rule:** *no Qt* and no other CC lib. If your algorithm doesn't need Qt, it could live here — but in practice algorithm code goes into the plugin that needs it.

Key namespaces / classes:
- `CCCoreLib::PointCloud`, `CCCoreLib::GenericIndexedCloudPersist`, `CCCoreLib::ReferenceCloud`.
- `CCCoreLib::Delaunay2dMesh`, `CCCoreLib::MeshSamplingTools`.
- `CCCoreLib::KdTree`, `CCCoreLib::Octree`.
- `CCCoreLib::ScalarFieldTools`, `CCCoreLib::StatisticalTestingTools`, `CCCoreLib::WeibullDistribution`, `CCCoreLib::NormalDistribution`.
- `CCCoreLib::CloudSamplingTools`, `CCCoreLib::Jacobi`, `CCCoreLib::ParallelSort`.
- `CCCoreLib::GenericTriangle`, `CCCoreLib::GenericMesh`.
- `CCCoreLib::DistanceComputationTools` (registration helpers).

Use it from anywhere; it's the base of the dependency graph.

---

## 2. `qCC_db` — the 3D scene-graph

**Where:** `libs/qCC_db/`.
**CMake target:** `QCC_DB_LIB` (SHARED).
**License:** LGPL-2-or-later.
**Links against:** `CCCoreLib`, `CC_FBO_LIB`, `Qt6::Widgets`, `Qt6::OpenGL`.
**Public headers:** `libs/qCC_db/include/`.

The **single most important class** is `ccHObject` (`include/ccHObject.h`) — every entity in the db-tree inherits from it. Companion: `ccObject` (`include/ccObject.h`) for the root object / flags / serialization.

Key headers, grouped by purpose:

| Header | Purpose |
|---|---|
| `ccHObject.h`, `ccObject.h`, `ccSerializableObject.h` | The scene graph root + serialization |
| `ccHObjectCaster.h` | Safe casts (`ccHObject` → concrete subclass via class-id checks) |
| `ccPointCloud.h`, `ccGenericPointCloud.h`, `ccGenericMesh.h`, `ccMesh.h`, `ccSubMesh.h` | Core 3D entities |
| `ccPolyline.h`, `ccCircle.h` | Polylines |
| `ccFacet.h`, `ccPlane.h`, `ccSphere.h`, `ccCylinder.h`, `ccCone.h`, `ccBox.h`, `ccDisc.h`, `ccDish.h`, `ccExtru.h`, `ccQuadric.h`, `ccTorus.h` | Geometric primitives |
| `cc2DLabel.h`, `cc2DViewportLabel.h`, `cc2DViewportObject.h` | 2D annotations |
| `ccImage.h` | Raster images attached to clouds |
| `ccSensor.h`, `ccCameraSensor.h`, `ccGBLSensor.h` | Sensors (projective, ground-based) |
| `ccDrawableObject.h`, `ccGLUtils.h`, `ccColorRampShader.h` | GL drawing + shaders |
| `ccColorScale.h`, `ccColorScalesManager.h`, `ccColorTypes.h` | Color scales |
| `ccBBox.h`, `ccChunk.h` | Geometry utilities |
| `ccProgressDialog.h`, `ccLog.h`, `ccConsole.h` | UI/logging helpers |
| `ccExternalFactory.h`, `ccExternalHObject.h` | **Plugin-defined custom object types** that can ride along in `.bin` files |
| `ccAdvancedTypes.h`, `ccBasicTypes.h`, `ccArray.h`, `ccGenericGLDisplay.h` | Foundational types |

**Vendored under it:** `CCCoreLib` (submodule) and `extern/cgaltest` (CGAL, optional).

---

## 3. `qCC_io` — the I/O layer

**Where:** `libs/qCC_io/`.
**CMake target:** `QCC_IO_LIB` (SHARED).
**License:** LGPL-2-or-later.
**Links against:** `QCC_DB_LIB`, `Qt::Concurrent`, optionally `dxflib` (default ON), `shapelib` (default ON), `GDAL` (default OFF).
**Public headers:** `libs/qCC_io/include/`.

The **single most important class** is `FileIOFilter` (`include/FileIOFilter.h`) — every file format plugin subclasses it.

Format-specific headers (built-in formats — always available):

| Header | Format |
|---|---|
| `BinFilter.h` | CloudCompare's own `.bin` (full fidelity) |
| `AsciiFilter.h` | Generic ASCII cloud/mesh |
| `PlyFilter.h` | Stanford PLY |
| `DxfFilter.h` | AutoCAD DXF (needs `OPTION_USE_DXF_LIB=ON`, default ON) |
| `ShpFilter.h` | ESRI Shapefile (needs `OPTION_USE_SHAPE_LIB=ON`, default ON) |
| `RasterGridFilter.h` | GeoTIFF etc. (needs `OPTION_USE_GDAL=ON`, default OFF) |
| `ImageFileFilter.h` | PNG/JPG/etc. as textures on clouds |
| `DepthMapFileFilter.h` | Depth maps |
| `ccGlobalShiftManager.h`, `ccShiftAndScaleCloudDlg.h` | Big-coordinate handling |

Plus dialogs in `libs/qCC_io/ui/` (`.ui` files for Open/Save dialogs) and helpers like `ccShiftAndScaleCloudDlg`.

**Vendored under it:** `extern/dxflib`, `extern/shapelib`, `extern/gdal` (when enabled).

**Vendored under it:** `extern/dxflib` (default ON), `extern/shapelib` (default ON); GDAL if `OPTION_USE_GDAL=ON`.

**Optional vendored:** `qCC_io/test/` (only when `BUILD_TESTING=ON`).

---

## 4. `qCC_glWindow` — the OpenGL viewport

**Where:** `libs/qCC_glWindow/`.
**CMake target:** `QCC_GL_LIB` (SHARED).
**License:** LGPL-2-or-later.
**Links against:** `QCC_DB_LIB`, `CC_FBO_LIB`, `Qt6::OpenGL`, `Qt6::OpenGLWidgets`.
**Public headers:** `libs/qCC_glWindow/include/`.

Public types you should care about:
- `ccGLWindowInterface` — the public viewport API (rendering, picking, refresh, setPointSize, etc.). Every UI feature calls into this.
- `ccGLWindow` — the implementation (don't reference directly from outside the lib).
- `ccRenderingTools` — helper static functions for 2D HUD drawing.
- `ccShader` — base class for shader programs.

GL plugins return a `ccGlFilter` from `getFilter()`. `ccGlFilter` itself lives in `libs/qCC_glWindow/include/ccGlFilter.h`.

---

## 5. `CCFbo` — FBO helper

**Where:** `libs/CCFbo/`.
**Tiny**: a single header (`ccFBO.h`) + implementation. Used by `qCC_db` and `qCC_glWindow` for offscreen rendering (e.g. picking buffer, GL filter passes).

---

## 6. `CCPluginAPI` — the public plugin surface

**Where:** `libs/CCPluginAPI/`.
**CMake target:** `CCPluginAPI` (SHARED).
**License:** GPL-2-or-later.
**Links against:** `QCC_DB_LIB`, `QCC_GL_LIB`, `Qt6::Widgets`.
**Public headers:** `libs/CCPluginAPI/include/`.

**This is the stable surface.** Plugins `#include` from here freely. **Anything not in `include/` is private** — do not consume from a plugin.

| Header | What you use it for |
|---|---|
| `ccMainAppInterface.h` | The host callback (get main window, console, db-tree, freeze UI, register overlay dialog, add to DB, refresh, …) |
| `ccOverlayDialog.h` | Base class for "MDI overlay" dialogs (clip box, segmentation, transformation, section extraction tools) |
| `ccPickingHub.h`, `ccPickingListener.h` | Interactive point / label / polyline picking |
| `ccPersistentSettings.h` | Plugin-side `QSettings` wrapper with namespaced keys |
| `ccCommandLineInterface.h` | Register command-line verbs (CLI mode) |
| `ccArgumentParser.h` | Typed argv parsing helper |
| `ccInfoDlg.h` | Pre-styled "About this plugin" dialog |
| `ccColorScaleEditorDlg.h`, `ccColorScaleEditorWidget.h`, `ccColorScaleSelector.h` | Re-use CC's color scale UI |
| `ccRenderToFileDlg.h` | Render-to-PNG/EXR dialog |
| `ccQtHelpers.h` | Misc Qt utilities (button creation, font scaling) |

**Source files:** `libs/CCAppCommon/src/ccPluginManager.cpp` lives here too — no, sorry, that's in `CCAppCommon`. `CCPluginAPI/src/` only holds the implementations of the headers above.

---

## 7. `CCPluginStub` — the plugin-side base classes

**Where:** `libs/CCPluginStub/`.
**CMake target:** `CCPluginStub` (STATIC, header-only-ish).
**Public headers:** `libs/CCPluginStub/include/`.

| Header | What it's for |
|---|---|
| `ccPluginInterface.h` (v3.2) | The base class every plugin inherits from (directly or via one of the type-specific ones below) |
| `ccDefaultPluginInterface.h` | Common implementation helpers (resource path, `getName()`/`getDescription()`/`getIcon()` defaults from `info.json`) |
| `ccStdPluginInterface.h` (v1.5) | Standard plugin — `getActions()` + `onNewSelection()` |
| `ccIOPluginInterface.h` (v1.3) | I/O plugin — `getFilters()` |
| `ccGLPluginInterface.h` (v1.4) | GL plugin — `getFilter()` |

Plugins `#include` these and **also** the relevant `ccPluginInterface` interface base. The `Q_DECLARE_INTERFACE` macros at the bottom of each header are versioned — never change a version without bumping the version string in the same macro.

---

## 8. `CCAppCommon` — application scaffolding

**Where:** `libs/CCAppCommon/`.
**CMake target:** `CCAppCommon` (SHARED).
**License:** GPL-2-or-later.
**Links against:** `CCPluginAPI`, `CC_FBO_LIB`.
**Public headers:** `libs/CCAppCommon/include/`.

| File | Role |
|---|---|
| `ccApplicationBase.h` / `.cpp` | The `QApplication` subclass shared by both `qCC` and `ccViewer`. Handles `InitOpenGL()`, plugin paths, translation path, shader path. The macro `ccApp` (in the header) is the easy global handle: `#define ccApp (static_cast<ccApplicationBase*>(QCoreApplication::instance()))`. |
| `ccPluginManager.h` / `.cpp` | Scans plugin paths, loads `.so/.dll/.dylib` via `QPluginLoader`, dispatches by `getType()`. The single source of plugin discovery. |
| `ccRecentFiles.h` / `.cpp` | Recent files menu state. |
| `devices/3dConnexion/` | 3D-mouse integration (only used when `OPTION_SUPPORT_3DMOUSE_WITH_3DxWARE=ON` or `OPTION_SUPPORT_3DMOUSE_WITH_HIDAPI=ON`). |
| `QDarkStyleSheet/` | The bundled dark/light QSS stylesheets. |

`QDarkStyleSheet` is **vendored** (its own submodule). It's not your concern unless you're theming the app.

`devices/3dConnexion/extern/hidapi` is **the submodule that caps CMake at 4.3** — see [`AGENTS.md`](AGENTS.md) gotcha #1. Do not edit it; bumping CMake past 4.3 will break the Windows build.

---

## 9. Where each thing lives — quick lookup

When you're looking for:

| Looking for … | Look in |
|---|---|
| The `ccHObject` root, scene graph, types | `libs/qCC_db/include/ccHObject.h`, `ccObject.h`, `ccBasicTypes.h` |
| How to read/write `.bin` | `libs/qCC_io/src/BinFilter.cpp` |
| How to add a new I/O format | copy `plugins/example/ExampleIOPlugin/` |
| How to add a new menu action | copy `plugins/example/ExamplePlugin/` |
| How to add a new GL shader | copy `plugins/example/ExampleGLPlugin/` |
| What `ccMainAppInterface` exposes | `libs/CCPluginAPI/include/ccMainAppInterface.h` |
| What `ccPluginManager` does | `libs/CCAppCommon/src/ccPluginManager.cpp` |
| What algorithm helpers exist | `libs/qCC_db/extern/CCCoreLib/include/*` |
| Where `CloudCompare.exe` is wired | `qCC/CMakeLists.txt` and `qCC/main.cpp` |
| Where `ccViewer.exe` is wired | `ccViewer/CMakeLists.txt` and `ccViewer/main.cpp` |
| Where dialogs live | `qCC/ui_templates/*.ui` + `qCC/cc*Dlg.cpp` |
| Where the db-tree lives | `qCC/db_tree/` |
| Where the plugin UI lives | `qCC/pluginManager/` |
| Where command-line parsing lives | `qCC/ccCommandLineParser.{h,cpp}` and `qCC/ccCommandLineCommands.{h,cpp}` |
| Where shaders are installed | `<install>/shaders/<SHADER_FOLDER_NAME>/` (e.g. `EDL/`, `SSAO/`) |
| Where 3D-mouse code lives | `libs/CCAppCommon/devices/3dConnexion/` |
| Where the dark stylesheet comes from | `libs/CCAppCommon/QDarkStyleSheet/` |
| Where the FBO helper lives | `libs/CCFbo/include/ccFBO.h` |
| Where the cmake glue lives | `cmake/*.cmake` and `plugins/cmake/*.cmake` |
| Where the upstream build instructions are | `BUILD.md` |
| Where the local Windows build notes are | `BUILD-LOCAL.md` |
