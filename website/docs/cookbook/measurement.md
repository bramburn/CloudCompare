---
title: Take a measurement
sidebar_label: Measurements
sidebar_position: 11
---

# Take a measurement

Measure distances and angles directly in the 3D viewport.

## Prerequisites

- A point cloud in the [DB tree](/docs/reference/db-tree)
- Camera positioned to see the measurement targets

## Three measurement tools (all under `Tools > Measurement`)

### Point to point

Click two points to get the Euclidean distance. Use for clearance
checks, beam lengths, anything where the endpoints are visible.

### Point to plane

Pick a point, then pick a plane (or a mesh face) to get the
perpendicular distance. Use for flatness checks, offset
measurements.

### Angle

Pick three points (vertex at the middle) to get the 3D angle.
Use for corner and fold measurements.

## After measuring

The measurement becomes a labelled entity in the DB tree. Right click
→ `Export to DXF` or `Export to ASCII` for the report.

## Next steps

- [Render to file](/docs/cookbook/screenshot) to capture the
  annotated viewport with measurements visible.
- Use [point-to-plane distance](/docs/cookbook/cloud-to-mesh-distance) for
  surface-to-surface comparisons.
