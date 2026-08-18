---
title: Cookbook
sidebar_label: Cookbook
sidebar_position: 1
description: Task-shaped recipes for the operations you'll do a hundred times — alignment, distance, cropping, scalar fields, measurements, batch CLI.
---

# Cookbook

A task-shaped reference for the operations you'll do a hundred times. Each
recipe is self-contained: starting state, the steps, the gotchas, the
related plugins. Use `Ctrl+F` to jump to the recipe you need.

> **Assumes you can drive the app.** This is the "How do I…?" layer, not
> the "What is this UI element?" layer. For UI navigation, see the
> [DB tree guide](/docs/reference/db-tree). For end-to-end pipelines, see
> the [workflows](/docs/workflows) section.

## Contents

1. [Align two scans with ICP](#align-two-scans-with-icp)
2. [Align with point pairs (manual)](#align-with-point-pairs-manual)
3. [Compute cloud-to-cloud distance](#compute-cloud-to-cloud-distance)
4. [Compute cloud-to-mesh distance](#compute-cloud-to-mesh-distance)
5. [Crop a region to a bounding box or polygon](#crop-a-region-to-a-bounding-box-or-polygon)
6. [Subsample a cloud](#subsample-a-cloud)
7. [Extract a cross-section / polyline profile](#extract-a-cross-section--polyline-profile)
8. [Compute 2.5D volume](#compute-25d-volume)
9. [Change scalar field mapping and colours](#change-scalar-field-mapping-and-colours)
10. [Take a measurement (point-to-point, point-to-plane, angle)](#take-a-measurement-point-to-point-point-to-plane-angle)
11. [Take a viewport screenshot at fixed DPI](#take-a-viewport-screenshot-at-fixed-dpi)
12. [Batch process from the command line](#batch-process-from-the-command-line)
13. [Merge multiple entities into one](#merge-multiple-entities-into-one)
14. [Save and reload a working session](#save-and-reload-a-working-session)

---

## Align two scans with ICP

**Start:** two clouds, roughly aligned already, in the
[DB tree](/docs/reference/db-tree). Roughly aligned = visually overlapping
within ~10° rotation and ~10% of the scene scale.

**Steps:**

1. Select the cloud you want to **move** (the one whose pose is wrong).
2. `Tools > Registration > Fine registration (ICP)`.
3. In the dialog, pick the **reference** cloud (the one that stays put).
4. Set `Random sampling limit` to 50,000 for the first pass; tighten to
   500,000 once the rough alignment is good.
5. Leave `Final overlap` at 100% if the clouds are full scenes; reduce
   to 70-90% if some portion of the moving cloud has no counterpart
   (e.g. you moved the scanner and the foreground changed).
6. Click `OK`. The moving cloud's transform is updated; the DB tree
   shows the new pose.

**Gotchas:**

- **ICP converges to the nearest local minimum, not the global one.** If
  the rough alignment is off by more than ~10°, ICP will not fix it.
  Use [point-pair alignment](#align-with-point-pairs-manual) first.
- **`Random sampling limit` too small** produces a jittery alignment
  that gets worse with more iterations. Too large and the first
  iteration is slow.
- **Don't ICP points that are not the same thing.** ICP aligns surfaces,
  not semantics. If you ICP a winter scan against a summer scan, the
  alignment will chase the foliage, not the ground.

---

## Align with point pairs (manual)

**Start:** two clouds, **not** roughly aligned (e.g. the scanner moved
to a new position and the coordinate system is fresh).

**Steps:**

1. Select both clouds in the DB tree.
2. `Tools > Registration > Align (point pairs)`.
3. Click a feature on the **reference** cloud (left viewport). The
   dialog captures the 3D pick.
4. Click the **same feature** on the moving cloud (right viewport).
5. Repeat for 3-4 point pairs. The more spread out they are, the better
   the alignment.
6. Click `Align`. A preview transform is shown.
7. `Apply` if it looks right, then `ICP` to refine.

**Gotchas:**

- **Pick features, not the nearest scan point.** A chimney corner, a
  fence post, the centre of a manhole cover. If you can't see the same
  feature in both clouds, you can't align them.
- **Pairs should not be collinear.** Four pairs on a line is worse than
  three pairs forming a triangle.
- **One bad pair breaks the alignment.** If the dialog highlights a
  pair as an outlier, delete it and re-pick.

---

## Compute cloud-to-cloud distance

**Start:** two roughly-aligned clouds of similar density.

**Steps:**

1. Select the **reference** cloud first, then the **compared** cloud
   (selection order matters — the second is the one that gets the
   scalar field).
2. `Tools > Distances > Cloud-Cloud distance` (built-in) or
   `Plugin > M3C2` (more robust for noisy real-world scans — see
   [M3C2 plugin doc](/docs/plugins/local-set)).
3. The compared cloud gets a new scalar field called `Distance`. Right
   click → `Display > Color by SF` to see the heatmap.
4. `Edit > Scalar fields > Filter by value` to drop noise-floor points.

**Built-in vs M3C2:** the built-in tool computes a per-point nearest
neighbour distance. M3C2 computes a robust multiscale normal-projected
distance with confidence. M3C2 is slower but more honest on real-world
scans. Use the built-in tool for clean synthetic or registration-error
analysis; use M3C2 for monitoring and as-built workflows.

---

## Compute cloud-to-mesh distance

**Start:** a cloud and a triangulated mesh (the design model, a
reconstructed surface, a prior scan converted to mesh).

**Steps:**

1. Select the cloud, then the mesh.
2. `Tools > Distances > Cloud-Mesh distance` (built-in) or
   `Plugin > M3C2` if both surfaces are noisy.
3. The cloud gets a new scalar field. Right click → `Color by SF`.

**Gotchas:**

- **Sign convention**: positive distance = the cloud point is on the
  **outside** of the mesh (further from the origin than the closest
  mesh face). Negative = inside. This is the convention for as-built
  vs design where "built in" reads as "inside the design envelope".
- **Open meshes give wrong distances at the edges** because the nearest
  mesh face can be on the other side. Close the mesh first
  (`Edit > Mesh > Close holes`) or crop the cloud to a safe buffer
  inside the mesh boundary.

---

## Crop a region to a bounding box or polygon

**Start:** a cloud you want to trim.

**Bounding box:**

1. Select the cloud.
2. `Edit > Crop`.
3. Drag the box handles in the viewport, or enter min/max
   coordinates by hand.
4. `OK`. The original is replaced by a smaller cloud. **The crop is
   destructive** — the original points are gone.

**Polygon:**

1. `Tools > Polyline` to draw a closed polygon on a viewport (or
   `Tools > Segment` to grow a region from a seed point).
2. `Edit > Crop` and switch the dialog to "Inside / outside polyline".

**Gotchas:**

- **Crop is destructive** — there is no undo after you save the file.
  Always work on a copy: right-click the cloud → `Clone` before
  cropping.
- **For M3C2 / monitoring**, crop both clouds to the **same** polygon.
  If you crop them to different regions, the distances are computed
  across the union, not the intersection.

---

## Subsample a cloud

**Three modes, three uses:**

- **Random subsampling** (`Edit > Subsample > Random`) — fastest, but
  preserves neither shape nor density. Use for quick previews.
- **Spatial subsampling** (`Edit > Subsample > Space`) — keeps roughly
  one point per voxel of side `step`. Use for clean pre-M3C2
  preparation. Recommended step is 2-5× the original point spacing.
- **Octree-based** (`Edit > Subsample > Octree`) — keeps at most one
  point per octree leaf at the chosen subdivision level. Use for
  level-of-detail rendering.

**Recommended** for monitoring / as-built / stockpile preparation is
**spatial subsampling at the larger of the two clouds' median spacing**,
applied to **both** clouds so the M3C2 normal estimation is symmetric.

---

## Extract a cross-section / polyline profile

**Start:** a cloud you want to profile.

**Steps:**

1. `Tools > Polyline` (or `Shift+P`).
2. Click points along the desired section line in a viewport. Press
   `Enter` to finish.
3. The polyline becomes a child of the cloud in the DB tree.
4. Right click the polyline → `Export to ASCII` (or `.dxf`) for CAD.
5. To get a **height profile** (distance from a reference plane),
   select the polyline and use
   `Tools > Projection > Project cloud / mesh along a polyline`.

**Gotchas:**

- **The polyline is a 2D line in viewport space, then projected to 3D**.
  Use the side or top view for clean straight sections; the 3D
  perspective view gives wobbly results.
- **For a width / cross-section view** of a stockpile or trench, draw
  two parallel polylines and use `Tools > Projection > Extract points
  between two polylines`.

---

## Compute 2.5D volume

**Start:** a cloud where every `(x, y)` cell has at most one `z` (a
DEM, a topographic surface, a rasterised stockpile). CloudCompare's
built-in volume tool assumes 2.5D.

**Steps:**

1. Select the cloud.
2. `Tools > Volume computation > 2.5D Volume`.
3. Pick the ground / reference as the base. The tool can either ask
   for a separate ground cloud or compute the lowest point as the
   reference.

**If your data is a true 3D pile** (no clean `(x, y)` raster), rasterise
first:

1. `Plugin > Rasterize` (built-in) to a regular grid.
2. Run the 2.5D volume tool on the rasterised cloud.

For full 3D meshes, use `Tools > Volume computation > Mesh volume`
(closed mesh required).

---

## Change scalar field mapping and colours

**Start:** a cloud with at least one scalar field (any operation that
produces a `Distance`, `Intensity`, or similar).

**Steps:**

1. Right click the cloud → `Display > Color by SF`. The default
   mapping is a uniform blue-to-red ramp.
2. Right click again → `Scalar fields > Display parameters` to fine
   tune:
   - **Saturation range** — drop the bottom 1% and top 1% to ignore
     outliers. This is the single most useful change.
   - **Colour ramp** — pick a **diverging** scale (blue → white → red)
     for signed fields like M3C2 distance. A sequential scale (blue →
     green → red) hides the zero point.
   - **Logarithmic scale** — for power-distribution fields (intensity,
     log-distances) where the bulk of the data sits at the low end.
3. The colour mapping is per-entity; the choice persists in the
   `.bin` file when you save.

---

## Take a measurement (point-to-point, point-to-plane, angle)

**Three measurement tools, all under `Tools > Measurement`:**

- **Point to point** — click two points, get the Euclidean distance.
  Use for clearance checks, beam lengths, anything where the
  endpoints are visible.
- **Point to plane** — pick a point, pick a plane (or a mesh face),
  get the perpendicular distance. Use for offset / flatness checks.
- **Angle** — three points, get the angle at the middle point. Use
  for corner / fold checks.

The measurement becomes a labelled entity in the DB tree. Export to
DXF for the report (`Edit > Save` or right click → `Export to DXF`).

---

## Take a viewport screenshot at fixed DPI

**Start:** the viewport showing what you want to capture.

**Steps:**

1. `Display > Render to file`.
2. Pick a filename, width, and height. The output is a PNG at the
   chosen resolution, independent of the on-screen window size.
3. The render uses the current camera angle, lighting, and
   display settings — set these up first.

**For reports**, render at 3000-4000 px wide (which prints at ~10"
at 300 DPI). For a doc page hero, 1600 px wide is plenty.

---

## Batch process from the command line

**Start:** a shell, a `CloudCompare` binary, one or more input files.

The CLI is the same `CloudCompare.exe` (or `CloudCompare.app` /
`CloudCompare` on Linux). Most dialogs have a CLI flag; the help is
self-documenting.

```bash
# Headless conversion: 100 .las files -> .ply
CloudCompare -SILENT -O "raw/*.las" -C_EXPORT_FMT PLY -SAVE_CLOUDS
```

Common flags:

| Flag | What it does |
|---|---|
| `-O <file>` | Open a file. Repeat for multiple files. |
| `-SILENT` | No GUI, no prompts. Required for batch. |
| `-AUTO_SAVE OFF` | Don't auto-save after every op. |
| `-C_EXPORT_FMT <fmt>` | Set export format (`PLY`, `LAS`, `OBJ`, `E57`, …). |
| `-O -GLOBAL_SHIFT AUTO` | Auto-shift large coordinates to fit float32. |
| `-NO_TIMESTAMP` | Stable output filenames for diff-friendly workflows. |

For full flag list, run `CloudCompare -help` or see
[CloudCompare's command-line reference](https://www.cloudcompare.org/doc/wiki/index.php?title=Command_line_mode).

---

## Merge multiple entities into one

**Start:** two or more clouds in the DB tree, all aligned.

**Steps:**

1. Select all (Ctrl+A in the DB tree).
2. `Edit > Merge`. One combined cloud replaces the inputs.

**Gotchas:**

- **Scalar fields are lost on merge** unless they all have the **same
  name**. If the inputs have `Distance` and the output has `Intensity`,
  the merge will pick one and drop the rest. Rename first
  (`Edit > Scalar fields > Rename`).
- **Colours are lost on merge** unless the inputs share a colour
  source. Re-apply colours after merging.
- **The merged cloud is not editable as inputs** — there's no
  "un-merge". Save the inputs as `.bin` first.

---

## Save and reload a working session

**Start:** a working set of entities you want to come back to.

**Steps:**

1. `File > Save` (or `Ctrl+S`). Choose a `.bin` file. The whole DB
   tree — clouds, meshes, polylines, scalar fields, camera angles,
   visibility, colour mappings — is serialised to a single file.
2. To reload: `File > Open`, pick the `.bin`. The whole session
   restores in one click.

**Gotchas:**

- **`.bin` is a CloudCompare-internal format.** It is **not** an
  interchange format — other tools can't read it. Use it for working
  state, not for deliverables.
- **Large `.bin` files** scale with point count + scalar field count
  + history. A 100M-point cloud with 5 scalar fields is ~5 GB.
  Externalise (save the cloud as `.las`, the session as `.bin`
  separately) for archive.
- **`.bin` doesn't include the camera view from another monitor**.
  Multi-monitor viewports are flattened on save.
