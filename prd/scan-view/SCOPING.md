# Scoping — qScanBrowser

**Reference:** [`PRD.md`](PRD.md) and [`IMPLEMENTATION.md`](IMPLEMENTATION.md).

---

## In Scope

### MVP (Phase 1)

- [ ] Load RCS/RCP via ReCap SDK v26
- [ ] Display structured scan as 360° colour panorama (equirectangular skybox sphere)
- [ ] Bubble-view drag rotation (horizontal = azimuth, vertical = elevation)
- [ ] Station list UI: show all scan stations in the RCP project
- [ ] Click a station → jump to that station's panorama
- [ ] Prev/Next arrow navigation between stations
- [ ] Context menu or File menu entry: "Open Spherical View"
- [ ] FOV scroll wheel

### Phase 2

- [ ] Range image loaded as depth buffer
- [ ] Click-to-measure (2-point distance, displayed in m)
- [ ] Polyline measurement (3+ points)
- [ ] Clipping radius slider (hide points beyond radius)

### Phase 3

- [ ] Camera position marker in project 3D view (frustum glyph at scanner origin)
- [ ] Station thumbnail previews in station list
- [ ] Annotation markers (named points in view)
- [ ] Export panorama as PNG

---

## Out of Scope (Do Not Implement in v1)

| Item | Reason |
|---|---|
| Raw camera photo display | Not exposed by ReCap SDK v26 API |
| VR / SteamVR / Oculus mode | Phase 1 infrastructure incompatible; separate ticket |
| Full point cloud rendering (all pixels as GL_POINTS simultaneously) | Too slow for Phase 1; range-image depth mode (Phase 2) is correct approach |
| Non-structured scan support (mobile scans) | Out of scope for v1; may work but untested |
| RCP project creation / writing back to RCS | Read-only |
| Registration / alignment tools | Separate from viewing; handled by `qManualRegistration` (AGENTS_REGISTRATION.md) |
| Web / browser deployment | Desktop plugin only |
| Colour correction / HDR tone mapping | JPEG-baked colour from SDK; no raw sensor data |
| Faro `.fls/.flp` format support | Requires FARO SDK (separate plugin) |
| Multi-scan overlay in one view (2 stations simultaneously) | Phase 3 at earliest |
| Cloud sync / shareable links | Out of scope |

---

## API Surface: ReCap SDK v26 Used in This Plugin

### Core read path

```
RCScan::loadFile()                       → load RCS
  └─ RCScan::getStructuredScan()        → get structured scan (range image)
       └─ RCSphericalModel (per pixel)
            ├─ polarFromImage(PixelIndex)   → azimuth + elevation
            ├─ cartesianFromImage(PixelIndex) → unit direction vector
            └─ getWidth(), getHeight()
       ├─ getRange(PixelIndex)           → float distance in mm
       ├─ getColor(PixelIndex)          → uint8_t[3] RGB
       ├─ isBadPoint(PixelIndex)         → bool (no return at this direction)
       ├─ getNormal(PixelIndex)          → RCVector3f
       └─ getSegmentId(PixelIndex)        → uint16_t

RCProjectImporter::loadProject()          → load RCP
  └─ RCProject::getScan(i)              → RCScan* per station
       └─ getName(), getOrigin(), getFullTransform()
```

### Coordinate system

| Concept | ReCap API | In view |
|---|---|---|
| Scanner origin (world) | `RCScan::getOrigin()` | Camera position |
| View direction | `azimuth ∈ [−π, +π]`, `elevation ∈ [−π/2, +π/2]` | Mouse drag |
| Pixel → world point | `origin + direction(azimuth,elev) * getRange(pixel)` | Measurement |
| Scan orientation | `getFullTransform()` | Rotates sphere/skybox |

### Key header files

```
Include/data/RCScan.h                 — loadFile(), getOrigin(), getFullTransform(), getStructuredScan()
Include/data/RCStructuredScan.h       — getSphericalModel(), getRange(), getColor(), isBadPoint()
Include/data/RCPointBuffer.h         — Spherical model coordinate type
Include/foundation/RCSphericalModel.h — polarFromImage(), cartesianFromImage()
Include/foundation/RCVector.h          — RCVector3d (identical layout to CCVector3)
Include/foundation/RCTransform.h     — RCTransform (4×4 matrix)
Include/foundation/RCBox.h           — bounding box
Include/data/RCProject.h             — RCProjectImporter, getScan(), getNumberOfScans()
```

---

## Key Decisions to Make Before Starting

### D1 — Single viewport or two?

**Option A:** One GL viewport only (Phase 1)
- Dialog panel on left (300px) + CC's GL viewport on right
- Uses existing CC viewport; no second window
- Simpler, faster

**Option B:** Two viewports from day 1 (Correspondence view)
- Left: project 3D overview with camera frustum marker
- Right: spherical panorama view
- Matches Faro Scene's design
- Requires creating a second `ccGLWindowInterface*`

**Recommendation:** Option A for Phase 1; upgrade to Option B in Phase 3.

### D2 — Render colour as texture on sphere OR as GL_POINTS?

**Option A:** Texture sphere (Phase 1)
- `QImage` → `QOpenGLTexture` → map to unit sphere
- Renders at texture resolution (e.g. 4000×2000 = 8 MP)
- Fast: one draw call
- No per-pixel depth (panorama is background, not occluded)

**Option B:** All pixels as GL_POINTS
- Every valid pixel → one `glVertex3f(origin + direction * range)`
- 8M points = 8M draw calls (too slow for Phase 1)
- Use `glDrawArraysInstanced` with range-image texture lookup (complex)

**Recommendation:** Option A for Phase 1. Phase 2 adds range-image depth buffer as Z-test for geometry overlay.

### D3 — Measurement coordinate system

**Option A:** World coordinates
- Store measurements in the RCP project's coordinate system
- Correct for real-world distances
- Requires applying `getFullTransform()` to all points

**Option B:** Scanner-local coordinates
- All measurements relative to scanner origin
- Simpler; transform to world only on export

**Recommendation:** Option A — survey users need real-world distances.

### D4 — Bubble view or custom rotation?

Bubble view (`setBubbleViewMode(true)`) already implements:
- Viewer-centric rotation (camera at fixed position)
- Mouse drag → view rotation
- Scroll → FOV change

**Decision:** Use bubble view as-is for Phase 1. If drag sensitivity needs tuning, the rotation speed can be scaled in `ccGLWindowInterface` or overridden.

---

## Knowns

| Fact | Source |
|---|---|
| ReCap SDK has structured scan (2D range image) | `RCScan::getStructuredScan()` confirmed |
| Scanner origin is local (0,0,0) in structured scan CS | `RCScan::getOrigin()` returns world position |
| Colour per pixel via `getColor(pixel)` | Confirmed in `IRCPointIterator` API |
| Azimuth −π to +π, elevation −π/2 to +π/2 | Confirmed in `RCSphericalModel` |
| `exportSphericalColorImage()` dumps baked panorama | Confirmed |
| `ccOverlayDialog` is the standard plugin dialog pattern | Confirmed in `qCC/` |
| `ccGBLSensor` uses same spherical model internally | Confirmed in `ccGBLSensor.cpp` |
| Bubble view mode rotates about camera centre | Confirmed in `ccGLWindowInterface` |
| No raw camera photos in SDK | Confirmed in header survey |
| No raycasting API in SDK | Confirmed |

---

## Unknowns (Pre-Implementation Verification Needed)

| # | Unknown | How to Verify |
|---|---|---|
| U1 | Max structured scan resolution for a typical scan (Faro Focus 360°)? | Load a real RCS file with `getStructuredScanWidth()` |
| U2 | Does `getStructuredScan()` work for ALL scans, or only those that were processed for structured output? | Test on an unprocessed RCS — it may return null |
| U3 | What happens if structured scan is unavailable? Fallback to point cloud? | Need graceful fallback path for non-structured scans |
| U4 | Time to load a 4000×2000 structured scan into memory? | Benchmark T0.3 |
| U5 | Can `ccOverlayDialog::drawFrame()` draw before the scene, i.e., as a background? | Check overlay dialog render order in `ccGLWindowInterface.cpp` |
| U6 | Can we share one GL viewport between two view modes (bubble + standard)? | Test: open bubble view → close → standard view still works |
| U7 | How does the ReCap `getFullTransform()` compose — is it column-major or row-major 4×4? | Compare against known scanner coordinates in a test file |
| U8 | Does the ReCap SDK require the DLLs to be on PATH, or is `LoadLibrary` via CMake sufficient? | Test loading on a clean machine or via dependency walker |

---

## Risk Register

| Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|
| Structured scan API returns null for real-world RCS files | Medium | High | T0.3: test before any UI work; fallback to point cloud iteration |
| Bubble view rotation conflicts with measurement picking | Low | Medium | T0.5: test; disable bubble drag when measurement mode active |
| Large scan (>8M px) causes OOM on load | Medium | Medium | T0.4: add density parameter; load at 50% or 25% resolution |
| Depth buffer Z-fight with CC scene geometry | Low | Medium | Phase 2: use `glDepthRange` to push sphere behind scene |
| SDK DLLs not on PATH at runtime | Low | High | POST_BUILD CMake copy → windeployqt bundles them; test on clean machine |
| Colour JPEG compression visible in panorama | Low | Low | Acceptable for v1; note as known limitation |
