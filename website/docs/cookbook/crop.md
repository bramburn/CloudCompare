---
title: Crop a cloud
sidebar_label: Crop a region
sidebar_position: 6
---

# Crop a cloud

Trim a point cloud to a bounding box or a polygon boundary.

:::warning Crop is destructive
The original points are removed. Always work on a copy.
:::

## Prerequisites

- A point cloud in the [DB tree](/docs/reference/db-tree)

## Bounding box

1. Select the cloud.
2. `Edit > Crop`.
3. Drag the box handles in the viewport, or enter min/max
   coordinates by hand.
4. `OK`. The original is replaced by the cropped cloud.

## Polygon

1. `Tools > Polyline` (or `Shift+P`) to draw a closed polygon on a
   viewport.
2. `Edit > Crop` and switch the dialog to **Inside / outside
   polyline**.

## Gotchas

- **Crop is destructive.** There is no undo after you save the file.
  Always work on a copy: right-click the cloud → `Clone` before
  cropping.
- **For M3C2 / monitoring**, crop both clouds to the **same**
  polygon. If you crop them to different regions, the distances are
  computed across the union, not the intersection.
- Use the **side or top viewport** for clean straight crop lines;
  the 3D perspective view gives irregular boundaries.
