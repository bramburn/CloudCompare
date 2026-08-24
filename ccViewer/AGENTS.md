# ccViewer/AGENTS.md

`ccViewer.exe` — the **minimal, read-only** 3D viewer. Same OpenGL viewport and same plugin scanner as `qCC`, but no edit tools, no db-tree, no command-line parser (well, almost — see `ccviewer.cpp`).

```
ccViewer/
├── CMakeLists.txt                # builds ccViewer.exe
├── main.cpp                      # main() — similar to qCC/main.cpp but minimal
├── ccViewerApplication.cpp/.h    # the QApplication subclass for ccViewer
├── ccviewer.cpp/.h               # the QMainWindow for ccViewer
├── ccviewerlog.h
├── ui_templates/                 # the main window's .ui file
├── Mac/                          # macOS bundle (ccViewer.plist)
├── bin_other/                    # misc platform bits
└── images/                       # icons
```

## How it differs from `qCC/`

- **No editing.** The viewer is read-only by design.
- **Own plugin scan path.** Set in `ccViewerApplication` (so its plugins don't clash with `qCC`'s).
- **Own dialogs / actions.** Only what's needed to view entities, switch views, manage cameras, etc.
- **No command-line parser** (use `qCC -SILENT_MODE` if you need headless behaviour).

## Editing ccViewer

Same rules as `qCC/`:
- New features still go into `plugins/`.
- Built-in viewer-only UI lives here.

## macOS bundle (not CI-tested)

`ccViewer/Mac/` holds `ccViewer.plist` (Info.plist — bundle identifier was verified by the now-removed `.ci/verify_macos_bundle_identifiers.py`; if you re-enable macOS CI, recreate that script) and bundle assembly glue.

> ⚠️ The fork dropped macOS support on 2026-08-24. These sources are **kept for local macOS builds** but are not exercised in CI. See [`AGENTS.md` §CI](../AGENTS.md#ci).

## Don't

- Don't duplicate `qCC/` dialogs here — share code via the `libs/` libraries, or accept that the viewer is intentionally limited.
- Don't add a command-line parser here — `qCC` has that.
- Don't add `qCC_db` GUI features (scalar-field editor, db-tree, …) here — the viewer is intentionally minimal.

## See also

- Root [`../AGENTS.md`](../AGENTS.md)
- [`../AGENTS-architecture.md`](../AGENTS-architecture.md) §2 (startup sequence — `ccViewer` is a smaller version of `qCC`'s)
- [`../AGENTS-ui.md`](../AGENTS-ui.md)
- [`../AGENTS-libs.md`](../AGENTS-libs.md) §8 (`CCAppCommon` is shared between `qCC` and `ccViewer`)
