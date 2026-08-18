---
title: Stockpile volume
sidebar_label: Stockpile volume
sidebar_position: 4
description: Compute cut / fill between a pile scan and a reference surface (ground, prior survey, design plane).
---

# Stockpile volume

The "how many cubic metres is in this pile" workflow. The deliverable is
typically a single number (m³) plus an optional 2D heatmap showing where
the volume sits in the pile.

Two main variants:

1. **Pile vs ground** — the surface under the pile is the original
   ground (often from a prior survey). Compute the cut / fill between
   the current scan and that reference.
2. **Pile vs prior epoch** — both surfaces are pile scans (e.g. this
   month's stock vs last month's). Compute the difference.

## Pipeline (pile vs reference surface)

1. **Open both** the pile scan and the reference surface (ground or
   prior epoch) in the [DB tree](/docs/reference/db-tree).
2. **Register** the two with ICP if they were captured independently.
   If the reference is a known coordinate surface, just align by
   matching the coordinate systems.
3. **Compute the volume** with `Tools > Volume computation` (built-in).
   Pick the ground / reference as the base and the pile as the top.
4. **Inspect the result** — the dialog reports `volume above` and
   `volume below` the base. For a stockpile sitting on the ground,
   `volume above` is what you want.
5. **Export** the per-point height / signed-distance as a `.las` with
   the height as a scalar field, so the client can re-render the
   heatmap in any viewer.

## Pipeline (pile vs prior epoch)

1. **Open both pile scans** as `epoch-0` and `epoch-N`.
2. **Register** with ICP, anchored on the **non-pile** parts of the
   site (the floor, the walls, the fixed structures) — never the
   pile itself, which has moved.
3. **M3C2 distance** — same as the
   [monitoring workflow](/docs/workflows/monitoring#pipeline), step 5.
4. **Volume from signed distance** — CloudCompare's built-in volume
   tool only works on a mesh or a closed surface, not on an M3C2
   scalar field. You have three options:
   - **Triangulate the pile surface first** (Poisson or 2.5D Delaunay),
     then run the built-in volume tool.
   - **Use the built-in `Tools > Compute 2.5D volume`** with a rasterised
     DEM of the height map.
   - **Export the M3C2 scalar field** and compute the volume in
     Python / MATLAB / your GIS of choice. The maths is `Σ(d_i × A_cell)`
     where `d_i` is the signed distance and `A_cell` is the cell area.

## Which plugins you actually touch

| Step | Plugin | Notes |
|---|---|---|
| ICP registration | built-in | No plugin |
| M3C2 (epoch vs epoch) | [`qM3C2`](/docs/plugins/local-set) | Self-contained |
| Poisson surface reconstruction | [`qPoissonRecon`](/docs/plugins/local-set) | For mesh-based volume |
| 2.5D volume | built-in | No plugin |

## Gotchas

- **The reference surface is everything.** If the "ground" reference has
  any slope or curvature, the volume will be wrong by a constant offset.
  For piles on a sloped yard, model the ground as a plane (least-squares
  RANSAC) and use that as the reference.
- **Edge effects at the pile base** are the biggest source of error. The
  bottom 10-20 cm of the pile (where the material meets the ground) is
  fuzzy in any scan, and any deviation between the pile base and the
  ground reference becomes a large volume error when integrated over
  the pile's footprint. Crop the pile to its clearly-above-ground
  portion before measuring.
- **Bulk density vs volume**. The workflow gives you volume. The
  tonnage comes from the material's bulk density (kg/m³). That's a
  separate input — sand, gravel, and coal all have different bulk
  densities and they change with moisture.
- **Date the scans**. Stockpile inventory is usually a snapshot, but
  auditors and clients want the timestamp.
