# libs/CCAppCommon/AGENTS.md

Application scaffolding shared by **`qCC`** and **`ccViewer`**. Owns `ccApplicationBase` (the `QApplication` subclass), `ccPluginManager` (the plugin scanner), and the 3D-mouse integration.

**CMake target:** `CCAppCommon` (SHARED, GPL).
**Public headers:** `include/`.

Read [`../../AGENTS-libs.md`](../../AGENTS-libs.md) §8.

## Folder layout

```
CCAppCommon/
├── CMakeLists.txt
├── include/                       # public headers
├── src/                           # implementations
├── ui/                            # QDialog/.ui pieces used by both apps
├── devices/
│   └── 3dConnexion/
│       ├── 3dConnexionDriver*     # 3D-mouse integration
│       └── extern/hidapi/         # submodule (caps CMake at 4.3!)
├── QDarkStyleSheet/               # submodule (dark/light QSS)
├── CMakeLists.txt                 # top-level for the lib
└── images/                        # bundled icons
```

## Key public headers

| Header | Role |
|---|---|
| `ccApplicationBase.h` / `.cpp` | The `QApplication` subclass. `InitOpenGL()`, plugin paths, translation path, shader path. The `ccApp` macro is the global handle (`#define ccApp (static_cast<ccApplicationBase*>(QCoreApplication::instance()))`). |
| `ccPluginManager.h` / `.cpp` | Scans plugin paths, loads `.so/.dll/.dylib` via `QPluginLoader`, dispatches by `getType()`. **Single source of plugin discovery.** |
| `ccRecentFiles.h` / `.cpp` | Recent files menu state (used by `qCC` and `ccViewer`). |

## The plugin scan flow (entry point)

When `CloudCompare.exe` (or `ccViewer.exe`) starts:
1. `ccApplicationBase::setupPaths()` populates `m_pluginPaths` from `QStandardPaths` / install layout.
2. `qCC/main.cpp` (or `ccViewer/main.cpp`) calls `ccPluginManager::Get().loadPlugins()`.
3. `ccPluginManager` walks every path, runs `QPluginLoader::instance()`, validates the `Q_PLUGIN_METADATA` `info.json` `type`, and registers filters / GL filters / Standard actions accordingly.
4. `MainWindow::initPlugins()` (or `ccviewer`'s equivalent) then asks `ccPluginUIManager` to wire UI.

**Adding a new plugin path** (e.g. a per-user drop folder) → override `ccApplicationBase::setupPaths()` or set `m_pluginPaths` before `loadPlugins()`.

## Vendored submodules — DO NOT EDIT

- **`QDarkStyleSheet/`** — QSS stylesheets, pinned upstream.
- **`devices/3dConnexion/extern/hidapi/`** — hidapi C API. **`CMakeLists.txt` caps CMake at 4.3** (`cmake_minimum_required(VERSION 3.6.3...4.3 FATAL_ERROR)`). Don't try to upgrade past 4.3 — see root [`../../AGENTS.md`](../../AGENTS.md) gotcha #1.

## Don't

- Don't add GUI dialogs here — they belong in `qCC/` (or as plugins). `CCAppCommon` is app *scaffolding*, not app *content*.
- Don't move `ccPluginManager` to `qCC/` — `ccViewer` also needs it.
- Don't edit the hidapi or QDarkStyleSheet submodules.

## See also

- Root [`../../AGENTS.md`](../../AGENTS.md)
- [`../../AGENTS-architecture.md`](../../AGENTS-architecture.md) §2 (startup sequence)
- [`../../AGENTS-libs.md`](../../AGENTS-libs.md) §8
