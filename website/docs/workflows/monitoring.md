---
title: Deformation monitoring
sidebar_label: Monitoring
sidebar_position: 3
description: Two-epoch comparison for structures, slopes, and retaining walls — register, M3C2, colour-map, report.
---

# Deformation monitoring

The "two scans, two epochs, one report" workflow. You take an
[epoch-0 scan](/docs/workflows/topo) (the baseline) and an epoch-N scan
(same site, weeks or months later), align them, compute the per-point
distance between the two surfaces, and colour-map the deltas. The output
is a heatmap that the client (or the engineer) reads to decide whether
the wall is moving, the slope is creeping, or the tank is settling.

## Pipeline

1. **Open both epochs** as two separate entries in the
   [DB tree](/docs/reference/db-tree). Keep them named `epoch-0` and
   `epoch-N` so the rest of the pipeline is self-documenting.
2. **Crop to the area of interest** — M3C2 is slow on full scans. Use
   `Edit > Crop` or `Tools > Segment` to bound both clouds to the
   structure or slope. See
   [Crop a region](/docs/cookbook#crop-a-region-to-a-bounding-box-or-polygon).
3. **Rough align** — pick three or more reference points that didn't move
   between the two epochs (a fixed benchmark, a piece of unmovable
   hardware). Use `Tools > Registration > Align (point pairs)` to seed
   the alignment.
4. **Fine align (ICP)** — `Tools > Registration > Fine registration (ICP)`.
   The default settings are fine; if alignment is poor, increase the
   `random sampling limit` and re-run.
5. **M3C2 distance** — `Plugin > M3C2 (Multiscale Cloud-to-Cloud
   Distance)`. Defaults work for most cases; if the surfaces are very
   rough, increase the `normal scale` so the normals reflect the local
   geometry, not the per-point noise.
6. **Filter** — most monitoring use-cases want a confidence interval.
   Apply a `filter by scalar value` to drop points below the M3C2
   `lodetection` threshold (points where the surface is too noisy to
   trust the distance).
7. **Colour-map** — `Edit > Scalar fields > Export to colour scale` (or
   the right-click `Display > Color by SF`). Use a **diverging**
   colour-scale centred on zero (blue → white → red), not a sequential
   one. The zero point is "no change" and must be the visual midpoint.
8. **Screenshot + report** — `Display > Render to file` at 4K for the
   client-facing figure. Annotate in your report tool of choice
   (Inkscape, PowerPoint). The fork does not auto-generate a PDF report.

## Which plugins you actually touch

| Step | Plugin | Notes |
|---|---|---|
| ICP fine registration | built-in | No plugin |
| RANSAC shape detection | [`qRANSAC_SD`](/docs/plugins/local-set) | Optional — for cleaning planes out before M3C2 |
| M3C2 distance | [`qM3C2`](/docs/plugins/local-set) | Self-contained |
| Multi-epoch alignment | built-in | No plugin |

## Gotchas

- **Temperature / season change** can produce a uniform "movement" across
  the whole structure (thermal expansion, ground freeze-thaw). Always
  report the date, time, and weather of both epochs alongside the
  deltas, so the engineer can sanity-check.
- **The diverging colour-scale midpoint is not optional**. Blue
  (negative) means the surface is closer to the scanner at epoch N than
  at epoch 0; red means further. A sequential colour scale hides which
  side of zero the movement is on.
- **`lodetection` is your friend** — without it, M3C2 reports a distance
  even for points where the local surface is too rough to give a
  reliable normal. The default `0.5×normal_scale` is a reasonable start.
- **Don't subsample the two epochs by different amounts**. If epoch 0
  is at 5 mm and epoch N is at 50 mm, M3C2 will systematically bias
  toward epoch 0 because of the higher point density. Decimate both
  to the same spacing before alignment.
- **M3C2 reports a signed distance, not absolute movement**. A point
  with a +5 mm reading is 5 mm further from the scanner, not
  necessarily 5 mm of "real" movement. The orientation of the normal
  matters — M3C2 computes it locally.
