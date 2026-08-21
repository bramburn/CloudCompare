---
title: Align scans with ICP
sidebar_label: ICP alignment
sidebar_position: 2
---

# Align scans with ICP

Fine-tune the alignment between two already-overlapping scans using the
Iterative Closest Point algorithm.

## Prerequisites

- Two point clouds, **roughly aligned** (visually overlapping within ~10°
  rotation and ~10% of the scene scale)
- Both clouds in the [DB tree](/docs/reference/db-tree)

## Steps

1. Select the cloud you want to **move** (the one whose pose is wrong).
2. `Tools > Registration > Fine registration (ICP)`.
3. In the dialog, pick the **reference** cloud (the one that stays put).
4. Set `Random sampling limit` to 50,000 for the first pass; tighten to
   500,000 once the rough alignment is good.
5. Leave `Final overlap` at 100% if the clouds are full scenes; reduce
   to 70–90% if some portion of the moving cloud has no counterpart
   (e.g. you moved the scanner and the foreground changed).
6. Click `OK`. The moving cloud's transform is updated; the DB tree
   shows the new pose.

## Gotchas

- **ICP converges to the nearest local minimum.** If the rough alignment is
  off by more than ~10°, ICP will not fix it. Use
  [point-pair alignment](/docs/cookbook/point-pairs) first.
- **`Random sampling limit` too small** produces a jittery alignment
  that gets worse with more iterations. Too large and the first
  iteration is slow.
- **Don't ICP points that are not the same thing.** ICP aligns surfaces,
  not semantics. If you ICP a winter scan against a summer scan, the
  alignment will chase the foliage, not the ground.

## Next steps

- Refine with [point-pair alignment](/docs/cookbook/point-pairs) if ICP
  doesn't converge cleanly.
- After alignment, run
  [cloud-to-cloud distance](/docs/cookbook/cloud-to-cloud-distance) to
  quantify the residual misclosure.
