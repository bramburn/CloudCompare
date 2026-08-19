# PRD — ReCap Scan Viewer: Spherical Panorama Point Cloud Browser

**Version:** 0.1 Draft
**Date:** 2026-08-19
**Author:** Icelabz / CloudCompare Fork
**Status:** Research Complete — Ready for Design Review

---

## 1. Vision & Purpose

> **"Google Street View, but for ReCap RCS/RCP point cloud scans."**

A plug-in for CloudCompare that lets users navigate registered scan positions from an RCP project as immersive 360° spherical views — the same experience as Faro Scene Classic's PanoCam viewer or Google Maps Street View, but driven by actual LiDAR point cloud data.

From any registered scan station in an RCP project, the user steps inside the scan, sees a full 360° × 300° spherical view of the point cloud rendered from that scanner's position, and can navigate between scan stations (like jumping between Street View panoramas). They can also take measurements, place annotations, and clip the view.

**This is not a 3D modelling tool.** It is a **scan browser** — immersive, positional, and navigation-first.

---

## 2. Reference Experiences

### 2a. Faro Scene Classic — PanoCam View

| Feature | How it works in SCENE |
|---|---|
| **Spherical panorama** | Colour baked as 8000×4000 (32 MP) JPEG spherical image inside PanoCam scan |
| **Pano Virtual Depth Mode** | Hidden point cloud used to occlude scene geometry — creates real depth occlusion even with panorama as background |
| **Correspondence View** | Two synchronized viewports: one shows scan placement in the 3D project; the other shows the scan from that position |
| **Scan navigation** | Double-click scan → enters PanoCam view; click on another scan station to jump to it |
| **Measurements** | Click two points in panorama view → distance returned in scene units |
| **Clipping boxes** | Multiple spatial filters to isolate regions in the view |
| **VR View** | SteamVR/Oculus Rift/HTC Vive stereoscopic panorama |
| **Scanner specs** | Focus: 360° horizontal × 300° vertical FOV |

**Key insight:** Faro doesn't render a real-time point cloud in PanoCam view — it renders a **pre-computed JPEG spherical colour panorama** (from the scanner camera images), with a **hidden depth buffer** (from the laser scan) used to occlude overlaid geometry. The depth buffer gives you real-world scale; the panorama gives you photorealism.

### 2b. Google Street View

| Feature | How it works |
|---|---|
| **Spherical image** | 28 tiles of 512×512 stitched to 3328×1664 equirectangular panorama |
| **Depth map** | Per-pixel depth from structure-from-motion (not LiDAR) |
| **Navigation** | Neighbouring panoramas linked; click arrows to jump |
| **Rendering** | Client requests tiles at appropriate LOD; renders as texture on sphere (or cube-map) |
| **Measurements** | Depth map + raycasting from click direction → 3D point → distance |

**Key insight:** Street View uses the equirectangular panorama as a skybox texture and the depth map for occlusion and measurement. The same pattern applies to LiDAR scans: equirectangular colour + range image = spherical point cloud view.

### 2c. Potree (open-source reference implementation)

- WebGL point cloud renderer (GitHub: potree/potree)
- Converts LAS/LAZ to Potree binary format (octree LOD)
- Supports 360° image mode: panorama image as background, point cloud rendered in front
- Measurement tools, clipping, annotations
- **Relevance:** the CloudCompare spherical view can use Potree's skybox-rendering approach inside a ccGLWindow

---

## 3. What We Have in the Codebase

### 3a. ReCap SDK v26 — Structured Scan API

The SDK already has a structured scan mode. Key capabilities:

```
RCStructuredScan.getSphericalModel()
  → imageFromPolar(PolarIndex)    azimuth/elevation → pixel (x,y)
  → polarFromImage(PixelIndex)    pixel → azimuth/elevation
  → cartesianFromImage(PixelIndex) pixel → unit direction vector

For each pixel (x, y):
  range     = getRange(pixel)           // distance in mm
  color[3]  = getColor(pixel)          // RGB uint8
  normal    = getNormal(pixel)          // RCVector3f
  segmentId = getSegmentId(pixel)       // uint16_t
  isBad     = isBadPoint(pixel)         // no return at this direction

Scanner origin = local (0,0,0)
Full transform: UCSTransform × GroupToSurveyTransform × ScanToGroupTransform
```

- **Azimuth:** −π to +π (full horizontal sweep)
- **Elevation:** −π/2 to +π/2 (top to bottom)
- **Export:** `exportSphericalColorImage(path)` — dumps baked panorama as image file
- **Colour storage:** JPEG-compressed spherical panorama image inside `.rcc`
- **No raw camera photos** exposed via SDK

### 3b. CloudCompare — What already exists

| What | Where | Status |
|---|---|---|
| `ccGBLSensor` — spherical depth map sensor (yaw/pitch grid) | `libs/qCC_db/ccGBLSensor.*` | ✅ Works |
| `ccDepthBuffer` — 2D range image | `libs/qCC_db/ccDepthBuffer.*` | ✅ Works |
| `setupProjectiveViewport()` — wire sensor pose into GL viewport | `libs/qCC_glWindow/ccGLWindowInterface.*` | ✅ Works |
| `bubbleViewMode` — viewer-centric rotation | `ccGLWindowInterface.*` | ✅ Works (not a sphere renderer) |
| `ccIndexedTransformationBuffer` — multi-position trajectory | `libs/qCC_db/ccIndexedTransformationBuffer.*` | ✅ Works |
| Sensor frustum drawing | `ccGBLSensor::drawMeOnly()` | ✅ Works |
| Measurement tools in GL viewport | Existing CC UI | ✅ Work |
| `ccOverlayDialog` — plugin dialog pattern | `qCC/` | ✅ Works |
| Equirectangular/spherical panorama viewer | — | ❌ Does not exist |
| Colour panorama background in GL window | — | ❌ Does not exist |
| Scan-station navigation (Street View jump) | — | ❌ Does not exist |
| Point-picking from scanner origin (raycasting) | — | ❌ Does not exist |

---

## 4. Proposed Architecture

### 4a. Plugin: `qScanBrowser`

**Type:** Standard Plugin (ccOverlayDialog subclass)
**Location:** `plugins/core/Standard/qScanBrowser/`
**Minimum CC version:** 2.12

### 4b. Core Concept: Spherical Range Image Renderer

The ReCap structured scan stores a **2D range image** (not an equirectangular image). The rendering strategy is:

```
ReCap structured scan (2D pixel grid: width × height)
  + SphericalModel (azimuth/elevation ↔ pixel mapping)
  = Equirectangular range buffer (width × height, depth in mm)
  + Colour image (RGB, from getColor(pixel) per pixel)
  = Spherical point cloud (visible from scanner origin)

Rendered as:
  ┌──────────────────────────────────────────┐
  │  Skybox / background: equirectangular    │  ← rendered as GL sphere or quad
  │  colour texture from ReCap SDK           │
  │                                          │
  │  Point cloud overlay: raw points from   │  ← GL_POINTS or textured quad
  │  structured scan projected per-pixel     │
  │                                          │
  │  Measurement layer: pick rays from       │  ← ccPickingHub integration
  │  scanner origin through click direction   │
  └──────────────────────────────────────────┘
```

**Two rendering modes (Faro-style):**

| Mode | Description | Implementation |
|---|---|---|
| **Colour panorama** | Pre-computed spherical colour image as skybox; range image used for depth occlusion | Load `getColor(pixel)` into `QImage`, map to GL texture on sphere; range image used for Z-buffer |
| **Point cloud** | Every pixel rendered as a GL point from scanner origin at correct range | `glBegin(GL_POINTS)` over all valid pixels; colour from `getColor()`; depth from `getRange()` |

### 4c. Navigation Model

```
RCP project loaded in CloudCompare
  → list of RCScan objects
  → for each scan: RCScan::getOrigin() + RCScan::getFullTransform()
  → stored as vector<ScanStation>

User clicks a scan station → qScanBrowser opens
  → Scanner origin = camera position in GL window
  → Azimuth/elevation from mouse drag = view direction
  → Click another station → jump (viewer navigates to new origin)
  → Arrow buttons in UI → navigate to nearest neighbour station
```

### 4d. Measurement

When user clicks in spherical view:
1. Convert mouse position to `(azimuth, elevation)` in the view
2. Get `range = getRange(pixel)` from structured scan at that direction
3. If `isBadPoint` → no measurement (sky/open air)
4. Else → `point3D = origin + direction(azimuth, elevation) * range`
5. Two such points → `distance = |p1 - p2|`

This is **identical to the Street View depth-map measurement approach.**

---

## 5. Feature List

### Phase 1 — MVP (Spherical View Only)

| # | Feature | Description |
|---|---|---|
| F1 | Open scan station | User double-clicks an RCS scan → opens spherical view overlay at that station |
| F2 | 360° spherical colour panorama | Colour image from ReCap SDK rendered as skybox in GL viewport |
| F3 | Mouse-drag navigation | Horizontal drag = azimuth; vertical drag = elevation (bubble view mode) |
| F4 | Station jump navigation | Click on a neighbouring scan in the project to jump to it |
| F5 | Arrow navigation | Prev/Next buttons to navigate between stations |
| F6 | FOV control | Scroll wheel changes field of view |

### Phase 2 — Depth & Measurement

| # | Feature | Description |
|---|---|---|
| F7 | Range-image depth buffer | Range image used as depth map for Z-buffer occlusion |
| F8 | Point cloud overlay | All valid pixels rendered as coloured GL_POINTS at correct 3D positions |
| F9 | Point-to-point measurement | Click two points → distance in scene units |
| F10 | Multi-point polyline measurement | Click multiple points → chain distance |
| F11 | Point cloud clipping | Clipping sphere/box centred on scanner origin |

### Phase 3 — Immersive Polish

| # | Feature | Description |
|---|---|---|
| F12 | Full 360° point cloud mode | Colour panorama as background + full-resolution point cloud overlay |
| F13 | Correspondence view | Two synchronized viewports: project overview + spherical view |
| F14 | Scan station thumbnails | Preview images for each station for navigation UI |
| F15 | Annotation markers | Place named point markers in the spherical view |
| F16 | Export spherical image | Save the equirectangular panorama as PNG/JPEG |

---

## 6. Technical Constraints

### 6a. Performance

| Concern | Mitigation |
|---|---|
| Millions of points per station | Render from range image directly (width × height = e.g. 4000 × 2000 = 8M points max); downsample via `RCPointIteratorSettings::setDensity()` for interactive view |
| Colour per pixel from ReCap SDK | `getColor(pixel)` is O(1) per pixel; pre-load into a `QImage` on station open |
| Smooth drag at 60 fps | Use `bubbleViewMode` (viewer-based rotation); do NOT recompute geometry on drag — just rotate the GL camera |
| Large RCP project (100+ scans) | Lazy-load: only decompress structured scan for current station |

### 6b. SDK Constraints

| Constraint | Impact |
|---|---|
| `exportSphericalColorImage()` uses baked JPEG colour | Colour quality limited to JPEG compression; no HDR |
| No raw camera photos in SDK | Cannot show unprocessed photos in the panorama |
| No raycasting API | Depth occlusion must be implemented manually via range image |
| `setIsVisiblePointsOnly()` is pre-computed | Cannot do dynamic occlusion queries; range image gives first-hit only |

### 6c. CC Plugin Constraints

| Constraint | Impact |
|---|---|
| `ccGLWindowInterface` is the only GL handle available to plugins | Must use existing viewport; cannot create raw OpenGL context |
| `bubbleViewMode` is viewer-centric rotation only | Drag rotates camera about its own axis (correct for spherical view) |
| No GPU-side scene graph | All rendering is CPU-side `drawMeOnly()`; no shader-based point cloud renderer yet |

---

## 7. Comparison: What We're Building vs What Exists

| Feature | Faro Scene PanoCam | Google Street View | CloudCompare today | **Our implementation** |
|---|---|---|---|---|
| Spherical colour panorama | ✅ JPEG baked | ✅ Tiled image | ❌ | ✅ ReCap SDK |
| Range-image depth | ✅ (from laser) | ✅ (from SfM) | ❌ | ✅ ReCap SDK |
| Point cloud overlay | ❌ (hidden depth only) | ❌ | ❌ | ✅ (Phase 2) |
| Click-to-measure | ✅ | ✅ (depth map raycast) | ❌ (3D view only) | ✅ Phase 2 |
| Station jump navigation | ✅ | ✅ | ❌ | ✅ Phase 1 |
| Correspondence view | ✅ | N/A | ❌ | ✅ Phase 3 |
| VR mode | ✅ (SteamVR) | ❌ | ❌ | Future |

---

## 8. Open Questions

| # | Question | Priority |
|---|---|---|
| Q1 | Can we load a structured scan without loading the full RCS point cloud? (i.e., just the `.rcc` spherical image) | High — affects memory budget |
| Q2 | What is the maximum structured scan resolution for a typical scan? Does it vary by scanner? | Medium — affects F1/F2 rendering approach |
| Q3 | Should the overlay dialog use one GL viewport or two (correspondence view from day 1)? | Design decision — affects Phase 1 scope |
| Q4 | Do we need to support non-structured scans (mobile scans, unstructured RCS)? | Low priority for MVP |
| Q5 | Should this integrate with the `AGENTS_REGISTRATION.md` dual-viewport goal, or be a separate plugin? | High — shared infra (dual ccGLWindow) should be designed together |

---

## 9. Success Criteria

| Criterion | Measurement |
|---|---|
| Load an RCP with 3+ stations → open station 2 → see 360° colour panorama | Manual test |
| Drag mouse → view rotates smoothly at ≥30 fps | Performance test |
| Click two points in view → measurement returned within 1s | Manual test |
| Jump to next station → view repositions correctly | Manual test |
| No memory blow-up with 100+ station RCP | Memory profiling |
| Plugin loads and shows "Open Spherical View" on scan right-click | Integration test |
