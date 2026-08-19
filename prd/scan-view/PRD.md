# PRD — ReCap Scan Viewer: Spherical Panorama Point Cloud Browser

**Version:** 0.2 — Decisions Finalised
**Date:** 2026-08-19
**Author:** Icelabz / CloudCompare Fork
**Status:** Decisions locked · Ready for implementation

---

## Changelog from v0.1

| Decision | Resolution |
|---|---|
| R7 (Scan viewer window model) | **A — separate top-level window** (not dockable panel) |
| R8 (Bidirectional picking) | **A — 3D ↔ bubble, both directions** |
| R9 (3D ↔ bubble geometry) | **A — ReCap camera pose from scan metadata** |
| R15 (Manual registration viewport) | **C — single viewport first; dual is Phase 2** (see AGENTS_REGISTRATION.md) |
| R16 (Memory budget) | **B — auto-subsample at 50M points (configurable)** |
| R17 (Project metadata) | **C — apply CRS + companion JSON file** |
| R20 (FARO parity) | **B — standalone; FARO is future plugin** |

---

## 1. Vision & Purpose

> **"Google Street View, but for ReCap RCS/RCP point cloud scans."**

A Standard plugin for CloudCompare that lets users navigate registered scan positions from an RCP project as immersive 360° spherical views — the same experience as Faro Scene Classic's PanoCam viewer or Google Maps Street View, but driven by actual LiDAR point cloud data.

From any registered scan station in an RCP project, the user steps inside the scan, sees a full 360° × 300° spherical view of the point cloud rendered from that scanner's position, and can navigate between scan stations (like jumping between Street View panoramas). They can also take measurements, place annotations, and clip the view.

**This is not a 3D modelling tool.** It is a **scan browser** — immersive, positional, and navigation-first. It is complementary to the **qManualRegistration** plugin (dual-viewport point-pair picking) — they share the picking infrastructure but address different workflows.

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

**Key insight:** Faro doesn't render a real-time point cloud in PanoCam view — it renders a **pre-computed JPEG spherical colour panorama** (from the scanner camera images), with a **hidden depth buffer** (from the laser scan) used to occlude overlaid geometry.

### 2b. Google Street View

| Feature | How it works |
|---|---|
| **Spherical image** | 28 tiles of 512×512 stitched to 3328×1664 equirectangular panorama |
| **Depth map** | Per-pixel depth from structure-from-motion (not LiDAR) |
| **Navigation** | Neighbouring panoramas linked; click arrows to jump |
| **Rendering** | Client requests tiles at appropriate LOD; renders as texture on sphere |
| **Measurements** | Depth map + raycasting from click direction → 3D point → distance |

### 2c. Potree (open-source reference implementation)

- WebGL point cloud renderer (GitHub: potree/potree)
- Converts LAS/LAZ to Potree binary format (octree LOD)
- Supports 360° image mode: panorama image as background, point cloud rendered in front
- Measurement tools, clipping, annotations
- **Relevance:** the CloudCompare spherical view can use Potree's skybox-rendering approach inside a ccGLWindow

---

## 3. What We Have in the Codebase

### 3a. ReCap SDK v26 — Structured Scan API

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
| `ccGBLSensor` — spherical depth map sensor | `libs/qCC_db/ccGBLSensor.*` | ✅ Works |
| `ccDepthBuffer` — 2D range image | `libs/qCC_db/ccDepthBuffer.*` | ✅ Works |
| `setupProjectiveViewport()` | `libs/qCC_glWindow/ccGLWindowInterface.*` | ✅ Works |
| `bubbleViewMode` — viewer-centric rotation | `ccGLWindowInterface.*` | ✅ Works |
| `ccIndexedTransformationBuffer` | `libs/qCC_db/ccIndexedTransformationBuffer.*` | ✅ Works |
| Sensor frustum drawing | `ccGBLSensor::drawMeOnly()` | ✅ Works |
| Measurement tools in GL viewport | Existing CC UI | ✅ Work |
| `ccOverlayDialog` — plugin dialog pattern | `qCC/` | ✅ Works |
| Equirectangular/spherical panorama viewer | — | ❌ Does not exist |
| Colour panorama background in GL window | — | ❌ Does not exist |
| Scan-station navigation (Street View jump) | — | ❌ Does not exist |
| Bidirectional 3D ↔ bubble picking | — | ❌ Does not exist |

---

## 4. Proposed Architecture

### 4a. Plugin: `qScanBrowser`

**Type:** Standard Plugin (QWidget + ccGLWindow subclass)
**Location:** `plugins/core/Standard/qScanBrowser/`
**Minimum CC version:** 2.12 / Qt 6.8

**Key design decisions (R1–R20 locked):**

| Decision | Resolution | Rationale |
|---|---|---|
| R1 — Transform on load | **B — store as ccHObject metadata** | Most flexible; recoverable; survey users may work in scanner-local coords |
| R2 — Normals | **A — import ReCap normals if present** | Avoids redundant computation; fall back to CC computation if rendering looks wrong |
| R3 — Scalar fields | **C — import both; default to intensity** | Both are valuable for survey use; user switches via Scalar Fields Manager |
| R4 — RCP multi-scan | **A — one container, N children** | Preserves scan-level identity; user can hide/show; can merge manually |
| R5 — Auto chunking | **A — no auto chunking** | Silent chunking changes the data model unexpectedly |
| R6 — Bubble view source | **A — spherical images from SDK** | Best UX if SDK exposes them; fall back to range-image render |
| R7 — Window model | **A — separate top-level window** | Dual-monitor setups; matches FARO SCENE; no dock complexity |
| R8 — Bidirectional picking | **A — 3D ↔ bubble both directions** | Core differentiator; required for registration workflow |
| R9 — 3D ↔ bubble geometry | **A — ReCap camera pose from metadata** | Most accurate; SDK already computed it during bundle adjustment |
| R10 — Intensity display | **B — leave on cloud, display RGB** | Safe default; user sees colour first; intensity available in SFM |
| R11 — Colour space | **A — pass through as-is** | No colour science info in scan; wrong conversion makes it worse |
| R12 — Station metadata | **A — entity metadata on ccHObject** | Simplest; survives save/load; visible in Properties panel |
| R13 — Read/write | **C — investigate write after validating read** | Out of scope for Phase 1 |
| R14 — Default interaction | **A — orbit** | Most users want to look first |
| R15 — Registration viewport | **C — single first, dual is Phase 2** | See AGENTS_REGISTRATION.md |
| R16 — Memory budget | **B — auto-subsample at 50M (configurable)** | Pragmatic middle ground; original cloud intact |
| R17 — Project CRS | **C — apply CRS + companion JSON** | CRS visible in UI + full fidelity preserved |
| R18 — Scan picker | **A — load all by default** | Minimal friction; user hides/deletes unwanted via db-tree |
| R19 — Integration tests | **A — alignment + ground fitting** | Survey primary use cases |
| R20 — FARO parity | **B — standalone** | FARO SDK evaluation is future work |

### 4b. Core Concept: Spherical Range Image Renderer

The ReCap structured scan stores a **2D range image** (not an equirectangular image). The rendering strategy:

```
ReCap structured scan (2D pixel grid: width × height)
  + SphericalModel (azimuth/elevation ↔ pixel mapping)
  = Equirectangular range buffer (width × height, depth in mm)
  + Colour image (RGB, from getColor(pixel) per pixel)
  = Spherical point cloud (visible from scanner origin)

Rendered as:
  ┌──────────────────────────────────────────┐
  │  Skybox / background: equirectangular    │  ← GL sphere with colour texture
  │  colour texture from ReCap SDK           │
  │                                          │
  │  Point cloud overlay: raw points from   │  ← GL_POINTS or textured quad
  │  structured scan projected per-pixel     │
  │                                          │
  │  Measurement layer: pick rays from      │  ← ccPickingHub integration
  │  scanner origin through click direction   │
  └──────────────────────────────────────────┘
```

### 4c. Bidirectional Picking (R8 — core differentiator)

The central feature. Implementation path:

1. **3D → bubble:** User clicks point on 3D cloud → use ReCap camera pose to project into spherical image → highlight corresponding pixel
2. **Bubble → 3D:** User clicks pixel in bubble view → get range from range image → reconstruct 3D point → highlight in 3D viewport

This requires the ReCap camera pose (position + orientation) from scan metadata — confirmed by R9.

### 4d. Navigation Model

```
RCP project loaded in CloudCompare (via qReCapIO)
  → list of RCScan objects
  → for each scan: RCScan::getOrigin() + RCScan::getFullTransform()
  → stored as vector<ScanStation>

User double-clicks an RCS scan → qScanBrowser opens
  → Separate top-level window (R7)
  → Scanner origin = camera position in GL window
  → Azimuth/elevation from mouse drag = view direction
  → Click another station → jump (viewer navigates to new origin)
  → Arrow buttons in UI → navigate to nearest neighbour station
```

---

## 5. Feature List

### Phase 1 — MVP (Spherical View Only)

| # | Feature | Decision ref |
|---|---|---|
| F1 | Open scan station | User double-clicks an RCS scan → opens scan viewer at that station |
| F2 | 360° spherical colour panorama | Colour image from ReCap SDK rendered as skybox in GL viewport |
| F3 | Mouse-drag navigation | Horizontal drag = azimuth; vertical drag = elevation |
| F4 | Station jump navigation | Click on a neighbouring scan in the project to jump to it |
| F5 | Arrow navigation | Prev/Next buttons to navigate between stations |
| F6 | FOV control | Scroll wheel changes field of view |
| F7 | Transform as metadata (R1) | Cloud opens in scanner-local coords; transform stored on entity |
| F8 | ReCap normals import (R2) | Import if present; fall back to CC computation |
| F9 | Both scalar fields (R3) | Intensity + RGB both imported; intensity default |
| F10 | RCP container (R4) | One ccHObject container with N point cloud children |
| F11 | Auto-subsample at 50M (R16) | Configurable threshold; original preserved |
| F12 | CRS + companion file (R17) | Apply CRS to entity; write companion JSON |
| F13 | Scan viewer as separate window (R7) | Top-level window, not dockable |

### Phase 2 — Depth & Measurement

| # | Feature | Decision ref |
|---|---|---|
| F14 | Range-image depth buffer | Range image used as depth map for Z-buffer occlusion |
| F15 | Point cloud overlay | All valid pixels rendered as coloured GL_POINTS at correct 3D positions |
| F16 | Bidirectional picking (R8) | Click in 3D → highlight in bubble; click in bubble → highlight in 3D |
| F17 | Point-to-point measurement | Click two points → distance in scene units |
| F18 | Multi-point polyline measurement | Click multiple points → chain distance |
| F19 | Point cloud clipping | Clipping sphere/box centred on scanner origin |

### Phase 3 — Immersive Polish

| # | Feature | Decision ref |
|---|---|---|
| F20 | Full 360° point cloud mode | Colour panorama as background + full-resolution point cloud overlay |
| F21 | Correspondence view (dual viewport) | Synchronized: project overview + spherical view — **see qManualRegistration** |
| F22 | Scan station thumbnails | Preview images for each station for navigation UI |
| F23 | Annotation markers | Place named point markers in the spherical view |
| F24 | Export spherical image | Save the equirectangular panorama as PNG/JPEG |
| F25 | Dual-viewport registration | **Phase 2 of qManualRegistration (AGENTS_REGISTRATION.md)** |

---

## 6. Technical Constraints

### 6a. Performance

| Concern | Mitigation |
|---|---|
| Millions of points per station | Render from range image directly (width × height = e.g. 4000 × 2000 = 8M points max); downsample via `RCPointIteratorSettings::setDensity()` for interactive view |
| Colour per pixel from ReCap SDK | `getColor(pixel)` is O(1) per pixel; pre-load into a `QImage` on station open |
| Smooth drag at 60 fps | Use `bubbleViewMode` (viewer-based rotation); do NOT recompute geometry on drag |
| Large RCP project (100+ scans) | Lazy-load: only decompress structured scan for current station |
| Auto-subsample at 50M (R16) | Configurable in CC preferences; original cloud preserved on disk |

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
| No GPU-side scene graph | All rendering is CPU-side `drawMeOnly()` |

---

## 7. Comparison: What We're Building vs What Exists

| Feature | Faro Scene PanoCam | Google Street View | CloudCompare today | **Our implementation** |
|---|---|---|---|---|
| Spherical colour panorama | ✅ JPEG baked | ✅ Tiled image | ❌ | ✅ Phase 1 (SDK) |
| Range-image depth | ✅ (from laser) | ✅ (from SfM) | ❌ | ✅ Phase 2 |
| Point cloud overlay | ❌ (hidden depth only) | ❌ | ❌ | ✅ Phase 2 |
| Click-to-measure | ✅ | ✅ (depth map raycast) | ❌ (3D view only) | ✅ Phase 2 |
| Station jump navigation | ✅ | ✅ | ❌ | ✅ Phase 1 |
| Bidirectional picking | ❌ | ❌ | ❌ | ✅ Phase 2 |
| Correspondence view | ✅ | N/A | ❌ | ✅ Phase 3 (qManualRegistration) |
| VR mode | ✅ (SteamVR) | ❌ | ❌ | Future |
| Auto-subsample at 50M | ❌ | N/A | ❌ | ✅ Phase 1 |

---

## 8. Relationship to qManualRegistration

> ⚠️ **R15: Single viewport first; dual-viewport registration is Phase 2.**

The **qScanBrowser** (this plugin) and **qManualRegistration** (AGENTS_REGISTRATION.md) are **complementary but separate plugins**:

| | qScanBrowser | qManualRegistration |
|---|---|---|
| **Purpose** | Spherical bubble view + scan browsing | Point-pair registration of two clouds |
| **Windows** | 1 (bubble view) | 2 (source + target) |
| **Interaction** | Drag bubble, click to measure/navigate | Pick point pairs |
| **Relationship** | Can feed into qManualRegistration (navigate to station → pick in 3D) | Independent standalone tool |
| **Shared infra** | Bidirectional picking (F16), ReCap camera pose (R9) | Transform math, Horn/Umeyama |
| **Phase** | Phase 1 (bubble) / Phase 2 (bidirectional) | Phase 2 (dual-viewport deferred) |

The bidirectional picking (F16 in qScanBrowser) is the **bridge** between the two plugins. Once a user is in the bubble view and has identified corresponding features, those points could be passed to qManualRegistration for the actual transform computation.

---

## 9. Success Criteria

| Criterion | Measurement |
|---|---|
| Load an RCP with 3+ stations → open station 2 → see 360° colour panorama | Manual test |
| Drag mouse → view rotates smoothly at ≥30 fps | Performance test |
| Click two points in view → measurement returned within 1s | Manual test |
| Jump to next station → view repositions correctly | Manual test |
| No memory blow-up with 100+ station RCP | Memory profiling |
| Auto-subsample kicks in above 50M threshold | Unit test |
| CRS applied + companion file written | Manual test |
| Bidirectional picking: click in 3D → highlight in bubble | Manual test (Phase 2) |
| Plugin loads and shows "Open Spherical View" on scan right-click | Integration test |
