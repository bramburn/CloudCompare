---
title: Compute cloud-to-cloud distance
sidebar_label: Cloud-to-cloud distance
sidebar_position: 4
---

# Compute cloud-to-cloud distance

Measure the per-point nearest-neighbour distance between two
overlapping clouds. The result is written as a scalar field on
the compared cloud.

## Prerequisites

- Two roughly-aligned point clouds in the [DB tree](/docs/reference/db-tree)
- Similar point density (resample first if they differ significantly)

## Steps

1. Select the **reference** cloud first, then the **compared** cloud
   (selection order matters — the second cloud gets the scalar field).
2. `Tools > Distances > Cloud-Cloud distance` (built-in) or
   `Plugin > M3C2` for noisy real-world scans.
3. The compared cloud gets a new scalar field called `Distance`. Right
   click → `Display > Color by SF` to see the heatmap.
4. `Edit > Scalar fields > Filter by value` to drop noise-floor
   points.

## Built-in vs M3C2

| | Built-in C2C | M3C2 plugin |
|---|---|---|
| Speed | Fast | Slower |
| Robustness | Sensitive to noise | Robust multiscale normal-projected |
| Confidence SF | No | Yes (uncertainty SF) |
| Use for | Clean data, registration error analysis | Monitoring, as-built workflows |

## Next steps

- [Change scalar field colours](/docs/cookbook/scalar-field-colors) to
  make deviations visible.
- Export the distance SF as CSV for further analysis.
