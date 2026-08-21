---
title: Compute cloud-to-mesh distance
sidebar_label: Cloud-to-mesh distance
sidebar_position: 5
---

# Compute cloud-to-mesh distance

Measure the signed per-point distance from a cloud to a mesh surface.
The result is written as a scalar field on the cloud.

## Prerequisites

- A point cloud in the [DB tree](/docs/reference/db-tree)
- A triangulated mesh (design model, prior scan converted to mesh, or
  reconstructed surface)

## Steps

1. Select the cloud, then the mesh (mesh selection order comes second).
2. `Tools > Distances > Cloud-Mesh distance` (built-in) or
   `Plugin > M3C2` if both surfaces are noisy.
3. The cloud gets a new scalar field. Right click → `Color by SF`.

## Sign convention

| Distance | Meaning |
|---|---|
| Positive | Cloud point is **outside** the mesh |
| Negative | Cloud point is **inside** the mesh |

This is the standard convention for as-built vs design workflows where
"built in" reads as inside the design envelope.

## Gotchas

- **Open meshes give wrong distances at the edges** because the nearest
  mesh face can be on the other side. Close the mesh first
  (`Edit > Mesh > Close holes`) or crop the cloud to a safe
  buffer inside the mesh boundary.
- **Use M3C2** for noisy real-world scans; the built-in tool
  projects each point orthogonally to the closest mesh face and is
  sensitive to mesh holes and mesh triangulation quality.

## Next steps

- [Crop](/docs/cookbook/crop) to the mesh boundary before computing
  to avoid edge artifacts.
