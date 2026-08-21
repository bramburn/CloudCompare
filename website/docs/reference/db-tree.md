---
title: DB tree
sidebar_label: DB tree
sidebar_position: 5
description: The hierarchical object tree is the central UI concept of CloudCompare. This page is the model, the operations, and the gotchas.
---

# DB tree

The DB tree is the hierarchical object tree in the left panel of
CloudCompare. **It is the central UI concept of the app** — every point
cloud, mesh, polyline, sensor, and group lives as a node in the tree,
and almost every action is a "select node, then do thing" pattern.

This page documents the model, the operations, and the gotchas. For
end-to-end pipelines, see the [workflows](/docs/workflows). For task
recipes, see the [cookbook](/docs/cookbook).

## The model

A DB tree is a **forest of trees**. Each top-level entry is an
independent document, but you can also group multiple files into a
single "session" tree.

A node can be:

| Type | Icon | What it is |
|---|---|---|
| Folder | 📁 | A grouping node. Doesn't contain geometry. |
| Point cloud | ☁️ | A `ccPointCloud`. The most common type. |
| Mesh | 🔷 | A `ccMesh`. Triangulated surface. |
| Polyline | 〰️ | A `ccPolyline`. Open or closed 2D/3D line. |
| Sensor | 📷 | A `ccSensor`. Camera or scanner metadata. |
| Primitive | 🟢 | A `ccSphere`, `ccCylinder`, `ccPlane`, etc. |
| Group | 📁 | A container with display properties that propagate to children. |

## Node properties (right panel)

Selecting a node fills the right panel with its properties. The
sections you'll touch most:

- **Properties** — the entity's intrinsic metadata: point count, name,
  colour, visibility toggle, bounding box, coordinate system if any.
- **Scalar fields** — every scalar field attached to the entity. A
  point cloud can have many: `Intensity`, `Distance`, `Classification`,
  custom fields from import. Use this panel to add, remove, rename,
  or change the current active field.
- **Colors** — RGBA per-vertex (if any). Usually empty until you
  import a coloured cloud or assign a colour ramp.
- **Normals** — per-vertex normals. Re-computed by
  `Edit > Normals > Compute`.
- **Octree** — the cloud's spatial index. Recompute after cropping or
  subsampling.
- **Waveform** — only for full-waveform lidar data.

## Visibility and selection

Two independent toggles, both on the node's left edge:

- **👁 Visibility** — shows or hides the node in the viewport.
  Independent of selection.
- **☑ Selection** — selects the node for the next operation. Multiple
  selection with `Ctrl+Click` or `Shift+Click`.

Visibility propagates **down the tree** by default — hide a folder,
all children hide. Selection does **not** propagate.

## Group operations

Right-click a multi-selection:

- **Merge** — combine multiple clouds into one. Loses scalar field
  names that don't match. See
  [Merge multiple entities](/docs/cookbook/merge).
- **Clone** — duplicate the entity. Use before any destructive
  operation (crop, subsample, delete).
- **Delete** — remove the entity from the tree. **Not undoable** after
  save.
- **Export to…** — write the entity to a file. Format chosen per
  type: clouds → `.las`/`.ply`/`.e57`/…; meshes → `.obj`/`.ply`/`.stl`/…;
  polylines → `.dxf`/`.shp`.

## Scalar fields, in detail

Every numeric value attached to a point is a scalar field. The most
common sources:

| Source | Scalar field name |
|---|---|
| LAS import | `Intensity`, `Classification`, `ReturnNumber`, `NumberOfReturns`, `ScanAngleRank`, custom per-VLR |
| E57 import | `Intensity`, `Color.Red`/`Color.Green`/`Color.Blue`, per-scan index |
| ICP | `Distance` (registration residual) |
| M3C2 | `Distance`, `Npoints (M3C2)`, `Sigma M3C2` |
| CSF | `Classification` (ground / non-ground) |
| RANSAC | `Classification` (inlier / outlier) |
| Custom filter | Whatever you name it |

Only **one** scalar field is the "active" one at a time — that's the
one shown in the colour-mapped view, the one used by
`Display > Color by SF`, the one exported as a LAS extra dim.

To change the active field: right click → `Scalar fields > Set as
active`. The current colour mapping follows the active field.

## Octree

Every `ccPointCloud` has an associated octree — a recursive spatial
subdivision that makes distance queries, neighbour lookups, and
spatial subsampling O(log n) instead of O(n). The octree is built
lazily on first use and cached.

The octree goes stale when you mutate the cloud (crop, merge,
subsample, edit individual points). Force a rebuild with right click
→ `Octree > Rebuild`.

For 100M+ point clouds, the octree is the difference between a
responsive UI and a frozen one. If the viewport stutters, check
whether the octree is up to date.

## Gotchas

- **Save to `.bin` to round-trip the whole tree.** See
  [Save and reload](/docs/cookbook/save-session).
  Other formats only export a single entity, not the whole tree.
- **Multi-monitor viewports flatten on save.** A `.bin` saves the
  geometry and the scalar fields but not the per-monitor camera setup.
- **Group-level visibility ≠ group-level selection.** Hiding a folder
  hides children; selecting a folder does not select its children.
- **Renaming a scalar field** doesn't rename its source. If you
  imported `Intensity` and renamed it to `MyIntensity`, the original
  `Intensity` will re-appear on re-import.
- **The DB tree is not a database.** It's a UI model. There's no
  query language, no transactions, no undo beyond the per-action
  "are you sure?" prompt. The persistent state is the files on disk.
