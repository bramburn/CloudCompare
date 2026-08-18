---
title: Plugin system
sidebar_label: Plugin system
sidebar_position: 4
---

# Plugin system

The plugin system is the **only** extension point in CloudCompare. The
core libraries don't have a "register a new algorithm" hook — to add
something, you write a plugin. This page is the lifecycle.

## The interfaces (`libs/CCPluginAPI/`)

The interfaces are pure-virtual C++ classes. The interesting ones:

| Interface | Header | What it adds |
|---|---|---|
| `ccPluginInterface` | `ccPluginInterface.h` | Base for all plugins. Provides `getName()`, `getDescription()`, `getVersion()`, `getAuthor()`. |
| `IStandardPlugin` | `ccStdPluginInterface.h` | Adds a menu entry under **Plugins > Standard Plugins**. |
| `IGLPlugin` | `ccGLPluginInterface.h` | Adds an OpenGL post-filter on the viewport. |
| `FileIOFilter` | `FileIOFilter.h` | Adds a file format to **File > Open** / **File > Save**. |
| `ccGLFilterPlugin` | `ccGLFilterPluginInterface.h` | Adds a GLSL shader filter to the viewport. |
| `ccOverlayDialog` | `ccOverlayDialog.h` | Helper for plugins that want a docked dialog. |

The three plugin *types* in
[Plugins / Overview](/docs/plugins/overview) are just
`IStandardPlugin`, `IGLPlugin`, and `FileIOFilter` (a single plugin
DLL can implement more than one).

## The loading sequence

When `qCC/CloudCompare.exe` starts:

1. `ccPluginManager::loadPlugins()` is called from
   `mainwindow.cpp`'s constructor.
2. For each known plugin path (`./plugins/standard`,
   `./plugins/io`, `./plugins/gl`):
   - List every `.dll` / `.so` / `.dylib` in the directory.
   - For each file, call `QPluginLoader(filename).instance()`.
   - The `QPluginLoader` resolves the `qt_plugin_instance` symbol
     exported by `Q_EXPORT_PLUGIN2`.
   - `qobject_cast` to the relevant interface; if the cast succeeds,
     register the plugin.
3. `ccPluginManager::init()` calls each plugin's `init()` (if any) and
   then asks each one for its actions / filters / shaders.
4. The host wires the returned actions into the menu, the filters
   into the file dialog, and the shaders into the viewport's filter
   list.

## The `ccMainAppInterface`

A plugin needs to talk to the host. It does this through
`ccMainAppInterface`, a wide API surface that includes:

| Member | Purpose |
|---|---|
| `dbTreeRoot()` | The top-level `ccHObjectContainer` — the plugin can add entities here. |
| `getActiveGLWindow()` | The current 3D viewport (or `nullptr` if no 3D window is open). |
| `getSelectedEntities()` | The current selection in the db-tree. |
| `dispToConsole(QString, ConsoleMessageLevel)` | Print to the console panel. |
| `addToDB(ccHObject*, bool updateZoom = true)` | Add an entity to the db-tree, optionally refit the camera. |
| `removeFromDB(ccHObject*)` | Remove an entity. |
| `setSelectedInDB(ccHObject*, bool selected)` | Toggle the db-tree selection. |
| `freezeUI(bool)` | Disable the UI while a long operation runs. |
| `refreshAll(RefreshFlags)` | Redraw the viewport. |
| `createCamera()` | Create a new `ccCamera` (a sensor / calibrated image). |

A plugin should **never** reach past `ccMainAppInterface` into the
host's internals. If you need a piece of the host that isn't on the
interface, that's a feature request against the upstream
`ccMainAppInterface`.

## The plugin stub

`CCPluginStub` is a small library that compiles a "null" plugin —
useful for unit tests of the plugin-loading code path. The fork's
CI uses it to verify that a fresh checkout still loads all 18
default plugins.

## Adding a new plugin type

If your plugin doesn't fit any of the existing interfaces, you need
to add a new one:

1. Add a new pure-virtual class to `CCPluginAPI/include/`.
2. Add a registration site in `ccPluginManager`.
3. Add a menu / file-dialog / viewport hook in `qCC/mainwindow.cpp`.

This is **not** a routine change — it's a core modification. Bring it
up on the upstream forum before doing it.

## What to read next

- [Plugins / Authoring](/docs/plugins/authoring) — the cookbook for
  adding a plugin end-to-end.
- [UI](/docs/architecture/ui) — the Qt side of how the plugin actions get wired into
  menus and dialogs.
