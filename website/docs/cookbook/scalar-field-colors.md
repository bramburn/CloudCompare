---
title: Change scalar field colours
sidebar_label: Scalar field colours
sidebar_position: 10
---

# Change scalar field colours

Adjust how scalar field values map to colours to make deviations
readable and reports clear.

## Prerequisites

- A point cloud with at least one scalar field (any `Distance`,
  `Intensity`, classification, etc.)

## Steps

1. Right click the cloud → `Display > Color by SF`. The default
   mapping is a uniform blue-to-red ramp.
2. Right click again → `Scalar fields > Display parameters`:
   - **Saturation range** — drop the bottom 1% and top 1% to
     ignore outliers. This is the single most useful change.
   - **Colour ramp** — pick a **diverging** scale (blue → white →
     red) for signed fields like M3C2 distance. A sequential
     scale hides the zero crossing.
   - **Logarithmic scale** — for power-distribution fields
     (intensity, log-distances) where most data is at the low end.
3. The colour mapping is per-entity and persists in the `.bin`
   file when you save.

## Scale selection guide

| Field type | Recommended scale |
|---|---|
| Signed distance (M3C2) | Diverging (blue–white–red) |
| Unsigned distance | Sequential (blue–green–red) |
| Intensity | Grayscale or sequential |
| Classification code | Discrete categorical |
| Curvature | Diverging |

## Next steps

- Export a screenshot at fixed DPI using
  [Render to file](/docs/cookbook/screenshot).
