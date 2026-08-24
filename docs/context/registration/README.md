# docs/context/registration/README.md

Layered context for the **manual dual-screen point-cloud registration** feature. This is the WHAT — what files, what classes, what data flows. For the WHY and the goal-level architecture, see [`../../AGENTS_REGISTRATION.md`](../../../AGENTS_REGISTRATION.md).

---

## 1. How to read this folder

The user-provided analysis framework recommended splitting the LLM agent context into three layers (Global / Domain / Per-task). This folder is the **Domain layer**. The Global layer is the root [`AGENTS.md`](../../../AGENTS.md); per-task prompts should compose from this folder.

| File | When to read it |
|---|---|
| [`data-flow.md`](data-flow.md) | First — read this end-to-end before touching code. Maps the user's actions to the code paths. |
| [`picking-system.md`](picking-system.md) | When implementing M3 (cross-window picking) or any per-listener routing. |
| [`dual-viewport.md`](dual-viewport.md) | When implementing M2 (dual `ccGLWindow` lifecycle). |
| [`transform-math.md`](transform-math.md) | When implementing M4 (preview) or M5 (commit). |

**Driving an LLM agent:** the goal doc's [`§11 Phase-by-phase agent prompts`](../../../AGENTS_REGISTRATION.md) contains five copy-pasteable prompts (recon → PRD → AGENTS → README → milestones) with the exact instructions. Run them in order; do not skip Phase 1.

---

## 2. Phase A — "Nearest neighbour" feature (read these end-to-end first)

Don't read top-down. Read these four files end-to-end *before* writing any code:

| File | Why |
|---|---|
| `qCC/ccPointPairRegistrationDlg.h` (260 lines) | The single-window manual registration dialog — this is the closest existing feature. Its API (`addAlignedPoint`, `addReferencePoint`, `removeAlignedPoint`, `removeRefPoint`, `onItemPicked`) is what we'll mirror in dual-window form. |
| `qCC/ccPointPairRegistrationDlg.cpp` (~1,400 lines) | The full implementation — picking registration, label-as-marker pattern, the Umeyama registration function at line 1240, the commit flow. **Read the constructor, the `onItemPicked` slot, and the `UmeyamaRegistration` static.** |
| `qCC/ui_templates/pointPairRegistrationDlg.ui` | The dialog layout — pair table, "Add / Remove" buttons, "Align" button, "Reset" button. We keep the pair table; we add a *second* viewport widget to the layout. |
| `qCC/mainwindow.cpp` around line 6980 (`activateRegisterPointPairTool`) | How the existing dialog is opened by `MainWindow`: it allocates a dedicated `ccGLWindow` via `new3DView()`, disables the other windows, registers the overlay dialog, and starts picking. We do almost the same thing but with **two** GL windows instead of one, and from a plugin instead of `MainWindow`. |

## 3. Phase B — Data flow (summary)

The full version is in [`data-flow.md`](data-flow.md). The 10,000-foot view:

```
[User selects 2 clouds in db-tree]
        ↓
[User clicks Tools → Manual Registration]
        ↓ plugin's getActions() returns m_action → onTriggered()
[Plugin: open dialog, allocate 2 ccGLWindow via m_app->createGLWindow()]
        ↓
[User picks point in source viewport]   [User picks point in target viewport]
        ↓ ccPickingHub→onItemPicked      ↓ ccPickingHub→onItemPicked
[Dialog: store aligned[i] = CCVector3]  [Dialog: store ref[i] = CCVector3]
        ↓
[User clicks Preview ≥3 pairs]
        ↓ Eigen Umeyama (from ccPointPairRegistrationDlg.cpp::UmeyamaRegistration)
[Dialog: ccGLMatrix T]
        ↓
[Dialog: source->setGLTransformation(T); source->enableGLTransformation(true)]
[Dialog: source's ccGLWindow->redraw()]
        ↓
[User clicks Apply]
        ↓
[Dialog: source->applyRigidTransformation(T)]
[Dialog: source->resetGLTransformation(); source->enableGLTransformation(false)]
[Dialog: m_app->addToDB(source, ...); m_app->refreshAll()]
        ↓
[Dialog closes; db-tree shows the moved cloud]
```

## 4. Phase C — Extension points

The user's framework asked "is this a plugin or a core change?" Our decision (per [`../../AGENTS_REGISTRATION.md`](../../../AGENTS_REGISTRATION.md) §8): **Standard plugin at `plugins/core/Standard/qManualRegistration/`**.

Three reasons:

1. **`ccMainAppInterface::createGLWindow(...)` already exists** in `libs/CCPluginAPI/include/ccMainAppInterface.h` (lines 80-99). A plugin can own its own `ccGLWindow`s without touching `qCC/mainwindow.cpp`.
2. **The plugin contract already covers Standard plugins** with multiple actions, GL filter integration, CLI command registration, and custom-object factories. See [`../../AGENTS-plugin-dev.md`](../../../AGENTS-plugin-dev.md) §1.1.
3. **`qCC/mainwindow.h` is a 900-line header with 900-line implementation.** Modifying it for v1 would slow review and is exactly what the plugin system was designed to avoid.

The one thing the plugin API doesn't have today is a "this is the active GL window" notification when the **plugin's** windows are active (vs. the MDI's). If we hit that, we bump `ccMainAppInterface` (and the version string) — see [`../../AGENTS-plugin-dev.md`](../../../AGENTS-plugin-dev.md) §6.

## 5. File map — what we'll create (v1, plugin-only)

| Path | Role | New / existing |
|---|---|---|
| `plugins/core/Standard/qManualRegistration/CMakeLists.txt` | Plugin CMakeLists (option + AddPlugin) | new |
| `plugins/core/Standard/qManualRegistration/qManualRegistration.qrc` | Qt resource file | new |
| `plugins/core/Standard/qManualRegistration/info.json` | Plugin metadata | new |
| `plugins/core/Standard/qManualRegistration/images/icon.png` | Icon | new |
| `plugins/core/Standard/qManualRegistration/include/qManualRegistration.h` | The `ccStdPluginInterface`-derived class | new |
| `plugins/core/Standard/qManualRegistration/include/qManualRegistrationDlg.h` | The dual-viewport dialog | new |
| `plugins/core/Standard/qManualRegistration/src/qManualRegistration.cpp` | Plugin entry point (`getActions`, `onNewSelection`) | new |
| `plugins/core/Standard/qManualRegistration/src/qManualRegistrationDlg.cpp` | Dialog implementation | new |
| `plugins/core/Standard/qManualRegistration/src/PairRegistration.cpp` | Wrapper around `UmeyamaRegistration` + RMS | new |
| `plugins/core/Standard/qManualRegistration/ui/qManualRegistrationDlg.ui` | Dialog layout (designer file) | new |
| `plugins/core/Standard/qManualRegistration/src/CMakeLists.txt` + `include/CMakeLists.txt` | Sub-folder CMake glue | new |
| `plugins/core/Standard/CMakeLists.txt` | Add `add_subdirectory(qManualRegistration)` | modify (one line) |
| `.github/workflows/build.yml` | Add `-DPLUGIN_STANDARD_QMANUAL_REGISTRATION=ON` to all 3 jobs (Windows MSVC, Ubuntu GCC, Ubuntu Clang — macOS dropped 2026-08-24) | modify |
| `cc-configure.cmd` (local only) | Add `-DPLUGIN_STANDARD_QMANUAL_REGISTRATION=ON` | modify |

That's it for v1. **Zero files in `qCC/`, `ccViewer/`, or `libs/` are touched.** This is the plugin system's promise being kept.

## 6. File map — what we read but don't modify (v1)

| Path | Why |
|---|---|
| `qCC/ccPointPairRegistrationDlg.{h,cpp}` + `qCC/ui_templates/pointPairRegistrationDlg.ui` | The single-window reference; copy the math, copy the table layout, NOT the wiring |
| `qCC/ccComparisonDlg.{h,cpp}` + `qCC/ui_templates/comparisonDlg.ui` | Pattern for the RMS / per-pair table UI |
| `libs/CCPluginAPI/include/ccMainAppInterface.h` | `createGLWindow` / `destroyGLWindow` / `registerOverlayDialog` / `addToDB` / `freezeUI` |
| `libs/CCPluginAPI/include/ccOverlayDialog.h` | The dialog base class (if we want overlay behaviour) |
| `libs/CCPluginAPI/include/ccPickingHub.h` | Multiple-listener picking (see [`picking-system.md`](picking-system.md)) |
| `libs/CCPluginAPI/include/ccPickingListener.h` | The `PickedItem` struct + `onItemPicked` callback |
| `libs/qCC_db/include/ccGLMatrix.h` + `ccGLMatrixTpl.h` | The matrix type |
| `libs/qCC_db/include/ccHObject.h` (`applyGLTransformation_recursive`, `setGLTransformation_history`) + `libs/qCC_db/include/ccDrawableObject.h` (`setGLTransformation`, `enableGLTransformation`, `resetGLTransformation`) | Preview-only transform hook |
| `libs/qCC_db/include/ccPointCloud.h` (`applyRigidTransformation`, line 623) | Commit transform |
| `libs/qCC_db/extern/CCCoreLib/include/RegistrationTools.h` (`HornRegistrationTools::FindAbsoluteOrientation`) + `…/PointProjectionTools.h` (Umeyama) | The math |
| `qCC/mainwindow.cpp` `applyTransformation` (line 1144) | Reference for the commit path (we don't call this from a plugin; we call `ccPointCloud::applyRigidTransformation` directly) |

## 7. Pointers

- [`../../AGENTS_REGISTRATION.md`](../../../AGENTS_REGISTRATION.md) — the goal-level doc
- [`../../AGENTS-architecture.md`](../../../AGENTS-architecture.md) — plugin model
- [`../../AGENTS-plugin-dev.md`](../../../AGENTS-plugin-dev.md) — full plugin recipe (use §1.1 for the Standard-plugin variant)
- [`../../AGENTS-libs.md`](../../../AGENTS-libs.md) — where each lib owns what
- [`../../AGENTS-ui.md`](../../../AGENTS-ui.md) — dialog patterns
- [`../../AGENTS-coding-standards.md`](../../../AGENTS-coding-standards.md) — file headers, naming, clang-format
