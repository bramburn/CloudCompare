---
title: Topographical survey
sidebar_label: Topographical
sidebar_position: 2
description: From a stack of terrestrial laser scans to a DTM, breaklines, and a contour deliverable.
---

# Topographical survey

The most common workflow for a surveying company: take a stack of overlapping
terrestrial scans (or a mobile mapping trajectory), clean them up, and
produce a Digital Terrain Model plus breaklines and contours.

## Pipeline

1. **Import** — drop the raw scans (`.las`, `.e57`, `.ply`, `.zlas`) into
   the [DB tree](/docs/reference/db-tree). One entry per scan is fine; group
   them under a folder called `raw`.
2. **Decimate** — for scans denser than ~5 mm point spacing, decimate by
   space to ~25 mm before any further processing. The CSF and M3C2 steps
   both scale badly with point count. See
   [Subsample > Spatial](/docs/cookbook/subsample).
3. **Register** — if the scans have overlapping coverage, run ICP (or
   manual alignment for sites with sparse overlap). The fork ships the
   built-in ICP under `Tools > Registration > Align (point pairs)` plus
   `Tools > Registration > Fine registration (ICP)`.
4. **Merge** — combine all registered clouds into a single entity
   (`Edit > Merge`). Save as `.bin` for a fast round-trip.
5. **Ground / non-ground** — apply [CSF](/docs/plugins/local-set)
   (Cloth Simulation Filtering) to classify ground points. The cloth
   resolution is your grid resolution — start at 0.5 m and tighten from
   there.
6. **Rasterise** — `Plugin > Rasterize` (or built-in `Edit > Rasterize`)
   to turn the ground points into a DTM (`.tif` GeoTIFF or `.asc` ESRI
   ASCII). Pick the right pixel size for the deliverable — too fine and
   the file balloons; too coarse and breaklines blur.
7. **Breaklines** — extract manually using the polyline tool
   (`Tools > Polyline`) on a cross-section, or use a custom filter. The
   fork does not auto-extract breaklines — that's a manual step.
8. **Contours** — generate from the DTM in your CAD/GIS tool of choice
   (QGIS, Global Mapper, Civil 3D). CloudCompare does not have a built-in
   contour generator.
9. **Export** — DTM as GeoTIFF, breaklines as DXF, classified point
   cloud as `.las` with the standard ASPRS classification codes
   (2 = ground, kept by CSF).

## Which plugins you actually touch

| Step | Plugin | Notes |
|---|---|---|
| ICP / manual align | built-in | No plugin needed |
| CSF | [`qCSF`](/docs/plugins/local-set) | Self-contained |
| M3C2 | [`qM3C2`](/docs/plugins/local-set) | Only if you also have a reference scan to compare against |
| RANSAC segmentation | [`qRANSAC_SD`](/docs/plugins/local-set) | For cleaning up cars, vegetation, fences |
| Hidden point removal | [`qHPR`](/docs/plugins/local-set) | When you want a single view from a fixed viewpoint |

## Gotchas

- **Coordinate system mismatch** is the #1 reason a topo deliverable looks
  wrong. Confirm the incoming scans are in the same system (e.g. OSGB36 /
  EPSG:27700 in the UK) before registration — ICP does not know what
  coordinates mean, only how points sit relative to each other.
- **CSF cloth resolution too tight** produces a noisy DTM. Too loose and
  you miss ridge lines. The cloth-rigidity slider matters more than the
  classification threshold; if you can't see a clean ground / non-ground
  split visually, the parameters are wrong, not the algorithm.
- **Decimation before CSF**, not after. CSF builds an internal octree
  and a 200M-point cloud will run for hours and may run out of memory.
  Decimate by space to 25-50 mm first.
- **Save before merge**. Merged clouds are not reversible. Save the
  individual registered scans as `.bin` and the merged cloud as a separate
  `.bin` so you can re-do a step.
