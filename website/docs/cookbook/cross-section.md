---
title: Extract a cross-section profile
sidebar_label: Cross-section profile
sidebar_position: 8
---

# Extract a cross-section profile

Generate a 2D polyline from a cloud by projecting onto a plane.

## Prerequisites

- A point cloud in the [DB tree](/docs/reference/db-tree)

## Steps

1. `Tools > Polyline` (or `Shift+P`).
2. Click points along the desired section line in a viewport. Press
   `Enter` to finish.
3. The polyline becomes a child of the cloud in the DB tree.
4. Right click the polyline → `Export to ASCII` (or `.dxf`) for CAD.
5. For a **height profile**, select the polyline and use
   `Tools > Projection > Project cloud / mesh along a polyline`.

## Gotchas

- **Use the side or top viewport** for clean straight sections.
  The 3D perspective view gives wobbly polylines.
- For a **cross-section view** of a stockpile or trench, draw two
  parallel polylines and use
  `Tools > Projection > Extract points between two polylines`.
