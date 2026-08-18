# docs/context/registration/picking-system.md

How CloudCompare's picking system works, what we can use as-is, and how to bridge it across two plugin-owned `ccGLWindow`s.

---

## 1. The three pieces

| Piece | Header | What it does |
|---|---|---|
| `ccPickingHub` | `libs/CCPluginAPI/include/ccPickingHub.h` | The router. Owned by `MainWindow` (one per app). Plugins add listeners to it. |
| `ccPickingListener` | `libs/CCPluginAPI/include/ccPickingListener.h` | The receiver. Has a single virtual `onItemPicked(const PickedItem&)`. |
| `ccGLWindowInterface::PICKING_MODE` | `libs/qCC_glWindow/include/ccGLWindowInterface.h` line 87 | The mode the hub puts the active window into. We want `POINT_PICKING`. |

The full lifecycle in the existing single-window dialog (`ccPointPairRegistrationDlg`):

```cpp
// ctor
m_pickingHub->addListener(this, /*exclusive*/true, /*autoStart*/true,
                           ccGLWindowInterface::POINT_PICKING);

// destructor / reject
m_pickingHub->removeListener(this);
```

When the user clicks in the GL window that's "active" (the MDI's current sub-window), the hub routes the pick to whichever listener is registered for the active picking mode.

---

## 2. The `PickedItem` struct

```cpp
struct PickedItem
{
    ccHObject* entity;        // the entity under the cursor (ccPointCloud, mesh, label, …)
    int subEntityID;          // triangle index / point index (for meshes/clouds)
    CCVector3d P3D;           // the 3D position (the user's pick)
    ccGLWindowInterface* win; // <-- THE viewport the pick happened in
};
```

`win` is the **disambiguator** we need for dual-viewport. Two `ccGLWindow`s reporting picks to one dialog → check `pi.win == m_glA` vs `pi.win == m_glB`.

---

## 3. Why one listener is enough (and why two are wrong)

CloudCompare's `ccPickingHub` was designed for **one active listener at a time**. You can register multiple, but the hub only routes picks from the currently-active GL window. If both windows are active simultaneously (which they are in a QSplitter), the hub needs careful handling.

**Two approaches:**

### Approach A — single listener, window-disambiguate

Inherit `ccPickingListener` once on the dialog. Register the dialog with the hub once. In `onItemPicked`, branch on `pi.win`:

```cpp
void qManualRegistrationDlg::onItemPicked(const PickedItem& pi) {
    if (!m_glA || !m_glB) return;
    if (pi.win != m_glA && pi.win != m_glB) return;  // user clicked somewhere else
    if (pi.entity != m_aligned && pi.entity != m_reference) return;  // not our clouds

    if (pi.win == m_glA) {
        // user clicked in the source viewport
        m_alignedPoints.push_back(pi.P3D);
        ...
    } else {
        // user clicked in the reference viewport
        m_referencePoints.push_back(pi.P3D);
        ...
    }
    redrawPairTable();
}
```

**Pros:** one registration, one callback, simple.
**Cons:** relies on `PickedItem::win` being reliably populated by the hub (it is, per the `ccPickingListener.h` header).

### Approach B — two adapter listeners

Create two tiny `ccPickingListener` subclasses (anonymous in the dialog's `.cpp`), one per window. Each forwards to the dialog with a window tag:

```cpp
struct AdapterListenerA : ccPickingListener {
    qManualRegistrationDlg* dlg;
    explicit AdapterListenerA(qManualRegistrationDlg* d) : dlg(d) {}
    void onItemPicked(const PickedItem& pi) override {
        dlg->onPickFromA(pi);
    }
};
// and AdapterListenerB
```

**Pros:** type-safe routing.
**Cons:** twice the boilerplate; the hub still treats them as two listeners and may try to set picking mode twice. Don't do this.

**Recommendation:** **Approach A.**

---

## 4. The hub API we use

`ccPickingHub::addListener` signature (from `ccPickingHub.h`):

```cpp
bool addListener(ccPickingListener*                listener,
                 bool                              exclusive        = false,
                 bool                              autoStartPicking = true,
                 ccGLWindowInterface::PICKING_MODE mode             = ccGLWindowInterface::POINT_OR_TRIANGLE_PICKING);
```

`exclusive = true` blocks other listeners from registering. We want `false` — other plugins may also be listening.

`autoStartPicking = true` flips the active window into picking mode automatically. We want `true` — but note this only affects the **active MDI window**, not our plugin-owned windows. We may need to call `m_glA->setPickingMode(POINT_PICKING)` and `m_glB->setPickingMode(POINT_PICKING)` ourselves, manually, on both windows.

`mode = POINT_PICKING` is the right mode.

`removeListener(listener)` undoes everything. Call it in the dialog destructor.

---

## 5. The plugin API doesn't expose the hub directly

⚠ **Verification needed before implementation:** `ccMainAppInterface` does **not** have a `pickingHub()` accessor as of the current source. `ccPointPairRegistrationDlg` gets the hub from `MainWindow` directly: `m_pprDlg = new ccPointPairRegistrationDlg(m_pickingHub, this, this)` (see `qCC/mainwindow.cpp:6988`).

Two options:

### Option A — add `ccMainAppInterface::pickingHub()` to the API

```cpp
// in CCPluginAPI/include/ccMainAppInterface.h
virtual ccPickingHub* getPickingHub() = 0;
```

Implementation in `qCC/mainwindow.h`: `ccPickingHub* getPickingHub() override { return m_pickingHub; }`.

This is a **non-breaking** change (pure addition). Bumps the interface's effective contract but not the `Q_DECLARE_INTERFACE` version (it's still the same set of methods, just one more).

### Option B — pass the hub into the plugin via the `setMainAppInterface` callback

The plugin's `onTriggered` can ask the main window for the hub via a callback registered through `ccMainAppInterface`. Same plumbing, just routed differently.

**Recommendation:** Option A — add `getPickingHub()` to `ccMainAppInterface`. The change is small and obviously correct. Document it in `AGENTS-plugin-dev.md` §6 (rare core-API change) and add the line to `qCC/mainwindow.h` + `.cpp`. This is one of the **two** non-plugin-only changes for v1 (the other being nothing — everything else fits in the plugin).

> **Note:** see [`../../AGENTS_REGISTRATION.md`](../../../AGENTS_REGISTRATION.md) §3 ("Architecture constraints") — this is a justified exception to "don't touch `qCC/`" because the alternative is to put GL window + picking logic somewhere it doesn't belong.

---

## 6. Subclassing `ccPickingListener` from a plugin

Two practical issues:

1. **Multiple inheritance.** `ccPickingListener` is a QObject subclass (it has signals). The dialog is also a QDialog. Both inherit QObject. Use:

   ```cpp
   class qManualRegistrationDlg : public QDialog, public ccPickingListener {
       Q_OBJECT  // for the QDialog side
       ...
   };
   ```

   This is fine because `ccPickingListener` is also QObject (so MI is OK as long as only one base is QObject-derived-with-signals — but here both are, so we have to be careful with `Q_OBJECT`'s placement). The pattern from `ccPointPairRegistrationDlg` works:

   ```cpp
   class ccPointPairRegistrationDlg : public ccOverlayDialog, public ccPickingListener, public Ui::pointPairRegistrationDlg {
       Q_OBJECT
       ...
   };
   ```

   `Q_OBJECT` macros are processed correctly under MI as long as the macro is in the most-derived class. ✓

2. **The hub's owner.** The hub is owned by `MainWindow`, not by us. We don't `new` it, we don't `delete` it. We just register and unregister our listener.

---

## 7. Picking on a non-active window — manual mode

Because our two GL windows are *not* in the MDI area (they're children of our dialog), the hub's "active window" tracking doesn't apply to them. We have to put each into picking mode ourselves:

```cpp
// after createGLWindow ×2
m_glA->setPickingMode(ccGLWindowInterface::POINT_PICKING, Qt::CrossCursor);
m_glB->setPickingMode(ccGLWindowInterface::POINT_PICKING, Qt::CrossCursor);
```

And on close:

```cpp
m_glA->setPickingMode(ccGLWindowInterface::NO_PICKING);
m_glB->setPickingMode(ccGLWindowInterface::NO_PICKING);
```

The hub will still receive picks via the `ccGLWindowInterface::processEvents` pipeline (the hub hooks into each window's mouse-press event) and route them to our listener.

---

## 8. Pointers

- [`dual-viewport.md`](dual-viewport.md) — how to create and own two GL windows
- [`transform-math.md`](transform-math.md) — what we do with the picked points
- [`../../AGENTS_REGISTRATION.md`](../../../AGENTS_REGISTRATION.md) — the goal-level doc
- Source of truth:
  - `libs/CCPluginAPI/include/ccPickingHub.h` (90 lines)
  - `libs/CCPluginAPI/include/ccPickingListener.h` (the `PickedItem` struct)
  - `libs/qCC_glWindow/include/ccGLWindowInterface.h` line 87 (`PICKING_MODE` enum), line 430 (`setPickingMode`)
  - `qCC/ccPointPairRegistrationDlg.{h,cpp}` (the working single-window pattern)
  - `qCC/mainwindow.cpp:6988` (how the existing dialog gets the hub)
