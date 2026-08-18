---
title: As-built vs design
sidebar_label: As-built vs design
sidebar_position: 5
description: Compare a finished scan against the design model (DWG / IFC / OBJ / STEP) and flag deviations.
---

# As-built vs design

The "did the contractor build it to spec" workflow. You compare the
as-built scan (what's actually on site) against the design model (what
the drawings said should be on site), and you report the deviations.

## Pipeline

1. **Open both** the as-built scan (`.las` / `.e57` / `.ply`) and the
   design model. The design model is usually one of:
   - **Mesh** — `.obj`, `.fbx`, `.stl` from the architect's tool
   - **CAD** — `.dxf` (2D), `.ifc` / `.step` (3D), but you'll need a
     [plugin for IFC/STEP](/docs/plugins/disabled-priority)
     that's currently disabled by default
   - **Point cloud** — the design was a previous scan (BIM-as-built)
2. **Align** the scan to the model. This is the trickiest step:
   - If the design is in real-world coordinates and the scan is too, just
     match coordinate systems.
   - If not, pick 3+ corresponding reference points
     (corners, column centres, known features) and use
     `Tools > Registration > Align (point pairs)` followed by
     `Tools > Registration > Fine registration (ICP)`.
3. **Crop both** to the area of interest. As-built scans often include
   scaffolding, temporary works, the contractor's kit — strip all of that
   out before measuring, otherwise the deviations include noise from
   the site, not from the building.
4. **Mesh-to-mesh distance** — if both are meshes,
   `Tools > Distances > Cloud / Mesh distance`. The result is a
   per-vertex signed distance, mapped onto the model.
5. **Cloud-to-mesh distance** — if the scan is a cloud and the design
   is a mesh, `Tools > Distances > Cloud / Mesh distance` works in the
   other direction. The result is per-cloud-point signed distance.
6. **M3C2** — for noisy real-world scans, M3C2 gives a more robust
   distance than the simple cloud-to-mesh tool. M3C2 needs a reference
   mesh (which you have — the design) and a comparison cloud (the
   as-built).
7. **Filter** to a tolerance band — `Edit > Scalar fields > Filter by
   value`. Anything outside ±tolerance (e.g. ±10 mm for concrete, ±25 mm
   for earthworks) is a deviation worth reporting.
8. **Export** the filtered deviations as a `.las` with the signed
   distance as a scalar field, plus a colour-mapped screenshot for the
   report.

## Which plugins you actually touch

| Step | Plugin | Notes |
|---|---|---|
| ICP / manual align | built-in | No plugin |
| Cloud / Mesh distance | built-in | No plugin |
| M3C2 | [`qM3C2`](/docs/plugins/local-set) | For noisy real-world scans |
| Mesh decimation | built-in | No plugin |
| RANSAC (clean cars / furniture) | [`qRANSAC_SD`](/docs/plugins/local-set) | Pre-cleaning |

## Gotchas

- **The "design" and the "as-built" must be in the same coordinate
  system**. A common mistake is to align visually (move the scan until
  it looks like it's on top of the model) without using point pairs,
  and then report the distances. The distances will be wrong by the
  alignment error.
- **The tolerance band is contract-defined, not algorithm-defined**.
  Don't pick ±5 mm because it looks nice — the spec says ±10 mm or
  ±25 mm. Anything tighter is a dispute waiting to happen.
- **Don't include the scan noise in the deviation**. Real-world scans
  have 2-5 mm of per-point noise. A 3 mm "deviation" is just noise. The
  tolerance band should be wider than the noise floor.
- **Filter the design too**. A 1 mm tessellation on a smooth surface
  creates a false "deviation" because the scan can't resolve sub-mm
  features. Coarsen the design mesh (or the cloud) to a common
  resolution first.
- **The colour scale is the report**. A diverging scale centred on zero
  is the only honest way to show deviations — blue for "inside the
  tolerance / built in", red for "outside the tolerance / built out".
  The client's eye should land on the red first.
