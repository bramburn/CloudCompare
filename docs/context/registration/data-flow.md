# docs/context/registration/data-flow.md

End-to-end data flow for the manual dual-screen registration plugin. The "what happens when the user does X" map.

---

## 0. Cast of characters (the types involved)

| Type | Where | Role |
|---|---|---|
| `ccStdPluginInterface` | `libs/CCPluginStub/include/ccStdPluginInterface.h` | Our base class. Provides `m_app` (the `ccMainAppInterface*`). |
| `ccMainAppInterface` | `libs/CCPluginAPI/include/ccMainAppInterface.h` | The host callback. `createGLWindow()`, `destroyGLWindow()`, `addToDB()`, `refreshAll()`, `dispToConsole()`, `freezeUI()`, `releaseUI()`, `registerOverlayDialog()`, `getActiveGLWindow()`, `db()`. |
| `ccGLWindowInterface` | `libs/qCC_glWindow/include/ccGLWindowInterface.h` | The viewport API. `setPickingMode()`, `redraw()`, `refresh()`, `setDisplay()`, `addToOwnDB()`, `displayNewMessage()`. |
| `ccGLWindow` | `libs/qCC_glWindow/include/ccGLWindow.h` | The concrete impl (private to `qCC_glWindow`). Returned by `ccGLWindowInterface::Create()`. |
| `ccPickingHub` | `libs/CCPluginAPI/include/ccPickingHub.h` | Picking event router. Singleton-ish (one per `MainWindow`). `addListener()`, `removeListener()`, `togglePickingMode()`. |
| `ccPickingListener` | `libs/CCPluginAPI/include/ccPickingListener.h` | Our base class. `onItemPicked(const PickedItem&)`. |
| `ccOverlayDialog` | `libs/CCPluginAPI/include/ccOverlayDialog.h` | Optional dialog base — provides `linkWith()`, `start()`, `stop()`, `processFinished`. We may or may not inherit this depending on UX choice. |
| `ccHObject` | `libs/qCC_db/include/ccHObject.h` | The scene-graph root. `getDisplay()`, `setGLTransformation()`, `enableGLTransformation()`, `applyGLTransformation_recursive()`. |
| `ccPointCloud` | `libs/qCC_db/include/ccPointCloud.h` | The point cloud entity. `applyRigidTransformation(const ccGLMatrix&)`. |
| `ccGLMatrix` / `ccGLMatrixd` | `libs/qCC_db/include/ccGLMatrix.h` | The transform. |
| `CCVector3` / `CCVector3d` | `libs/qCC_db/extern/CCCoreLib/include/CCConst.h` | 3D points. |
| `CCCoreLib::HornRegistrationTools` | `libs/qCC_db/extern/CCCoreLib/include/RegistrationTools.h` | `FindAbsoluteOrientation()` — the math. |
| `CCCoreLib::RegistrationTools::ComputeRMS` | same header | `double ComputeRMS(GenericCloud*, GenericCloud*, ScaledTransformation)` — overall RMS. |

---

## 1. The happy path

### 1.1 Plugin startup (one-shot at `loadPlugins` time)

```
ccPluginManager::loadPlugins()
   ↓
QPluginLoader loads plugins/core/Standard/qManualRegistration/<lib>.{so,dll,dylib}
   ↓
qManualRegistration::qManualRegistration() → ctor wires m_action
   ↓
m_action->setIcon(...); connect(m_action, &QAction::triggered, this, [this]() { onTriggered(); });
   ↓
[later, when user clicks the action]
```

### 1.2 User selects two clouds

The plugin's `onNewSelection(const ccHObject::Container& selected)` runs whenever the selection changes. We check for **exactly two** `ccPointCloud` (or `ccGenericPointCloud`) entities selected:

```cpp
void qManualRegistration::onNewSelection(const ccHObject::Container& sel) {
    ccPointCloud* a = nullptr; ccPointCloud* b = nullptr;
    for (auto* o : sel) {
        ccPointCloud* pc = ccHObjectCaster::ToPointCloud(o);
        if (pc) { if (!a) a = pc; else if (!b) b = pc; }
    }
    m_action->setEnabled(a && b && a != b);
}
```

### 1.3 User clicks the action → dialog opens

```cpp
void qManualRegistration::onTriggered() {
    // Find the two clouds from selection
    auto sel = m_app->getSelectedEntities();
    ccPointCloud* aligned   = ccHObjectCaster::ToPointCloud(sel[0]);
    ccPointCloud* reference = ccHObjectCaster::ToPointCloud(sel[1]);

    // Allocate the dialog (heaps both viewports via m_app->createGLWindow)
    m_dlg = new qManualRegistrationDlg(m_app, aligned, reference);

    // Wire Apply / Cancel
    connect(m_dlg, &QDialog::accepted, this, &qManualRegistration::onApply);
    connect(m_dlg, &QDialog::rejected, this, &qManualRegistration::onCancel);

    // Show modeless so both viewports stay interactive
    m_dlg->show();
}
```

### 1.4 Dialog ctor — allocate two viewports

```cpp
qManualRegistrationDlg::qManualRegistrationDlg(ccMainAppInterface* app,
                                               ccPointCloud* aligned,
                                               ccPointCloud* reference,
                                               QWidget* parent)
    : QDialog(parent)
    , m_app(app)
    , m_aligned(aligned)
    , m_reference(reference)
{
    setupUi(this);  // builds splitter + pair table + buttons from .ui

    // Create two GL windows
    ccGLWindowInterface* glA = nullptr; QWidget* wA = nullptr;
    ccGLWindowInterface* glB = nullptr; QWidget* wB = nullptr;
    m_app->createGLWindow(glA, wA);
    m_app->createGLWindow(glB, wB);
    m_glA = glA; m_glB = glB;
    m_widgetA = wA; m_widgetB = wB;

    // Embed in the splitter
    m_splitter->addWidget(wA);  // for aligned
    m_splitter->addWidget(wB);  // for reference

    // Show the right cloud in each
    glA->addToOwnDB(aligned);   glA->zoomGlobal(); glA->redraw();
    glB->addToOwnDB(reference); glB->zoomGlobal(); glB->redraw();

    // Register two picking listeners
    ccPickingHub* hub = m_app->pickingHub();   // or grab it via the app; see picking-system.md
    hub->addListener(this, /*exclusive*/false, /*autoStart*/true,
                     ccGLWindowInterface::POINT_PICKING);
    hub->addListener(&m_listenerB, false, true, ccGLWindowInterface::POINT_PICKING);
}
```

> **Note:** one dialog can hold *one* `ccPickingListener`; if we need per-window disambiguation, we either:
> - Use a single listener and check the picked item's `win` pointer against `m_glA` / `m_glB`, or
> - Create a small adapter class per window that forwards to the dialog with a window tag.
>
> The first option is cleaner — see [`picking-system.md`](picking-system.md).

### 1.5 User picks points

User clicks in viewport A → `onItemPicked(PickedItem{point: P, entity: aligned, win: glA})`.
User clicks in viewport B → same with `win: glB`.

```cpp
void qManualRegistrationDlg::onItemPicked(const PickedItem& pi) {
    if (pi.entity != m_aligned && pi.entity != m_reference) return;  // wrong cloud

    if (pi.win == m_glA) {
        m_alignedPoints.push_back(pi.P3D);
        m_table->insertRow({ QString::number(m_alignedPoints.size()), "A",
                              QString::number(pi.P3D.x), ... });
    } else if (pi.win == m_glB) {
        m_referencePoints.push_back(pi.P3D);
        m_table->insertRow({ QString::number(m_referencePoints.size()), "B", ... });
    }

    m_previewButton->setEnabled(m_alignedPoints.size() >= 3
                              && m_referencePoints.size() >= 3
                              && m_alignedPoints.size() == m_referencePoints.size());
}
```

### 1.6 User clicks Preview (≥3 pairs)

```cpp
void qManualRegistrationDlg::onPreviewClicked() {
    if (m_alignedPoints.size() != m_referencePoints.size()) return;

    // Build two GenericClouds for the math
    CCCoreLib::PointCloud alignedCloud;
    for (auto& p : m_alignedPoints)   alignedCloud.addPoint(CCVector3(p.x, p.y, p.z));
    CCCoreLib::PointCloud referenceCloud;
    for (auto& p : m_referencePoints) referenceCloud.addPoint(CCVector3(p.x, p.y, p.z));

    // Compute rigid transform (Umeyama or Horn; see transform-math.md)
    CCCoreLib::PointProjectionTools::Transformation trans;
    if (!UmeyamaRegistration(&alignedCloud, &referenceCloud, trans, /*fixedScale*/true)) {
        m_app->dispToConsole("Failed to compute transform", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
        return;
    }

    // Convert to ccGLMatrix for the preview
    ccGLMatrix glTrans = ccGLMatrix::FromDoublePrecision(trans.R, trans.T);

    // Display-only transform on the source cloud
    m_aligned->setGLTransformation(glTrans);
    m_aligned->enableGLTransformation(true);
    m_glA->redraw();

    // Compute per-pair + overall RMS
    double rms = CCCoreLib::RegistrationTools::ComputeRMS(&alignedCloud, &referenceCloud, trans);
    m_overallRmsLabel->setText(QString("Overall RMS: %1").arg(rms));
    // per-pair: for each pair i, ||R*P_i + T - Q_i||
}
```

### 1.7 User clicks Reset (preview off)

```cpp
void qManualRegistrationDlg::onResetClicked() {
    m_aligned->resetGLTransformation();
    m_aligned->enableGLTransformation(false);
    m_glA->redraw();
}
```

### 1.8 User clicks Apply (commit)

```cpp
void qManualRegistrationDlg::onApplyClicked() {
    // Reuse the same Umeyama computation (already cached from Preview)
    ccGLMatrix glTrans = /* cached */;

    // Commit (mutates the point cloud's vertices)
    m_aligned->applyRigidTransformation(glTrans);

    // Clear the preview-only transform
    m_aligned->resetGLTransformation();
    m_aligned->enableGLTransformation(false);

    // Notify the db-tree and re-zoom
    m_app->addToDB(m_aligned, /*updateZoom*/true, /*autoExpand*/false, /*checkDimensions*/false);
    m_app->refreshAll();

    // Close
    accept();
}
```

### 1.9 Dialog closes (Cancel or Apply)

```cpp
qManualRegistrationDlg::~qManualRegistrationDlg() {
    if (m_pickingHub && m_listenerAdded) {
        m_pickingHub->removeListener(this);
    }
    if (m_glA) { m_glA->setParent(nullptr); m_app->destroyGLWindow(m_glA); }
    if (m_glB) { m_glB->setParent(nullptr); m_app->destroyGLWindow(m_glB); }
}
```

---

## 2. Failure paths & edge cases

### 2.1 User selects only one cloud (or zero)

`onNewSelection` does not enable `m_action`. The action stays disabled in the menu.

### 2.2 User cancels during Preview

`reject()` is called. The dialog's destructor unregisters the picking listener and destroys the two GL windows. The source cloud's `setGLTransformation` was display-only, so closing the dialog leaves the db-tree untouched. ✓

### 2.3 User picks in the wrong viewport (clicks in A when adding a "B" pair)

We **don't** care — the user picks freely in either viewport; the rows are auto-paired by insertion order. If they pick out of order, they get bad pairs; they should remove + re-pick. (This matches `ccPointPairRegistrationDlg`'s behaviour.)

### 2.4 Source and reference clouds have very different scales (mm vs m)

Apply `ccGlobalShiftManager` rules during commit (we don't worry about this for v1 — it's a polish item). The Umeyama math doesn't care about absolute scale (we pass `fixedScale=true`).

### 2.5 Source cloud has locked vertices (`lockedVertices`)

Skip the cloud in `onNewSelection` (warn the user). The single-window `ccPointPairRegistrationDlg` doesn't handle this case either.

### 2.6 Transform computation fails (degenerate pairs: 3 collinear points)

`UmeyamaRegistration` returns false. Show a QMessageBox with "Pairs are degenerate — pick better correspondences." Leave the dialog open; don't apply.

### 2.7 User picks the same point twice

`UmeyamaRegistration` will return a near-identity matrix. Show "RMS = 0 — pick distinct points." The user removes duplicates and re-picks.

### 2.8 Cloud is closed before dialog closes

The db-tree may delete the cloud underneath us. We hold `m_aligned` / `m_reference` as raw pointers — **risk**. v1.5: switch to `ccHObjectContext` from `ccMainAppInterface::removeObjectTemporarilyFromDBTree(...)`.

---

## 3. State machine of the dialog

```
[constructed]
   ↓ setupUi; createGLWindow ×2; addListener ×2
[Ready]   ── user picks → pairs grow
   ↓ user clicks Preview (≥3 pairs)
[PreviewShown]   ── live transform on source
   ↓ user clicks Reset → back to [Ready]
   ↓ user clicks Apply → [Committing] → [Closing]
   ↓ user clicks Cancel → [Closing]
[Closing]
   ↓ removeListener ×2; destroyGLWindow ×2; ~qManualRegistrationDlg()
[~qManualRegistrationDlg]
```

## 4. Threading / events

- **UI thread does everything.** CloudCompare has no worker-thread convention for plugins (the closest is `QFuture` / `QtConcurrent::run`, which neither `ccPointPairRegistrationDlg` nor the existing ICP dialog uses).
- **`UmeyamaRegistration`** on a 100-pair input is microseconds. No need for a worker.
- **`computeCloudCloudDistance` / `ComputeRMS`** is O(n) in point count per pair. For v1 with ≤100 pairs, this is fine. For v2 with >10k pairs, move to `QtConcurrent::run` + a progress dialog (`ccProgressDialog`).

## 5. Undo (out of v1)

`ccPointPairRegistrationDlg` doesn't have an undo hook. v1 of the dual-screen plugin doesn't either. Documenting the path for v2:

- Capture `ccGLMatrix` pre-transform.
- After commit, push to a `ccCommandLine`-style command with `undo()` and `redo()` slots.
- Or wrap the transform in a `ccHObject` group whose `applyGLTransformation_recursive` can be inverted.

None of this is in v1 scope. See [`../../AGENTS_REGISTRATION.md`](../../../AGENTS_REGISTRATION.md) §6.

## 6. CLI mode (M6 — optional but recommended)

A CLI command like `CloudCompare -MANUAL_REGISTER -A <file> -B <file> -PA x,y,z -PB x,y,z ...` would let the user script registrations. Plumbing:

- Override `registerCommands(ccCommandLineInterface* cmd)` in `qManualRegistration.cpp`.
- Add a `ccCommandLineInterface::Command::Shared(new ManualRegistrationCommand)`.
- The command opens the dialog headlessly (no viewports), runs the math, commits.
- See `plugins/core/Standard/qCompass` for the pattern.

## 7. Pointers

- [`picking-system.md`](picking-system.md)
- [`dual-viewport.md`](dual-viewport.md)
- [`transform-math.md`](transform-math.md)
- [`../../AGENTS_REGISTRATION.md`](../../../AGENTS_REGISTRATION.md)
- Single-window reference: `qCC/ccPointPairRegistrationDlg.{h,cpp}`
- Plugin API: `libs/CCPluginAPI/include/ccMainAppInterface.h`
- Picking API: `libs/CCPluginAPI/include/ccPickingHub.h`, `ccPickingListener.h`
- Transform API: `libs/qCC_db/include/ccHObject.h`, `ccPointCloud.h`, `ccDrawableObject.h`
- Math: `libs/qCC_db/extern/CCCoreLib/include/RegistrationTools.h`
