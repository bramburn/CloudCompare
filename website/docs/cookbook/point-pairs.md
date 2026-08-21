---
title: Align scans with point pairs
sidebar_label: Point-pair alignment
sidebar_position: 3
---

# Align scans with point pairs

Coarsely register two unrelated scans by picking corresponding 3D
features in each cloud. Follow with ICP for fine-tuning.

## Prerequisites

- Two point clouds in the [DB tree](/docs/reference/db-tree)
- Both clouds visible in at least two viewports simultaneously

## Steps

1. Select both clouds in the DB tree.
2. `Tools > Registration > Align (point pairs)`.
3. Click a feature on the **reference** cloud (left viewport). The
   dialog captures the 3D pick.
4. Click the **same feature** on the moving cloud (right viewport).
5. Repeat for 3–4 point pairs. The more spread out they are, the
   better the alignment.
6. Click `Align`. A preview transform is shown.
7. `Apply` if it looks right, then use
   [ICP](/docs/cookbook/icp) to refine.

## Gotchas

- **Pick features, not the nearest scan point.** A chimney corner, a
  fence post, the centre of a manhole cover. If you can't see the
  same feature in both clouds, you can't align them.
- **Pairs should not be collinear.** Four pairs on a line is worse
  than three pairs forming a triangle.
- **One bad pair breaks the alignment.** If the dialog highlights a
  pair as an outlier, delete it and re-pick.

## Next steps

- Run [ICP](/docs/cookbook/icp) to refine the coarse alignment.
- Validate with [cloud-to-cloud distance](/docs/cookbook/cloud-to-cloud-distance).
