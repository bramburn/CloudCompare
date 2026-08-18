# docs/context/registration/dual-viewport.md

How a plugin **owns** two `ccGLWindow`s without touching `qCC/mainwindow.cpp`. This is the cleanest path; the alternative is to borrow the MDI area, which we explicitly avoid for v1.

---

## 1. The plugin-API contract

From `libs/CCPluginAPI/include/ccMainAppInterface.h` (lines 80-99):

```cpp
//! Creates a new instance of GL window (with its encapsulating widget)
/** \warning This instance must be destroyed by the application as well (see destroyGLWindow)
    Note that the encapsulating widget is the window instance itself if 'stereo' mode is disabled
**/
virtual void createGLWindow(ccGLWindowInterface*& window, QWidget*& widget) const
{
    window = nullptr;
    widget = nullptr;
}

//! Destroys an instance of GL window created by createGLWindow
virtual void destroyGLWindow(ccGLWindowInterface*) const
{
}
```

The default implementation does **nothing**. The override in `qCC/mainwindow.cpp:12113` is the real provider:

```cpp
void MainWindow::createGLWindow(ccGLWindowInterface*& window, QWidget*& widget) const
{
    bool stereoMode = ccGLWindowInterface::TestStereoSupport();
    ccGLWindowInterface::Create(window, widget, stereoMode);
    assert(window && widget);
}

void MainWindow::destroyGLWindow(ccGLWindowInterface* view3D) const
{
    if (view3D) {
        if (QWidget* widget = dynamic_cast<QWidget*>(view3D)) {
            widget->setParent(nullptr);
        } else {
            view3D->asQObject()->setParent(nullptr);
        }
        delete view3D;
    }
}
```

**The plugin contract is:** the host (`MainWindow`) creates and destroys the windows on our behalf; we just hold the pointer and don't `delete` it.

> **CLI mode:** in headless mode `MainWindow` is never instantiated, so the default `createGLWindow` is used → returns null → the plugin must check for null and gracefully refuse. v1 is GUI-only; this is documented but not a hard requirement.

---

## 2. The lifecycle

### 2.1 Open

```cpp
// dialog ctor
ccGLWindowInterface* glA = nullptr; QWidget* wA = nullptr;
ccGLWindowInterface* glB = nullptr; QWidget* wB = nullptr;

m_app->createGLWindow(glA, wA);   // may return null in CLI / tests
m_app->createGLWindow(glB, wB);

if (!glA || !glB) {
    // graceful fail — disable the dialog or show an error
    QMessageBox::critical(this, "Manual Registration", "3D viewports are unavailable in this mode.");
    return;
}

m_glA = glA; m_glB = glB;
m_widgetA = wA; m_widgetB = wB;

// Embed in the dialog's layout (QSplitter recommended for resizing)
m_splitter->addWidget(wA);   // aligned on the left
m_splitter->addWidget(wB);   // reference on the right

// Wire the cameras: each viewport shows one of the clouds
glA->addToOwnDB(m_aligned);
glA->zoomGlobal();
glA->redraw();

glB->addToOwnDB(m_reference);
glB->zoomGlobal();
glB->redraw();
```

### 2.2 Use

- The user interacts with each viewport independently (or with cross-window sync — out of v1).
- `ccGLWindowInterface::redraw()` is cheap when nothing has changed.
- Each viewport owns its own camera, picking mode, render state. No sharing unless we explicitly opt in (see §4).

### 2.3 Close

```cpp
// dialog dtor
if (m_glA) {
    m_glA->setPickingMode(ccGLWindowInterface::NO_PICKING);  // disable picking first
    m_app->destroyGLWindow(m_glA);  // host will delete; we don't
    m_glA = nullptr;
}
// same for m_glB
```

### 2.4 Failure to destroy

If you `delete` the window yourself instead of calling `destroyGLWindow`, you bypass the host's ownership conventions. On Windows, this can crash on shutdown when `MainWindow` tries to clean up. Don't do it.

---

## 3. The single biggest gotcha — `m_app->db()` vs. `m_aligned`

`ccPointPairRegistrationDlg` uses **`m_associatedWin->addToOwnDB(label)`** for the picked-point markers (it draws `cc2DLabel`s at each picked point). That's a **per-window** db-tree, not the app's main db-tree. The label belongs to the GL window, not to the cloud.

Two choices for the dual-viewport plugin:

### Option A — copy the existing pattern

Draw `cc2DLabel`s at each picked point in the corresponding viewport. They show up in the window's "own db" tree (a per-window side tree, not the app's main db). User sees markers, doesn't pollute the main tree. ✓

```cpp
// when user picks in viewport A
CCVector3d picked = pi.P3D;
m_alignedPoints.push_back(picked);

cc2DLabel* label = new cc2DLabel();
label->setPickedPos(picked);
label->setName(QString("A%1").arg(m_alignedPoints.size()));
m_glA->addToOwnDB(label);  // shown in viewport A only
m_glA->redraw();
```

### Option B — keep points in a `std::vector<CCVector3d>`

Don't draw anything in the viewport until Preview. Show the points in the pair table only. Cheaper, less visual feedback during picking.

**Recommendation:** **Option A.** It matches the existing dialog and is what users expect.

---

## 4. Cross-window coordination (out of v1, but worth knowing)

The user wants "synchronized camera views" in the future. CloudCompare's `ccGLWindowInterface` has no built-in camera-sync. If we want it, we have two choices:

### 4.1 Mirror cameras by hand

Each viewport's camera state lives in `ccViewportParameters` (on `ccGLWindowInterface::getViewportParameters()`). We can copy A's viewport params into B every time A's camera changes:

```cpp
// connect m_glA->signalCameraChanged to a slot in the dialog
connect(m_glA, &ccGLWindowInterface::baseViewMatChanged,
        this, [this]() {
            ccViewportParameters params = m_glA->getViewportParameters();
            m_glB->setViewportParameters(params);
            m_glB->redraw();
        });
```

Caveat: this also copies the *pivot*, which the user may not want synced.

### 4.2 Link cameras via `ccViewportParameters`

Some plugins and the MDI's "tile" view share a viewport. Look at `qCC/mainwindow.cpp` for any existing examples. (None of the shipped code shares cameras between MDI windows.)

**Out of v1.** Don't sync cameras in v1 — let each viewport be independent.

---

## 5. Layout — QSplitter vs. dock widgets vs. QStackedWidget

| Option | Pros | Cons |
|---|---|---|
| `QSplitter` (horizontal) inside the dialog | Simple, resizable, both visible at once | Dialog grows tall; no full-screen mode |
| `QMainWindow` with two `QDockWidget`s | Full-screen each, dockable | More boilerplate; not idiomatic for a modal-feeling dialog |
| `QStackedWidget` with a "swap" button | Compact | User has to flip back and forth to pick in the other cloud — bad UX |

**Recommendation:** `QSplitter` for v1. Add a "Fullscreen A / Fullscreen B / Split" toggle as a v2 polish.

---

## 6. The "Show Fullscreen" button

Use `m_splitter->setSizes({INT_MAX, INT_MAX})` to give all space to one side, or `m_widgetA->setParent(nullptr); m_widgetA->show()` to detach. For v1, skip this.

---

## 7. Pointers

- [`picking-system.md`](picking-system.md)
- [`transform-math.md`](transform-math.md)
- [`../../AGENTS_REGISTRATION.md`](../../../AGENTS_REGISTRATION.md)
- Source:
  - `libs/CCPluginAPI/include/ccMainAppInterface.h` lines 80-99
  - `qCC/mainwindow.cpp` line 12113 (`createGLWindow` impl), 12130 (`destroyGLWindow` impl)
  - `qCC/ccPointPairRegistrationDlg.cpp` line 231 onwards (`addToOwnDB`, `displayNewMessage` pattern)
  - `libs/qCC_db/include/cc2DLabel.h` (the picked-point marker type)
