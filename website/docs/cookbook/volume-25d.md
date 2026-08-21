---
title: Compute 2.5D volume
sidebar_label: 2.5D volume
sidebar_position: 9
---

# Compute 2.5D volume

Calculate the volume between a point cloud surface and a reference
plane. Requires the cloud to have at most one point per `(x, y)` cell
(a rasterised surface / DEM).

## Prerequisites

- A 2.5D point cloud (DEM, topographic surface, rasterised stockpile)

## Steps

1. Select the cloud.
2. `Tools > Volume computation > 2.5D Volume`.
3. Pick the ground / reference as the base, or let the tool
   compute the lowest point as the reference.
4. The result includes cut volume, fill volume, and total.

## True 3D piles

If your data has no clean `(x, y)` raster (a true 3D pile):

1. `Plugin > Rasterize` to a regular grid first.
2. Run the 2.5D volume tool on the rasterised cloud.

For full 3D meshes, use `Tools > Volume computation > Mesh volume`
(closed mesh required).

## Next steps

- Compare against design volume for cut/fill reporting.
- Export the rasterised cloud as GeoTIFF for GIS software.
