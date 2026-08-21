---
title: Merge multiple entities
sidebar_label: Merge entities
sidebar_position: 14
---

# Merge multiple entities

Combine two or more aligned clouds into a single cloud.

## Prerequisites

- Two or more point clouds in the [DB tree](/docs/reference/db-tree)
- All clouds must be in the same coordinate system (already registered)

## Steps

1. Select all clouds (Ctrl+A in the DB tree, or Shift+click).
2. `Edit > Merge`. One combined cloud replaces the inputs.

## Gotchas

- **Scalar fields are lost on merge** unless they all have the **same
  name**. If the inputs have `Distance` and `Intensity`, the merge
  picks one and drops the rest. Rename first
  (`Edit > Scalar fields > Rename`) so both clouds share the same SF
  name before merging.
- **Colours are lost on merge** unless the inputs share a colour
  source. Re-apply colours after merging.
- **The merged cloud is not recoverable as inputs.** There is no
  un-merge. Save the inputs as `.bin` before merging.
- **Always subsample both clouds to the same density** before merging
  for M3C2 — otherwise the M3C2 normal estimation is asymmetric.
