# qCC/AGENTS.md

The full GUI: `CloudCompare.exe`. ~12,000-line `QMainWindow` + all "built-in" dialogs/tools + the plugin UI manager + the command-line parser.

**Don't add new features here.** New features go into `plugins/core/<TYPE>/<NAME>/`. Edit `qCC/` only when:
- You're fixing a bug in a built-in dialog/tool.
- You're changing the db-tree, MDI area, or console behavior.
- You're adding a top-level menu/toolbar that's not plugin-driven (rare).

Read [`../AGENTS-ui.md`](../AGENTS-ui.md) for the UI conventions; [`../AGENTS-coding-standards.md`](../AGENTS-coding-standards.md) for the file/style rules.

## Folder layout

```
qCC/
├── CMakeLists.txt                # builds CloudCompare.exe
├── main.cpp                      # main(), CLI vs GUI dispatch
├── mainwindow.h / .cpp           # the QMainWindow (everything plugs into here)
├── ccApplication.h / .cpp        # the QApplication subclass
├── ccConsole.h / .cpp            # in-app console widget
├── ccCommandLineParser.{h,cpp}   # CLI parser
├── ccCommandLineCommands.{h,cpp} # CLI verb implementations
├── ccEntityAction.{h,cpp}        # right-click context-menu helpers
├── ccRecentFiles.{h,cpp}
├── ccReservedIDs.h               # reserved ccHObject unique IDs
├── ccUtils.{h,cpp}               # misc helpers
├── ccTranslationManager.{h,cpp}  # translations
├── cc*PickingGenericInterface.*  # picking helpers
├── cc*Dlg.{h,cpp}                # one per "built-in" dialog (50+ of them)
├── db_tree/
│   ├── ccDBRoot.{h,cpp}                    # db-tree root (a ccHObject)
│   ├── ccPropertiesTreeDelegate.{h,cpp}    # right-pane entity properties
│   ├── sfEditDlg.{h,cpp}                   # scalar-field editor
│   └── matrixDisplayDlg.{h,cpp}            # 4x4 transform matrix display
├── pluginManager/
│   ├── ccPluginUIManager.{h,cpp}           # dynamic menu/toolbar builder
│   ├── ccPluginInfoDlg.{h,cpp}             # Help → About → Plugins
│   ├── pluginManager.qrc
│   └── ui/
├── ui_templates/
│   └── *.ui                                # Qt Designer files (one per cc*Dlg)
├── images/
│   └── icon/                               # app + button icons
├── Mac/                                    # macOS bundle (Info.plist)
├── bin_other/                              # misc platform-specific bits
└── TODO.txt                                # notes (read occasionally)
```

## The `MainWindow` class

- **Header:** `qCC/mainwindow.h` — declares all `QAction` members, all `doAction*` slots, all tool pointer members.
- **Impl:** `qCC/mainwindow.cpp` — ~12,000 lines. Constructor wires `QAction`s to slots; slots do the actual work.

**To find where a menu item is wired:** grep for the action's `objectName` (e.g. `actionSaveFile`) in `mainwindow.cpp`'s constructor.

**To add a new menu action** (rare — prefer a plugin): follow the recipe in [`../AGENTS-ui.md`](../AGENTS-ui.md) §3.

## Built-in dialogs (the `cc*Dlg` zoo)

~50 dialog classes, all `cc<Feature>Dlg.{h,cpp}` with a matching `ui_templates/cc<Feature>Dlg.ui`. The convention is:
- `ccXxxDlg` is the `QDialog` (or `QWidget`) subclass.
- Its layout is `ui_templates/ccXxxDlg.ui`, loaded via `setupUi(this)` in the constructor.
- Modal dialogs are shown with `exec()`; modeless (tool) dialogs are added to the MDI overlay area via `m_app->registerOverlayDialog(this, Qt::Corner)`.

Examples to copy from when adding a new one:
- `ccFilterByValueDlg`, `ccNoiseFilterDlg`, `ccSORFilterDlg` — modal "apply a filter to the current selection" dialogs.
- `ccRegistrationDlg`, `ccAlignDlg` — modal multi-step workflows.
- `ccClippingBoxTool`, `ccGraphicalSegmentationTool`, `ccGraphicalTransformationTool` — interactive tools (overlay dialogs).

## Built-in tools (overlay dialogs)

These are the long-running interactive tools that grab the cursor. They all inherit `ccOverlayDialog` (from `CCPluginAPI`):
- `ccClippingBoxTool`
- `ccGraphicalSegmentationTool`
- `ccGraphicalTransformationTool`
- `ccSectionExtractionTool`
- `ccTracePolylineTool`
- `ccVolumeCalcTool`
- `ccRasterizeTool`
- `ccCropTool`
- `ccPointListPickingDlg`
- `ccPointPairRegistrationDlg`
- `ccPointPickingGenericInterface`

Pattern: `m_app->registerOverlayDialog(this, Qt::TopRightCorner);` + `m_app->freezeUI();` on open, mirror on close.

## The db-tree (`db_tree/`)

- `ccDBRoot` **is a `ccHObject`** — owns every loaded entity. The whole loaded scene is a subtree rooted here.
- The `QTreeView` is bound to `ccDBRoot` via `QStandardItemModel` in `MainWindow`.
- The right pane is a **property tree** (`ccPropertiesTreeDelegate`) — switches based on the selected entity type.
- `ccDBRoot` is exposed to plugins via `m_app->db()`.

## The console (`ccConsole`)

- The static methods `ccConsole::Print`, `ccConsole::Warning`, `ccConsole::Error` are used everywhere in the codebase as a poor-man's logging.
- From a plugin: `m_app->dispToConsole(message, level)`.
- Don't use `qDebug()` / `qWarning()` for user-visible messages — they go to stderr.

## The command-line mode

`qCC/main.cpp` detects CLI vs GUI at startup. In CLI mode:
1. `FileIOFilter::InitInternalFilters()` runs.
2. `ccPluginManager::loadPlugins()` runs — **this is what gives CLI access to plugin-registered commands**.
3. `ccCommandLineParser::Parse(arguments, pluginList)` does everything else.

The CLI verb implementations live in `ccCommandLineCommands.{h,cpp}`. Plugin-specific CLI commands live in each plugin's `registerCommands(ccCommandLineInterface*)`.

## macOS bundle

`qCC/Mac/` holds the macOS bundle resources:
- `CloudCompare.plist` — Info.plist (bundle identifier is verified by `.ci/verify_macos_bundle_identifiers.py`).
- `CMakeLists.txt` — bundle assembly glue.

## Don't

- Don't add a new top-level feature here — make a plugin.
- Don't edit `mainwindow.cpp`'s `doAction*` slot ordering — append, don't reorder.
- Don't bypass the `.ui` files by hand-rolling layouts in `.cpp`.
- Don't use `qDebug()` for user-visible messages.
- Don't `delete` `ccHObject`s you didn't `new` — use `removeFromChildren()` or hand them to `m_app->addToDB(...)`.
- Don't put `using namespace std;` in `mainwindow.h`.

## See also

- Root [`../AGENTS.md`](../AGENTS.md)
- [`../AGENTS-architecture.md`](../AGENTS-architecture.md)
- [`../AGENTS-ui.md`](../AGENTS-ui.md) — UI patterns in detail
- [`../AGENTS-coding-standards.md`](../AGENTS-coding-standards.md)
- [`../AGENTS-plugin-dev.md`](../AGENTS-plugin-dev.md) — when you should be making a plugin instead
