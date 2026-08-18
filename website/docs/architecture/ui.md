---
title: UI patterns
sidebar_label: UI
sidebar_position: 5
---

# UI patterns

The UI conventions used throughout `qCC/`. The Qt patterns are
deliberately conservative — the same patterns repeat across the
codebase, and matching them keeps the codebase legible.

## MDI, not single-window

`qCC` is an **MDI** (multiple-document-interface) app. The main
window holds a `QMdiArea`; every loaded file gets a child window
inside it. The child window is a `ccGLWindow` (3D viewport) or a
`cc2DViewport` (ortho).

This means **don't use `QMainWindow` directly** for new features —
use a `QDialog` or a docked widget inside the MDI area instead. The
MDI is the reason the db-tree, console, and 3D viewport are
separate from the document window.

## The db-tree

The db-tree is a `QTreeView` backed by a `ccHObjectTreeModel` (in
`qCC/db_tree/`). The model is a thin adapter from
`ccHObjectContainer` to `QAbstractItemModel`. To add a custom column
or a custom context menu, subclass the model and register it in
`mainwindow.cpp`.

Context-menu actions on db-tree items are the right place for "I want
to do X with this entity" features. The actions live in
`qCC/db_tree/ccDBTree.cpp` — find the `addEntityType` block that
matches your entity class.

## The console

The console is a `QTextEdit` subclass (`ccConsole`) wrapped as a
`QDockWidget`. Plugins print to it via
`m_app->dispToConsole(message, level)`. Levels are `LOG_STANDARD`,
`LOG_WARNING`, `LOG_ERROR`, and `LOG_DEBUG`.

The console is **the** way to surface non-fatal errors. Don't open a
modal `QMessageBox` for routine errors — the user has dismissed six
of them in the last minute, they'll dismiss yours too.

## Dialogs: `ccOverlayDialog` vs `QDialog`

Two patterns:

- **Standalone** — use a plain `QDialog`. Submit on `accept()`, cancel
  on `reject()`. The dialog is modal to the active 3D window.
- **Overlay** — use `ccOverlayDialog`. This is a frameless,
  semi-transparent dialog that floats over the 3D window. The user
  can keep manipulating the 3D view while the dialog is open. Use
  this for tools where the user needs to keep seeing the scene
  (e.g. the "pick a point" workflow in qM3C2).

The `ccOverlayDialog` base class is in `libs/qCC_io/CC/include/
ccOverlayDialog.h`. Subclass and override `start()` (called when the
dialog opens) and `stop()` (called when it closes).

## Qt Designer files

Dialogs are designed in Qt Designer. The `.ui` file lives next to the
dialog's `.h` and `.cpp`. The convention is `<plugin>Dialog.ui` for
a Standard plugin and `<plugin>OpenDialog.ui` /
`<plugin>SaveDialog.ui` for an I/O plugin.

To open a `.ui` file:

```bash
"C:\dev\tools\Qt\6.8.3\msvc2022_64\bin\designer.exe" myDialog.ui
```

(or `designer-qt6` on Linux/macOS).

## Action icons

Every action needs an icon. Icons live in `<plugin>/Resources/`
as `.png` files. The convention is `icon.png` (the plugin's main
icon), plus per-action icons named after the action
(e.g. `action_csf.png`).

If you need a new icon, generate it at 32x32 and 64x64 (the toolbar
uses 32x32, the menu uses 16x16 and 32x32 depending on the Qt style).
Save as 32-bit PNG with alpha — the toolbar background is theme-
dependent.

## String externalisation

Every user-visible string is wrapped in `tr("…")` so it can be
translated. The fork's translation files live in
`qCC/i18n/CloudCompare_<lang>.ts`; update them with `lupdate`.

For new strings, `lupdate` will pick them up automatically on the
next CI run.

## What to read next

- [Plugins / Authoring](/docs/plugins/authoring) — the cookbook for
  the Standard / I/O / GL plugin patterns.
