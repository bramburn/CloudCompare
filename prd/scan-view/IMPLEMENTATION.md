# Implementation Plan — qScanBrowser

**Reference:** [`PRD.md`](PRD.md) for full feature definitions and architecture.

---

## Phase 0 — Infrastructure & SDK Wrapper

**Goal:** Verify ReCap structured scan API works, establish rendering scaffolding.

### T0.1 — New plugin scaffold

```
plugins/core/Standard/qScanBrowser/
├── CMakeLists.txt
├── qScanBrowser.json
├── qScanBrowser.qrc
├── include/
│   ├── CMakeLists.txt
│   ├── qScanBrowser.h           ← ccPlugin subclass
│   ├── ScanBrowserDialog.h       ← ccOverlayDialog subclass
│   └── ScanStation.h             ← data: origin + transform + structured scan
└── src/
    ├── CMakeLists.txt
    ├── qScanBrowser.cpp
    ├── ScanBrowserDialog.cpp
    └── ScanStation.cpp
```

**Task:** Copy `plugins/example/ExamplePlugin` → `qScanBrowser/`. Adapt to Standard plugin type.
Change `registerCommands()` to add a context-menu item on `ccPointCloud` entities: **"Open Spherical View"**.

Wire `PLUGIN_STANDARD_QSCANBROWSER=ON` into `cc-configure.cmd`.

### T0.2 — ScanStation data class

```cpp
struct ScanStation {
    RCString name;
    RCVector3d origin;           // scanner world position
    RCTransform fullTransform;    // full transform to survey CS
    int azimuthStart, azimuthEnd, elevationStart, elevationEnd; // in radians
    int width, height;          // structured scan image dimensions
    bool isStructured;           // true if has structured scan
};
```

**Task:** Create `ScanStation.h/cpp`. Implement factory:
```cpp
QVector<ScanStation> LoadScanStationsFromRCP(const QString& rcpPath);
```
Iterates `RCScan` objects in the RCP project, extracts origin, full transform, structured scan dimensions.

### T0.3 — Verify structured scan load

Write a test that:
1. Opens one RCS file
2. Gets `getStructuredScan()`
3. Calls `structuredScan->getSphericalModel()->getWidth()`, `getHeight()`
4. Iterates a 100×100 grid, calls `getColor(pixel)`, `getRange(pixel)`, `isBadPoint(pixel)`
5. Logs first 10 non-bad points with colour

**Acceptance:** You can read structured scan data from a real RCS file. Verify this before any UI work.

### T0.4 — Spherical colour image loader

```cpp
class SphericalColourImage {
    // Loads all pixel colours from structured scan into a QImage
    // Dimensions: structuredScan->getSphericalModel()->getWidth/Height()
    QImage colourImage;

    QRgb colourAt(int x, int y) const;
    bool isValidPixel(int x, int y) const;  // !isBadPoint(x,y)
};
```

**Task:** Write `SphericalColourImage.h/cpp`. On construction:
1. Load structured scan
2. Allocate `QImage(width, height, QImage::Format_RGB888)`
3. Iterate all pixels → `setPixel(x,y, colourAt(x,y))`
4. Cache the image; reuse on redraw

**Performance target:** <2s for a 4000×2000 image on SSD.

### T0.5 — Bubble view integration

Review `ccGLWindowInterface::setBubbleViewMode(true)` and `setBubbleViewFov(float)`.
Confirm that bubble view:
- Camera stays fixed at a position (scanner origin)
- Mouse drag rotates view direction (azimuth/elevation)
- Scroll changes FOV

**Task:** Write a minimal test dialog that calls `win->setBubbleViewMode(true)` and `win->setBubbleViewFov(90.0f)` when it opens, and restores the prior state on close. Verify with an existing `.e57` or `.las` point cloud in CC.

---

## Phase 1 — Spherical Colour Panorama View

**Goal:** User opens a scan → sees a 360° colour panorama → can navigate to other stations.

### T1.1 — Dialog shell

```cpp
class ScanBrowserDialog : public ccOverlayDialog {
    Q_OBJECT
public:
    ScanBrowserDialog(ccMainAppInterface* app, QWidget* parent = nullptr);
    void openStation(const ScanStation& station);
    void openProject(const QString& rcpPath);
private:
    ccGLWindowInterface* m_glWindow;   // CC's main viewport
    ScanStation m_currentStation;
    QVector<ScanStation> m_stations;
    SphericalColourImage* m_colourImage = nullptr;
    bool m_bubbleViewWasEnabled = false;
};
```

**Lifecycle:**
- `open()` → save bubble view state → enable bubble view
- `close()` → restore bubble view state → release colour image
- No secondary window; renders directly into CC's GL viewport

### T1.2 — GL sphere background renderer

Render the colour panorama as a textured sphere (the viewer is at the centre):

```cpp
// In ScanBrowserDialog or a helper class:
GLuint m_sphereVAO;
QOpenGLTexture* m_panoramaTexture;  // QImage → GL texture

void renderPanoramaSphere() {
    // Unit sphere, viewer at origin
    // Texture coordinates: equirectangular UV mapped to sphere
    // Use gluSphere or manual VBO with normals = positions
    m_panoramaTexture->bind();
    // Draw sphere with sphere texture
    m_panoramaTexture->release();
}
```

**Sphere geometry approach:** Pre-compute a unit UV sphere in VBO once:
- Vertices: `sin(phi)*cos(theta), sin(phi)*sin(theta), cos(phi)` for phi/theta in regular grid
- UVs: `u = theta/(2*PI)+0.5`, `v = phi/PI` (standard equirectangular mapping)
- Normals = positions (for lighting if added later)

**Integration:** Override `ScanBrowserDialog::drawFrame()` — called by `ccOverlayDialog` during the GL render loop. Draw the sphere before other scene geometry.

### T1.3 — Drag-to-rotate navigation

Use `ccGLWindowInterface::bubbleViewMode` for rotation:
- Horizontal drag → azimuth change → `bubbleViewFov` not used for rotation; bubble view rotation is handled internally in `ccGLWindowInterface` (see `BubbleViewMode` mouse handling at `ccGLWindowInterface.cpp:~6556`)
- Vertical drag → elevation change

**Alternative (custom rotation):** If bubble view rotation needs to be controlled from outside:
```cpp
// Track last mouse position
void mousePressEvent(QMouseEvent* event) override;
void mouseMoveEvent(QMouseEvent* event) override;

// On drag:
// azimuth += deltaX * rotationSpeed
// elevation = clamp(elevation + deltaY * rotationSpeed, -PI/2, PI/2)
// Apply as rotation to viewMat using:
//   viewMat = rotationZ(azimuth) * rotationX(elevation) * originalViewMat
```

**Decision point (TBD):** Use bubble view's built-in drag rotation OR implement custom. Test first with T0.5.

### T1.4 — Station navigation UI

Add to `ScanBrowserDialog`:
- **Toolbar / sidebar:** List of scan stations (names from `RCScan::getName()`)
- **Prev/Next arrows:** Jump to adjacent stations
- **Station thumbnails:** `QListWidget` with station names; click → `openStation(station)`
- **Close button:** Returns to normal CC viewport

**Task:** Design the UI layout (see SCOPING.md). Recommend: dialog panel on left (300px) + GL viewport fills remaining space.

### T1.5 — Context menu integration

In `qScanBrowser::registerCommands()`:
```cpp
// Add "Open Spherical View" to ccPointCloud right-click menu
// Requires: ccPluginInterface::ccStdPluginInterface or similar
// Alternative: add command to File → Open menu
app->registerCommand(new OpenScanBrowserCommand(app));
```

**Task:** Find how other Standard plugins add context-menu items. Check `qPCL`, `qAnimation` for examples. If no context menu API exists, add to File menu as "Open Spherical View…".

### T1.6 — RCP project loader

```cpp
void ScanBrowserDialog::openProject(const QString& rcpPath) {
    // Load RCP via RCProjectImporter
    // For each RCScan:
    //   - Get name, origin, full transform
    //   - Check isStructured()
    //   - Store ScanStation
    // Populate station list UI
    // Open first station by default
}
```

---

## Phase 2 — Depth Buffer & Measurements

**Goal:** Measure points in the spherical view using the range image.

### T2.1 — Range image data class

```cpp
class SphericalRangeImage {
    // Parallel to SphericalColourImage
    // stores: range[height][width] as float (millimetres)
    //         isBad[height][width] as bool

    int width, height;
    std::vector<float> ranges;       // size = width * height
    std::vector<bool> badPixels;     // size = width * height

    float getRange(int x, int y) const;
    bool isBad(int x, y) const;
    CCVector3d worldPoint(int x, int y) const;  // origin + direction * range
};
```

**Task:** Load all range values from `getRange(pixelIndex)` into this structure.
**Memory:** For 4000×2000, this is 32M floats = 128 MB. Acceptable for one station.

### T2.2 — Pixel-to-3D conversion

```cpp
CCVector3d SphericalRangeImage::pixelToWorld(int x, int y, const CCVector3d& origin, double azimuth, double elevation) {
    // direction = (sin(elevation)*cos(azimuth), sin(elevation)*sin(azimuth), cos(elevation))
    // return origin + direction * getRange(x,y)
}
```

Use `RCSphericalModel::polarFromImage(PixelIndex)` to get `(azimuth, elevation)` from pixel coordinates.

### T2.3 — Click-to-measure (2-point distance)

```cpp
void ScanBrowserDialog::mousePressEvent(QMouseEvent* event) {
    if (m_glWindow && event->button() == Qt::LeftButton) {
        // Convert screen click to azimuth/elevation
        // Map to pixel (x, y) in the range image
        // Get range at that pixel
        // Store as measurement start or end point
        // If two points: compute distance → show in overlay label
    }
}
```

**Measurement display:** Show distance in a floating `QLabel` anchored to the viewport, e.g.:
```
Point A → Point B: 3.452 m
```

### T2.4 — Polyline measurement

Extend T2.3: allow multiple clicks to create a chain measurement. Right-click to finish the chain.

### T2.5 — Clipping controls

Add a clipping sphere widget:
- Slider for clipping radius (0 → max range)
- Applied as: only show points where `range <= clipRadius`
- Renders as a separate GL sphere at scanner origin with radius = clipRadius

---

## Phase 3 — Correspondence View & Polish

**Goal:** Two synchronized viewports + station thumbnails + export.

### T3.1 — Correspondence view (two synchronized viewports)

```cpp
// ScanBrowserDialog owns two ccGLWindowInterface*:
//   m_mainWindow: full spherical view (this is already the CC viewport)
//   m_overviewWindow: the standard CC 3D project view (no bubble view)

// Synchronization:
// When user rotates spherical view (drags):
//   → update azimuth/elevation state
//   → optionally update a marker in the 3D overview showing camera position

// When user clicks a station in the 3D overview:
//   → openStation(thatStation) in the spherical view
```

**Approach:** Keep `m_glWindow` as the main spherical view. The existing CC 3D viewport IS the overview. No second window needed at first — implement "camera position marker" in the existing 3D view as a 3D frustum glyph drawn at the current scanner origin.

### T3.2 — Station thumbnail generation

For each station, render a small preview:
1. Use `renderToImage(zoomFactor)` from `ccGLWindowInterface` (or offscreen FBO) after opening a station
2. Save as `QImage`; use in the station list as `QListWidgetItem` with icon

### T3.3 — Annotation markers

```cpp
struct AnnotationMarker {
    CCVector3d position;   // world coordinates
    QString label;
    QColor color;
};
QVector<AnnotationMarker> m_annotations;
```

Render as coloured spheres in the spherical view. Click on an annotation → show label in popup.

### T3.4 — Export spherical image

```cpp
void ScanBrowserDialog::exportPanorama(const QString& path) {
    // Export the equirectangular colour image as PNG
    m_colourImage->save(path);
}
```

Add "Export Panorama…" button to toolbar.

---

## Atomic Task Index

| Task | File(s) | Effort | Dependency |
|---|---|---|---|
| T0.1 Plugin scaffold | `qScanBrowser/*` | 2h | None |
| T0.2 ScanStation class | `ScanStation.h/.cpp` | 1h | None |
| T0.3 Structured scan load test | test script | 2h | T0.2 |
| T0.4 SphericalColourImage | `SphericalColourImage.h/.cpp` | 3h | T0.3 |
| T0.5 Bubble view test | `ScanBrowserDialog.cpp` | 1h | T0.1 |
| T1.1 Dialog shell | `ScanBrowserDialog.h/.cpp` | 2h | T0.1 |
| T1.2 Sphere renderer | `SphereRenderer.h/.cpp` | 4h | T0.4, T0.5 |
| T1.3 Drag navigation | `ScanBrowserDialog.cpp` | 3h | T1.1, T1.2 |
| T1.4 Station UI | `ScanBrowserDialog.cpp` | 4h | T1.1 |
| T1.5 Context menu | `qScanBrowser.cpp` | 2h | T0.1 |
| T1.6 RCP loader | `ScanStation.cpp` | 4h | T0.2 |
| T2.1 Range image class | `SphericalRangeImage.h/.cpp` | 3h | T0.3 |
| T2.2 Pixel→3D conversion | `SphericalRangeImage.cpp` | 2h | T2.1 |
| T2.3 2-point measurement | `ScanBrowserDialog.cpp` | 3h | T2.2 |
| T2.4 Polyline measurement | `ScanBrowserDialog.cpp` | 2h | T2.3 |
| T2.5 Clipping controls | `ScanBrowserDialog.cpp` | 3h | T2.1 |
| T3.1 Camera position marker | `ScanBrowserDialog.cpp` | 3h | T1.1 |
| T3.2 Station thumbnails | `ScanBrowserDialog.cpp` | 3h | T1.2 |
| T3.3 Annotations | `ScanBrowserDialog.cpp` | 4h | T2.3 |
| T3.4 Export | `ScanBrowserDialog.cpp` | 1h | T0.4 |

**Total estimated effort: ~52 hours**

---

## Build & Test Commands

```powershell
# After scaffolding (Phase 0)
# Edit C:\dev\tools\cc-configure.cmd and add:
#   -DPLUGIN_STANDARD_QSCANBROWSER=ON

# Then rebuild:
& C:\dev\tools\cc-configure.cmd
& C:\dev\tools\cc-build.cmd

# Run:
& 'C:\dev\CloudCompare\build\qCC\deployqt\CloudCompare.exe'

# Open an RCP file → right-click a scan → "Open Spherical View"
```
