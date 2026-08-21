---
title: Save and reload a working session
sidebar_label: Save / reload session
sidebar_position: 15
---

# Save and reload a working session

Save the entire state of the DB tree — clouds, meshes, scalar
fields, camera angles, visibility, colour mappings — and restore
it in one click.

## Prerequisites

- A working set of entities you want to restore later

## Steps

1. `File > Save` (or `Ctrl+S`). Choose a `.bin` file.
2. To reload: `File > Open`, pick the `.bin`. The whole session
   restores in one click.

## What is saved

- All entities in the DB tree (clouds, meshes, polylines, sensors, etc.)
- Scalar field values and names
- Per-entity colour mappings
- Global coordinate shift
- Camera angles and zoom
- Visibility and lock state

## What is NOT saved

- `.bin` is a CloudCompare-internal format — other software
  cannot read it.
- Multi-monitor viewport positions are flattened on save.
- Camera views from a second monitor are not restored.

## File size

A 100M-point cloud with 5 scalar fields produces a ~5 GB `.bin`
file. For archive purposes, save the cloud as `.las` and the
session as `.bin` separately.

## Next steps

- [Batch export](/docs/cookbook/batch-cli) deliverables from the saved
  session without reopening the GUI.
