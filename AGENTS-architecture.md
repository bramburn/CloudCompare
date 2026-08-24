# AGENTS-architecture.md — Layered architecture & runtime model

The 60-second mental model for **why** this repo is split the way it is. Read this before touching anything in `libs/` or `qCC/`. For the build/install workflow see root [`AGENTS.md`](AGENTS.md). For the feature-add workflow see [`AGENTS-plugin-dev.md`](AGENTS-plugin-dev.md).

---

## 1. The layers (bottom → top)

CloudCompare is organised as a strict dependency stack. Every layer links only **down**, never sideways or up.

```
┌──────────────────────────────────────────────────────────────────────┐
│  qCC/  CloudCompare.exe        ccViewer/ccViewer.exe                 │  ← applications
├──────────────────────────────────────────────────────────────────────┤
│  plugins/  (Standard | IO | GL)  ← loadable .so/.dll, Qt Q_PLUGIN_…  │  ← extension surface
├──────────────────────────────────────────────────────────────────────┤
│  libs/CCAppCommon               libs/CCPluginAPI  libs/CCPluginStub │  ← app scaffolding + plugin API
├──────────────────────────────────────────────────────────────────────┤
│  libs/qCC_io  libs/qCC_glWindow  libs/qCC_db                       │  ← domain libs (data / I/O / 3D)
├──────────────────────────────────────────────────────────────────────┤
│  libs/qCC_db/extern/CCCoreLib   (vendored)                          │  ← algorithms (no Qt)
└──────────────────────────────────────────────────────────────────────┘
```

The arrows point **downward** at link time: `qCC` links everything; `qCC_io` links `qCC_db` + `CCCoreLib`; `CCCoreLib` links nothing of ours (only Qt-free deps). Plugins link only **`CCCoreLib` + `CCPluginAPI` + `CCPluginStub`** plus their own third-party libs — never `qCC` or `qCC_io` internals (they consume those via headers in `qCC_io/include/`).

> **Mental check:** if you're about to `#include "mainwindow.h"` from inside a plugin, **stop**. The plugin only sees `ccMainAppInterface` (`libs/CCPluginAPI/include/ccMainAppInterface.h`).

### 1.1 What lives in each layer

| Layer | Owns | Forbidden from |
|---|---|---|
| **`CCCoreLib`** (vendored) | All algorithms — `PointCloud`, `KdTree`, `Delaunay2dMesh`, `MeshSamplingTools`, `ScalarFieldTools`, `WeibullDistribution`, etc. Pure C++17, **no Qt**. | Including any Qt header, any qCC header. |
| **`qCC_db`** | The 3D scene-graph + serializable object hierarchy: `ccHObject` (root), `ccPointCloud`, `ccMesh`, `ccPolyline`, `ccSphere`, `ccPlane`, `ccFacet`, `ccSensor`, `ccImage`, … plus drawables + selection. Builds on `CCCoreLib`. | Re-implementing algorithms (use CCCoreLib). |
| **`qCC_io`** | File-format I/O: `FileIOFilter` registry (priority-sorted), `BinFilter`, `AsciiFilter`, `PlyFilter`, `DxfFilter`, `ShpFilter`, `RasterGridFilter`, `ImageFileFilter`, `DepthMapFileFilter`. Optional `dxflib`, `shapelib`, `GDAL`. Builds on `qCC_db`. | Implementing GL, dialogs outside I/O scope. |
| **`qCC_glWindow`** | The OpenGL 3D viewport: `ccGLWindow`, `ccGLWindowInterface`, shaders, picking. | Touching the db-tree. |
| **`CCFbo`** | Tiny FBO helper (one header/source pair). | |
| **`CCPluginAPI`** | The **stable surface** plugins depend on: `ccMainAppInterface`, `ccOverlayDialog`, `ccColorScaleEditorDlg`, `ccRenderToFileDlg`, `ccPickingHub`, `ccPersistentSettings`, `ccCommandLineInterface`, `ccArgumentParser`, `ccInfoDlg`. **Stable across versions** — never break ABI here without a major bump. | Implementing domain logic. |
| **`CCPluginStub`** | The plugin-side base classes: `ccPluginInterface` (v3.2), `ccStdPluginInterface` (v1.5), `ccIOPluginInterface` (v1.3), `ccGLPluginInterface` (v1.4), `ccDefaultPluginInterface`. These are the **interfaces plugins implement**. | |
| **`CCAppCommon`** | App scaffolding shared by both `qCC` and `ccViewer`: `ccApplicationBase`, `ccPluginManager`, `ccRecentFiles`, 3D-mouse manager, dark/light stylesheets, picking hub. | GUI-specific dialogs (those live in `qCC/` or as plugins). |
| **`plugins/core/{Standard,IO,GL}`** | Shipped features as Qt-loadable shared libs. | Modifying `libs/` to make a plugin work. |
| **`plugins/example/`** | Copy-paste templates for new plugins. **Do not enable by default** (their CMakeLists all `OFF`). | Being enabled in production. |
| **`qCC/`** | The full GUI: `MainWindow`, db-tree (`db_tree/`), all dialogs (`cc*Dlg`), plugin UI manager (`pluginManager/`), command-line parser, `ccApplication`. | Linking plugins directly (plugins are loaded by `QPluginLoader`, not linked). |
| **`ccViewer/`** | Read-only viewer: own `ccviewer.cpp/h`, separate plugin scan path, no `qCC_db` GUI. | Re-using `qCC`'s `MainWindow`. |

---

## 2. The runtime model — what happens when `CloudCompare.exe` starts

Source for this sequence: `qCC/main.cpp` and `libs/CCAppCommon/src/ccPluginManager.cpp`.

```
main(argc, argv)
 └─ ccApplication(argc, argv, isCommandLine)         // qCC/ccApplication.{h,cpp}
     ├─ ccApplicationBase::InitOpenGL()              // libs/CCAppCommon
     ├─ setupPaths()                                 // sets pluginPaths, translationPath, shaderPath
     └─ QSettings → restore Global Shift params
 ├─ FileIOFilter::InitInternalFilters()              // qCC_io — registers BIN, ASCII, PLY, …
 ├─ ccPluginManager::Get().loadPlugins()             // walks pluginPaths, QPluginLoader
 │    ├─ QPluginLoader::staticInstances() (static plugins, none today)
 │    ├─ loadFromPathsAndAddToList()                 // every *.dll/*.so/*.dylib in pluginPaths
 │    └─ for each plugin: dispatch by getType()      // CC_STD / CC_IO_FILTER / CC_GL_FILTER
 │         ├─ STD → register custom-object factory (if any) + ccStdPluginInterface
 │         ├─ IO  → FileIOFilter::Register(filter) for each getFilters()
 │         └─ GL  → ccGlFilter::Register(filter)
 ├─ if (commandLine) ccCommandLineParser::Parse(...) // dispatches to plugins' registerCommands()
 └─ else MainWindow::TheInstance(); mainWindow->initPlugins()
                                              └─ m_pluginUIManager->init()
                                                   ├─ builds plugin Menu (qCC/pluginManager/)
                                                   ├─ builds plugin Toolbar(s)
                                                   └─ adds GL filter menu/toolbar
```

The two application executables (`qCC` and `ccViewer`) share the same plugin scanning code (`ccPluginManager::loadPlugins`) but use different `pluginPaths` — see [`ccViewer/AGENTS.md`](ccViewer/AGENTS.md).

---

## 3. The plugin model (the most opinionated part)

Plugins are **Qt5/Qt6 loadable shared objects**. The whole plugin system is defined by these headers in `libs/CCPluginStub/include/`:

- `ccPluginInterface.h` — base (v3.2): `getType()`, `isCore()`, `getName()`, `getDescription()`, `getIcon()`, `getReferences()`, `getAuthors()`, `getMaintainers()`, `start()`, `stop()`, `getCustomObjectsFactory()`, `registerCommands()`, `setIID()`, `IID()`.
- `ccStdPluginInterface.h` — Standard plugin (v1.5): adds `setMainAppInterface()`, `getMainAppInterface()`, **`getActions()`** (returns `QList<QAction*>`), `onNewSelection(selectedEntities)`.
- `ccIOPluginInterface.h` — I/O plugin (v1.3): adds **`getFilters()`** returning `QVector<FileIOFilter::Shared>`.
- `ccGLPluginInterface.h` — GL plugin (v1.4): adds **`getFilter()`** returning `ccGlFilter*`.

The host side (`CCPluginAPI`) defines what plugins **call back into**:

- `ccMainAppInterface` — get the main window, active GL sub-window, console, db-tree, add entities, update UI, dispatch to console, freeze/unfreeze UI, register overlay dialogs.
- `ccOverlayDialog` — base class for "MDI overlay" dialogs (the small dialogs that float above 3D views: clip box, segmentation, transformation, section extraction).
- `ccPickingHub` / `ccPickingListener` — pick points/labels/polyline interactively.
- `ccPersistentSettings` — Qt `QSettings` wrapper for plugin-specific preferences.
- `ccColorScaleEditorDlg` / `ccColorScaleSelector` — re-use CC's color scale UI.
- `ccRenderToFileDlg` — render the active 3D view to an image file.
- `ccCommandLineInterface` — register command-line verbs.

### 3.1 The three plugin archetypes

| Archetype | When to use it | Key method | File in `qCC/pluginManager` it lands in |
|---|---|---|---|
| **Standard** | The user picks a menu item / toolbar button → your code runs on the current selection. | `QList<QAction*> getActions()` + `void onNewSelection(...)` | The "Plugins" menu + a per-plugin toolbar. |
| **I/O** | You handle a new file format. CC auto-registers your filter and shows it in Open/Save dialogs. | `ccIOPluginInterface::FilterList getFilters()` | Adds an entry to the Open/Save file-type drop-downs. |
| **GL** | You post-process the 3D viewport with a shader (Eye-Dome Lighting, SSAO, …). | `ccGlFilter* getFilter()` | Adds an entry to the Display → Shader & Filters menu + a GL filter toolbar button. |

Template skeletons for all three are in `plugins/example/`. See [`AGENTS-plugin-dev.md`](AGENTS-plugin-dev.md) for the copy-rename-build cycle.

### 3.2 Plugin discovery rules (opinionated — do not deviate)

- **Every plugin must** have a `CMakeLists.txt` calling `AddPlugin(NAME <name> [TYPE io|gl|standard] [SHADER_FOLDER <name>])`. `AddPlugin` is defined in `plugins/cmake/Plugins.cmake`.
- **Every plugin must** have a `.qrc` file named `<PluginFolderName>.qrc` (it must exist; `AddPlugin` errors out if it doesn't).
- **Every plugin must** have an `info.json` next to the `.qrc` with `"type"` ∈ `{Standard, GL, I/O}` (the loader validates this — `ccPluginManager::IsMetaDataValid`).
- **Every plugin must** declare its `Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.<YourPlugin>" FILE "../info.json")` macro in the class derived from a plugin interface.
- **IID must be unique across plugins.** Convention: `ccorp.cloudcompare.plugin.<LowerCamelName>` (look at any shipped plugin's `.h`).
- **The plugin's `option(PLUGIN_<TYPE>_<NAME> ...)` flag must default to `OFF`** unless the plugin needs no external dep and should always be available. The CI matrix explicitly toggles each one.
- **A plugin must not link `qCC`, `ccViewer`, or any internal header outside the published API.** `CCPluginAPI` + `CCPluginStub` + `CCCoreLib` are the only CC headers plugins link against.

### 3.3 Plugin metadata (`info.json`)

Schema (canonical example: `plugins/example/ExamplePlugin/info.json`):

```json
{
  "type": "Standard" | "GL" | "I/O",
  "name": "Human-readable name (shown in About dialog + tooltip)",
  "icon": ":/CC/plugin/<PluginFolderName>/images/icon.png",
  "description": "Tooltip when the plugin has actions; About-dialog body otherwise.",
  "authors":    [{ "name": "...", "email": "..." }],
  "maintainers":[{ "name": "...", "email": "..." }],
  "references": [{ "text": "...", "url": "..." }]
}
```

The `icon` Qt resource path is relative to the `.qrc` `<qresource prefix="/CC/plugin/<PluginFolderName>">`. The class constructor takes the **same prefix** as an argument: `ccStdPluginInterface(":/CC/plugin/<PluginFolderName>/info.json")`. Get both right or the plugin fails `Q_PLUGIN_METADATA` loading.

### 3.4 Plugin CMake contract

The canonical shape (lift from `plugins/core/IO/qCoreIO/CMakeLists.txt`):

```cmake
option( PLUGIN_IO_QCORE "Install the CoreIO plugin" ON )   # <-- first line, OFF unless always-on

if( PLUGIN_IO_QCORE )
    project( QCORE_IO_PLUGIN )                             # target name (uppercase _PLUGIN)
    AddPlugin( NAME ${PROJECT_NAME} TYPE io )              # see plugins/cmake/Plugins.cmake
    add_subdirectory( include )
    add_subdirectory( src )
    add_subdirectory( ui )                                 # optional — only if you have .ui files
    # target_link_libraries( ${PROJECT_NAME} <external libs> )   # only what's needed beyond CCCoreLib
endif()
```

`AddPlugin` itself (in `plugins/cmake/Plugins.cmake`) does these things for you — never duplicate them:

1. Sets `AUTOUIC ON` and `AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui`.
2. Creates a `SHARED` library target.
3. Reads `<PluginFolderName>.qrc` via `qt6_add_resources()` and adds it to sources.
4. Adds `info.json` to sources.
5. Links the target against `CCCoreLib`, `CCPluginAPI`, `CCPluginStub`.
6. On Windows/Release: defines `QT_NO_DEBUG` for the target.
7. On Apple: copies the built `.dylib` to `${CMAKE_BINARY_DIR}/ccPlugins/`.
8. Records the plugin name into the internal `CC_PLUGIN_TARGET_LIST` for later install.

### 3.5 Plugin install layout

After `cmake --install`:

| OS | Plugin DLL | Plugin metadata | CI tested? |
|---|---|---|---|
| Windows | `<install>/<CLOUDCOMPARE_DEST_FOLDER>/plugins/*.dll` | `<install>/<CLOUDCOMPARE_DEST_FOLDER>/plugins/*.json` (copied from `info.json`) | Yes (full, Conda-based) |
| Linux   | `<prefix>/<LINUX_INSTALL_SHARED_DESTINATION>/cloudcompare/<plugin>.so` | alongside | Yes (slim smoke test, apt) |
| macOS   | `<app>.app/Contents/PlugIns/ccPlugins/*.dylib` | alongside | **No** (sources kept in tree, not exercised in CI) |

`ccPluginManager::loadPlugins` scans those paths automatically (see `setupPaths()` in `ccApplicationBase`).

---

## 4. The data model — `ccHObject`

The single most important type in the codebase is `ccHObject` (in `libs/qCC_db/include/ccHObject.h`). **Everything in the db-tree is a `ccHObject` subclass.** It is:

- A **tree node** (parent/children) — `ccHObject` derives from `ccObject` and `ccDrawableObject`.
- **Drawable** (renderable by `ccGLWindow`) via `ccDrawableObject`.
- **Serializable** via `ccSerializableObject` (so it can be saved to `.bin`).
- **Typed** by a `CC_CLASS_ENUM` and bit-flag class-id set (`CC_TYPES` in `ccBasicTypes.h`).

Casting is done through `ccHObjectCaster.h` (templated helpers) or `qobject_cast`-style checks on `getClassID()`. Never `dynamic_cast` on the public `ccHObject` API.

The standard pattern for "the user selected something" in a Standard plugin:

```cpp
void MyPlugin::onNewSelection(const ccHObject::Container& selectedEntities)
{
    bool hasClouds = false;
    for (ccHObject* obj : selectedEntities) {
        if (obj && obj->isA(CC_TYPES::POINT_CLOUD)) { hasClouds = true; break; }
    }
    m_action->setEnabled(hasClouds);
}
```

When your action fires, you typically:

1. Get `m_app->getMainWindow()` for modal dialogs.
2. Get `m_app->getActiveGLWindow()` for refresh calls.
3. Add the result to the db via `m_app->addToDB(obj, updateZoom, autoExpandDBTree, checkDimensions)`.
4. Force a redraw with `m_app->refreshAll()` or `m_app->updateUI()`.
5. Log to console: `m_app->dispToConsole(...)`.

---

## 5. The I/O registry — `FileIOFilter`

In `libs/qCC_io/src/FileIOFilter.cpp`:

- `FileIOFilter::InitInternalFilters()` — registers the **core, always-available** filters (BIN, ASCII, PLY; optional DXF/SHP/Raster/Image/DepthMap).
- `FileIOFilter::Register(Shared)` — **this is the public extension point used by I/O plugins** (called from `ccPluginManager::loadPlugins` for each `ccIOPluginInterface::getFilters()` entry).
- The container is **priority-sorted** (`comparePriorities` lambda). `DEFAULT_PRIORITY` is `FileIOFilter::DEFAULT_PRIORITY`; plugins should pick a number higher (lower priority) than the core filters unless they want to **override** a built-in.
- I/O plugins must subclass `FileIOFilter` and pass a `FilterInfo` struct in their constructor (id, priority, extensions, default extension, import/export filter strings, features `Import`/`Export`).

Example I/O filter (from `plugins/example/ExampleIOPlugin/src/FooFilter.cpp`):

```cpp
FooFilter::FooFilter()
    : FileIOFilter( {
        "Foo Filter",
        DEFAULT_PRIORITY,                       // priority
        QStringList{ "foo", "txt" },            // import extensions
        "foo",                                  // default extension
        QStringList{ "Foo file (*.foo)", "Text file (*.txt)" },
        QStringList(),                          // no export
        Import                                  // features
    } )
{}
```

Override `loadFile`, and (if export is enabled) `saveToFile` + `canSave`.

---

## 6. The 3D rendering pipeline (sketch)

- `ccGLWindowInterface` (in `libs/qCC_glWindow/include/`) is the public viewport API. It owns the OpenGL context + picking.
- `ccGLWindow` (same dir, internal) implements it.
- `ccDrawableObject::draw()` is what every drawable overrides.
- GL plugins return a `ccGlFilter` from `getFilter()`. These run as a post-process pass over the framebuffer; the GL menu/toolbar comes from `qCC/pluginManager/ccPluginUIManager::enableGLFilter()`.

---

## 7. The command-line mode

`qCC/main.cpp` detects command-line vs GUI at startup. In CLI mode:

1. `FileIOFilter::InitInternalFilters()` runs as usual.
2. `ccPluginManager::loadPlugins()` runs as usual — **this is what gives CLI access to plugin-registered commands and I/O filters**.
3. `ccCommandLineParser::Parse(arguments, pluginList)` does everything: `LOAD`, `SAVE`, `OPEN`, command-line verbs added by `ccStdPluginInterface::registerCommands(cmd)`, etc.

So **a plugin that registers a CLI command via `registerCommands()` works in both GUI and CLI mode** without any extra wiring. See `plugins/core/Standard/qCompass` or `qAnimation` for examples.

---

## 8. The "don't do this" list

These come from looking at the code, not from `CONTRIBUTING.md`. They will break the build or break the architectural contract:

- **Don't** add a `#include <QtWidgets>` in a plugin when you only need `Qt::CaseSensitivity`. Use forward declarations and `<QString>`.
- **Don't** `#include "mainwindow.h"` from a plugin. Use `ccMainAppInterface`.
- **Don't** add new code to `libs/qCC_io/src/FileIOFilter.cpp` to add a format. Make it a plugin instead — see `qLASIO`, `qE57IO`, `qDracoIO`.
- **Don't** add a new dialog in `qCC/` for a feature that belongs to a plugin. The plugin can host its own `.ui` files via `AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui` (already set up by `AddPlugin`).
- **Don't** change a `ccPluginInterface` version (v3.2 → v3.3) without bumping the `Q_DECLARE_INTERFACE` string and the plugin loader's check. Existing plugins will silently fail to load.
- **Don't** add `.cpp/.h` files to `qCC/` without going through `qt6_wrap_ui()` for `.ui` files and `qt6_add_resources()` for `.qrc` files. See `qCC/CMakeLists.txt`.
- **Don't** link a plugin against `qCC` or `ccViewer`. The contract is `CCCoreLib + CCPluginAPI + CCPluginStub` + your own deps.
- **Don't** name a new class without the `cc` prefix, a new plugin `.qrc` after anything other than the folder name, or an `info.json` without `"type"`.
