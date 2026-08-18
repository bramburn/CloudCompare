# libs/CCPluginAPI/AGENTS.md

**The stable public surface for plugins.** This is the boundary between "internal CloudCompare" and "what third-party plugins can call." Treat changes here as a public-API change.

**CMake target:** `CCPluginAPI` (SHARED, GPL).
**Public headers:** `include/` (consumed by `qCC/`, `ccViewer/`, every plugin).

Read [`../../AGENTS-libs.md`](../../AGENTS-libs.md) §6 for the full surface; [`../../AGENTS-architecture.md`](../../AGENTS-architecture.md) §3 for the runtime model.

## Folder layout

```
CCPluginAPI/
├── CMakeLists.txt
├── include/                       # THE public API (stable)
└── src/                           # implementations of the headers
```

## Public API surface (must-know subset)

| Header | What it's for |
|---|---|
| `ccMainAppInterface.h` | The host callback — get main window, console, db-tree, freeze UI, register overlay dialog, add to DB, refresh, … |
| `ccOverlayDialog.h` | Base class for MDI overlay dialogs (clip box, segmentation, transformation, section extraction) |
| `ccPickingHub.h`, `ccPickingListener.h` | Interactive picking (point / label / polyline) |
| `ccPersistentSettings.h` | `QSettings` wrapper with namespaced keys |
| `ccCommandLineInterface.h` | Register command-line verbs |
| `ccArgumentParser.h` | Typed argv parsing helper |
| `ccInfoDlg.h` | Pre-styled "About this plugin" dialog |
| `ccColorScaleEditorDlg.h`, `ccColorScaleEditorWidget.h`, `ccColorScaleSelector.h` | Re-use CC's color scale UI |
| `ccRenderToFileDlg.h` | Render-to-PNG/EXR dialog |
| `ccQtHelpers.h` | Misc Qt utilities (button creation, font scaling) |
| `CCPluginAPI.h` | The `CCPLUGIN_LIB_API` import/export macro |

## Editing rules

- **Adding a method** to `ccMainAppInterface` is a breaking change for every plugin that implements the interface. See [`../../AGENTS-plugin-dev.md`](../../AGENTS-plugin-dev.md) §6.
- **Renaming a header** is a breaking change for every plugin that includes it.
- **Bumping a `Q_DECLARE_INTERFACE` version** (in `CCPluginStub/include/cc*PluginInterface.h`) is the formal way to declare a breaking change.
- Add the GPL header to new files (this lib is GPL because it links into `qCC`).
- Match the style in [`../../AGENTS-coding-standards.md`](../../AGENTS-coding-standards.md).

## Don't

- Don't move a header out of `include/` into `src/` — that removes it from the public surface (good or bad depending on intent; **announce it in the PR**).
- Don't add a `Q_OBJECT` class here that depends on a private header in `qCC/`. This lib must remain Qt-UI-only on its public surface, not pull in `mainwindow.h`.
- Don't `#include` anything from `qCC/`, `ccViewer/`, or `qCC_db/src/` here. Use `qCC_db/include/`.

## See also

- Root [`../../AGENTS.md`](../../AGENTS.md)
- [`../../AGENTS-architecture.md`](../../AGENTS-architecture.md) §3.1
- [`../../AGENTS-libs.md`](../../AGENTS-libs.md) §6
