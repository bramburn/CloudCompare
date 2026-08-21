---
title: Subsample a cloud
sidebar_label: Subsample a cloud
sidebar_position: 7
---

# Subsample a cloud

Reduce point density while preserving the most important geometry.

## Prerequisites

- A point cloud in the [DB tree](/docs/reference/db-tree)

## Three modes

| Mode | Menu path | Best for |
|---|---|---|
| **Random** | `Edit > Subsample > Random` | Quick previews; preserves nothing |
| **Spatial** | `Edit > Subsample > Space` | Pre-M3C2 preparation; keeps one point per voxel |
| **Octree** | `Edit > Subsample > Octree` | Level-of-detail rendering |

**Spatial subsampling** is the recommended mode for monitoring and
as-built workflows. Apply the same voxel size to **both** clouds so
the M3C2 normal estimation is symmetric.

## Recommended voxel size

2–5× the median point spacing. Estimate spacing with
`Tools > Statistics` on a representative cloud.

## Gotchas

- **Random subsampling destroys spatial information.** Never use it
  as a precursor to M3C2 or any measurement tool.
- **Both clouds must use the same voxel size** for M3C2 to give
  symmetric results.
