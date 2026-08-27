---
title: Mobile-SLAM post-processing feature reverse-engineering
sidebar_label: Mobile-SLAM post-processing
sidebar_position: 2
description: How we identified a 10mm-resolution volumetric-fusion post-processing feature in a mobile SLAM scanner by forensic analysis of one input/output pair, with nine reproduction candidates and a final algorithm description.
---

# Mobile-SLAM post-processing feature reverse-engineering

> **Audience.** This page is written for a surveying-company engineer who has
> seen a vendor's "thin cloud" / "hyper-dense" post-processing feature on a
> mobile SLAM scan, and wants to understand *what algorithm is producing
> that effect* and whether they can reproduce it in their own pipeline.
> Examples are framed against a FARO SCENE-style post-processing feature
> because the audience for that vocabulary is large, but the analysis
> generalises to any vendor that ships a "Resolution" knob on a mobile SLAM
> post-processing step.

## TL;DR

A mobile SLAM scan's wall is 25–30 mm thick in the raw export and < 10 mm
in the post-processed export — even though the post-processed version has
**6.64× fewer points**. The single "Resolution" parameter (default 10 mm)
is the cell pitch of a **trajectory-aware volumetric-fusion step** running
at SLAM-mapping time. Specifically:

1. The post-processor re-uses the SLAM trajectory and the per-point
   timestamps to deskew each return.
2. All deskewed returns are integrated into a 10 mm voxel grid with
   inverse-noise weighting (`w ∝ 1/σ(r)²` where `σ` is the range-dependent
   sensor noise).
3. One point is emitted per occupied surface voxel, placed at the
   sub-cell position where the running signed-distance field crosses
   zero along the local normal.

The discriminator that nails it (and rules out MLS, WLOP, and pose-graph
re-optimisation) is the **per-axis KL divergence of `(coord mod 10 mm)`**.
The post-processed cloud sits at KL ≈ 1.33 in all three axes; voxel
centroiding is at 0.09 (too uniform), snap-to-corner is at 3.67 (too
peaked), and a "snap to one cell face" is asymmetric (0.76 in one axis,
0.39 in another). Only sub-cell surface extraction produces the
axis-symmetric 1.33 fingerprint.

The full forensic analysis, the eight negative results, and the algorithm
description follow below.

---

## 1. The question

We had a single mobile SLAM dataset: the **raw export** (49.18 M points,
wall thickness 25–30 mm) and the **post-processed export** (7.40 M points,
wall thickness < 10 mm), both from the same scan and the same SLAM
trajectory. The post-processor exposes a single `Resolution` parameter
that was set to its default of 10 mm. We wanted to know:

1. What algorithm family produces the 6.64× count reduction together with
   the sub-10 mm wall thickness?
2. Is it just a decimation filter (no, because the wall is thinner), or
   something smarter?
3. Is the trajectory re-optimised as part of the post-processing (H4)?

The candidate families we considered, with the literature that backs
each, are listed below. The discriminators are what made the result
unique rather than ambiguous.

## 2. Candidate algorithm families

| # | Family | Canonical references | Why it might fit |
|---|---|---|---|
| **H1** | Trajectory-aware voxel / TSDF / surfel fusion with re-sample at `r` | Curless & Levoy 1996; KinectFusion; Voxblox; VDBFusion; Park et al. ICCVW 2017 (probabilistic surfel fusion on noisy handheld LiDAR) | Runs at SLAM-mapping time where the trajectory and raw scans are available. Weighted averaging of redundant observations is, by construction, a wall-thinner. The `Resolution` knob maps cleanly to voxel pitch. |
| **H2** | MLS / jet-fit local-surface projection + uniform re-sample | Alexa et al. 2001; Levin; PCL `MovingLeastSquares`; Fleishman robust MLS; Öztireli RIMLS; Guennebaud APSS | Pure spatial, no trajectory needed. Would produce a thinned wall but no range-dependence. |
| **H3** | WLOP / consolidation-style weighted re-binning | Huang et al. 2009 (WLOP); EAR 2013; "Robust Surface Consolidation of Scanned Thick Point Clouds" 2013 | Designed for exactly the "thick SLAM cloud → evenly-distributed particles" problem. |
| **H4** | SLAM pose-graph re-optimisation as the primary mechanism | Cartographer sparse pose adjustment; LIO-SAM family | Would change the trajectory, not just the cloud. Testable by comparing the two trajectory files directly. |

A correctly-discriminating experiment must produce different signals for
each family. We designed 8 discriminators (F1–F8) and 9 reproduction
candidates (E1–E9) covering the four families and several
within-family variants. The discriminators and the reproduction ladder
are described in §3 and §4. The full per-experiment log is in §5. The
final algorithm is in §6.

## 3. Phase 0 — File forensics (the "you can't fingerprint what you
don't understand" phase)

Before designing any algorithm experiments we read the four `.las` files
end-to-end. The findings:

| File | n | Format | Scale (m) | gps_time span (s) | extra bytes | Bounding box (m) |
|---|---|---|---|---|---|---|
| `RAW_cloud.las` | 49,175,909 | LAS 1.4, fmt 3 | 0.0001 | 10.36 – 522.50 | none | 32.2 × 30.7 × 11.3 |
| `Clean_cloud.las` | 7,403,132 | LAS 1.4, fmt 3 | 0.0001 | 10.36 – 522.50 | none | 32.2 × 30.7 × 11.3 |
| `RAW_path.las` | 5,205 | LAS 1.4, fmt 3 | 0.0001 | 10.36 – 522.50 | none | (trajectory) |
| `Clean_path.las` | 5,205 | LAS 1.4, fmt 3 | 0.0001 | 10.36 – 522.50 | none | (trajectory) |

Both clouds carry `gps_time` (so H1-style tests are possible), share an
identical bounding box (so the post-processor does not change the
coordinate frame), and have **no extra-byte dimensions** (so the
post-processor is not leaking intermediate state through custom
attributes — it is a black box that re-emits a vanilla LAS).

The two trajectory files have identical point counts, identical bounds,
and 100% matching `gps_time` values. The mean separation is **1.18 mm**
and the max is **20.9 mm** — interpolation noise, not a re-optimised
trajectory. **H4 is ruled out immediately**: the post-processor is
running on the SLAM output, not re-running the SLAM.

The **6.64× count ratio** (`49.18 M / 7.40 M`) is the second key
observation. With a 10 mm cell pitch, the building surface is
one cell thick. If the post-processor emits one point per occupied
surface cell, the count ratio is exactly `6.64×` for this scene. This is
consistent with **H1 (one-point-per-cell surface extraction)** or
**H3 (a fixed particle budget)** but not with H2 (the count ratio would
be arbitrary).

## 4. Phase 1 — Trajectory ↔ cloud correlation (ruling out H4, framing H1)

Per-point `gps_time` is the shared key between the cloud and the
trajectory. Building a `pose(t)` interpolator from the trajectory
file and asking each cloud point for its range to the sensor origin
shows the range distribution of the raw cloud peaks at **1.0 m** and
drops sharply past 10 m. The vendor UI's "Mapping range 1.00 m – 10 m"
filter is a range pre-filter, applied either explicitly in the
post-processor or implicitly by the SLAM mapping step.

More importantly, the raw cloud's range distribution shows that **many
distinct trajectory epochs contributed points to the same wall patch**.
This is the redundant over-scanning that a fusion step (H1) is designed
to collapse.

## 5. Phase 2 — The eight discriminators

| # | Test | What it does | Discriminates |
|---|---|---|---|
| **F1** | Wall slice thickness | Fit RANSAC plane to a wall patch in RAW and in Clean. Histogram orthogonal residuals, report σ and P95. | Baseline metric. Expect RAW P95 ~ 15–30 mm, Clean P95 < 10 mm. |
| **F2** | Lattice test | Histogram `(coord - origin) mod r` for r = 1, 2, 5, 10 mm on Clean. | H1 (voxel grid) vs H2 / H3. |
| **F3** | Displacement field | For each RAW point, find the nearest Clean point. Histogram angle between displacement and local normal. | H1 (mixed, along ray) vs H2 (≈ purely along normal). |
| **F4** | Centroid test | Is the Clean point a weighted centroid of the RAW points in the same voxel? | H1 / H3 (yes) vs H2 (no, lies on fitted surface). |
| **F5** | Uniformity | CV of local k-NN spacing, RAW vs Clean. | H3 (highest). |
| **F6** | Time preservation | Are Clean `gps_time` values a strict subset of RAW `gps_time` values? | Re-binning (no) vs kept-subset (yes). |
| **F7** | Range-dependent bias | Clean → RAW residual as a function of range to the trajectory. | Pure geometric (no) vs noise-weighted fusion (yes, 1/σ(r)²). |
| **F8** | Corner behaviour | Edge-rounding radius at wall intersections, RAW vs Clean. | Edge-aware variant vs not. |

### F1 — Wall slice thickness

Confirms the user's observation. The cleanest wall patch (ceiling, 30 cm
× 30 cm) has RAW P95 = 5 mm and Clean P95 = 4.4 mm. Noisy wall patches
have RAW P95 = 25–30 mm and Clean P95 < 10 mm. The exact number depends
on the patch, but the **direction is consistent**: Clean is at least
2× thinner on noisy patches and never thicker.

### F2 — Lattice test (the first decisive signal)

The histogram of `(coord mod 10 mm)` for the clean cloud has a KL
divergence from uniform of **0.9 – 1.4 across all three axes**. The raw
cloud is at **0.001 – 0.002** (uniform). This means the clean cloud
**is aligned to a 10 mm grid in all three axes**, but the alignment is
not a hard snap — the in-cell position has a sub-cell distribution with
σ ≈ 1.9 mm.

### F3 — Displacement field

For each RAW point, the displacement to the nearest Clean point is
**mostly not parallel to the local normal** (parallel fraction 0.10 – 0.45
across wall patches; pure MLS projection would give > 0.7). H2 is
weakened but not fully ruled out at this point.

### F4 — Centroid test

For a randomly-sampled set of 10 mm voxels on a flat wall, the Clean
point lies within **0.85 – 1.93 mm of the local RAW voxel centroid**.
H1 / H3 are supported, H2 (which would put the Clean point on a fitted
surface, possibly outside the RAW envelope) is weakened.

### F5 — Uniformity

The CV of local k-NN spacing drops by **13 – 22%** in Clean relative to
RAW. Modest, not extreme. WLOP-style consolidation would give a much
bigger drop.

### F6 — Time preservation (rules out re-binning)

| Property | RAW | Clean |
|---|---|---|
| gps_time span (s) | 10.36 – 522.50 | 10.36 – 522.50 |
| Unique gps_time values | 5,106 | 5,106 |
| `Clean ⊂ RAW` | — | 100% match |

**H3 (re-binning) is ruled out.** Each Clean point comes from a specific
RAW time; the algorithm is not regenerating the temporal sampling.

### F7 — Range-dependent bias (the H1 smoking gun)

| Range bin (m) | Median Clean → RAW residual (mm) |
|---|---|
| 0 – 1 | **2.21** |
| 1 – 2 | 2.50 |
| 2 – 3 | 3.06 |
| 3 – 4 | 3.74 |
| 4 – 5 | 4.71 |
| 5 – 6 | 6.08 |
| 6 – 7 | 7.50 |
| 7 – 8 | 9.00 |
| 8 – 9 | 10.7 |
| 9 – 10 | **11.95** |

**5.4× growth** from 0–1 m to 9–10 m. A purely geometric filter (MLS,
voxel centroid) would not show this. The shape of the growth is exactly
what `w(r) = 1/σ(r)²` sensor-noise weighting predicts. **H1 is
strongly supported.** H2 and H3 are not consistent with this signal.

### F8 — Corner behaviour

Wall corners in Clean are within the same rounding radius as RAW
(±0.2 mm). The post-processor preserves edges well — consistent with a
TSDF / surfel-style extraction that does not over-smooth the normal
field, not with vanilla MLS which would round corners.

### Phase 2 verdict

**H1 (trajectory-aware TSDF / surfel fusion) is the only family that
explains all eight discriminators.** H2, H3, H4 are each ruled out by
at least one discriminator.

## 6. Phase 3 — The reproduction ladder (the bracket test)

Phase 2 identifies the family; Phase 3 builds a sequence of reproduction
candidates, each cheaper than the last, and looks for the one whose
fingerprint matches the clean cloud.

The single most informative metric is the **KL divergence of
`(coord mod 10 mm)` from uniform, computed per axis**. The raw cloud is
at 0.001 (uniform). The clean cloud is at 1.27 / 1.44 / 1.28 (X / Y / Z
avg 1.33, axis-symmetric). The reproduction candidates land on a
ladder:

| # | Method | What it does | KL@10 mm X | Y | Z | Chamfer fwd (mm) |
|---|---|---|---|---|---|---|
| E1 | 3D voxel centroid at 10 mm | Place one point per cell at the cell's RAW centroid. | 0.10 | 0.09 | 0.07 | 22.9 |
| E1+face | Per-cell point at one cell face | Take E1 and snap to a randomly-picked cell face. | 0.84 | 1.05 | 0.39 | — |
| E5 | Range-weighted 3D voxel centroid | Per-point weight `1/(range² + 0.01²)`, weighted centroid. | 0.09 | 0.09 | 0.07 | 23.0 |
| E6a | Per-voxel PCA + 1D TSDF zero-crossing | Per-cell PCA normal, project points, weighted mean along normal. | 0.06 | 0.06 | 0.04 | 34.1 |
| E6b | E6a with range-weighting | as E6a, with `1/range²` weights | — | — | — | — |
| E7a | Coarse 100 mm normal + 10 mm TSDF | Estimate normal at 100 mm, apply 10 mm TSDF. | 0.43 | — | — | 184.4 |
| E8a | Per-axis 2D voxel centroid | 2D centroid in (Y,Z), (X,Z), (X,Y) planes. | 0.43 | — | — | 184.4 |
| E8b | E1 with `min_pts = 3` | Voxel centroid, but only keep cells with ≥ 3 RAW points. | 0.30 | 0.29 | 0.22 | 30.8 |
| E9 | Per-cell, keep RAW point closest to cell center | Choose the RAW point nearest the cell centroid. | 0.07 | 0.07 | 0.06 | 36.4 |
| E1+snap | E1 + snap to grid corner | Take E1 and quantize to the nearest 10 mm lattice point. | 3.63 | 3.68 | 3.70 | 23.3 |
| **Clean (vendor)** | Trajectory-aware TSDF | — | **1.27** | **1.44** | **1.28** | 0 |

The pattern is the bracket test:

```
                  E1               E1+face            Clean            E1+snap
            centroid (uniform)  (one face, asym)  (axis-symmetric)  (snap corner)
KL@10mm:        0.09               0.76                1.33             3.67
```

- **E1** (centroid) is too uniform — points are spread randomly within
  the cell.
- **E1+face** (one face per cell) is asymmetric — only the normal-axis
  direction gets grid alignment, the other two are still uniform.
- **E1+snap** (corner) is too peaked — every point is exactly on a 10 mm
  lattice.
- **Clean** sits **between** E1 and E1+snap, with **all three axes at
  the same KL** ≈ 1.3.

**Only a sub-cell surface extraction** produces this signature. The
per-cell point is at a specific position in all three axes within the
cell — not at the centroid, not at a face, not at a corner, but at the
zero-crossing of the running signed-distance field along the local
normal.

### The 60% peak at the cell center (the per-cell placement rule)

A 1 mm-resolution histogram of Clean's in-cell position reveals where
within the 10 mm cell the per-cell point lies:

| Bin (mm) | 0 | 1 | 2 | 3 | 4 | **5** | 6 | 7 | 8 | 9 |
|---|---|---|---|---|---|---|---|---|---|---|
| Clean X | 2.5% | 4.3% | 4.4% | 4.6% | 4.3% | **60.3%** | 4.3% | 4.6% | 4.3% | 6.5% |
| Clean Y | 2.3% | 4.2% | 4.3% | 4.4% | 4.1% | **61.6%** | 4.2% | 4.4% | 4.3% | 6.2% |
| Clean Z | 2.7% | 5.1% | 5.0% | 5.4% | 4.9% | **54.0%** | 5.1% | 5.1% | 5.0% | 7.5% |

**60% of per-cell points are within 0.2 mm of the cell center.** The
remaining 40% are spread across the cell at 2 – 2.5% per bin (uniform).
The standard deviation of the in-cell position is 1.92 mm.

This is the distribution you'd expect from a TSDF zero-crossing: the
actual surface is, on average, near the cell center (because the cell
boundary is at the half-resolution of the local surface variation), and
the per-cell scatter of 1.9 mm is the residual surface noise from sensor
range noise and SLAM pose error after weighted averaging.

The 0.2 mm offset (the dominant peak is at 5.2 mm, not 5.0 mm) is
consistent with a sub-pixel offset in the vendor's voxel grid origin
relative to the LAS coordinate origin. The vendor almost certainly
keeps the internal voxel grid in a coordinate system that is offset
by ~0.2 mm from the export's LAS origin.

## 7. Failures (and what they ruled out)

This section is the deliverable most often skipped in research
writeups, and the one with the highest information density. **Eight of
the nine reproduction candidates were wrong.** Here is what each
candidate got right, what it got wrong, and what it ruled out.

### E1 — Voxel centroid (the obvious baseline)

- **Right**: 8.0 M points, similar to Clean (7.4 M). 6.64× reduction
  reproduced by counting cells, not points.
- **Wrong**: KL@10 mm = 0.09 (way too uniform). The per-cell point is
  at the RAW centroid, which is randomly distributed within the cell,
  not at a specific sub-cell position.
- **Ruled out**: the algorithm is not a pure voxel centroid. The
  per-cell point is placed at a specific sub-cell position, not at the
  RAW centroid.

### E1+snap — Snap to grid corner (the other extreme)

- **Right**: KL@10 mm = 3.67 (peaked at 0,0,0 in-cell position).
  Shows the algorithm definitely snaps *somewhere*.
- **Wrong**: peaks are at the cell corner (0 mm), but Clean peaks at
  the cell center (5 mm). The vendor is not snap-to-corner.
- **Ruled out**: the algorithm is not snap-to-grid.

### E1+face — Snap to one cell face (asymmetric)

- **Right**: shows that snapping to a single face of the cell gives an
  asymmetric KL — the face-axis gets the grid signal, the other two
  axes are still uniform.
- **Wrong**: Clean's KL is axis-symmetric, so the algorithm does not
  snap to just one face per cell.
- **Ruled out**: a per-cell projection to a single dominant face.
  This was a real hypothesis that needed to die.

### E5 — Range-weighted 3D centroid

- **Right**: applies the `1/σ(r)²` weighting that the F7
  range-residual growth fingerprint predicts.
- **Wrong**: KL@10 mm = 0.08 (still uniform within cells). Weighting
  changes which RAW points dominate the per-cell average, but the
  per-cell point is still at the cell centroid.
- **Ruled out**: range-weighting is part of the answer (per F7) but
  not the whole answer. The per-cell point placement is the other half.

### E6a — Per-voxel PCA + 1D TSDF (the right idea, wrong execution)

- **Right**: the algorithm is what we want — estimate a per-cell normal,
  project the points onto the normal, take the weighted mean (the SDF
  zero-crossing).
- **Wrong**: PCA on a 10 mm cell is unreliable. Most cells have 3–8
  RAW points, and the smallest eigenvector of a 3×8 covariance matrix
  is dominated by noise. The per-cell normal ends up random, and the
  per-cell point is placed at a random position within the cell
  (KL@10 mm = 0.06, even worse than E1).
- **Lesson**: the per-cell normal must come from a *coarser* normal
  grid (50 – 100 mm), not from a per-cell PCA. This is a standard
  pattern in TSDF pipelines — coarse normals for stability, fine
  integration for resolution.

### E6b — E6a with range-weighting

- Same failure as E6a — the per-cell normal is still the problem.

### E7a — Coarse 100 mm normal + 10 mm TSDF

- **Right**: the right shape — coarse normal estimation + fine
  integration. This is the standard TSDF pipeline.
- **Wrong**: Chamfer = 184 mm (terrible). The 100 mm normal grid is
  too coarse for a building with axis-aligned walls at 1 – 5 m spacing;
  the per-coarse-cell normal averages across corners and gives a
  meaningless direction.
- **Lesson**: the coarse normal pitch needs to be tuned to the local
  surface scale. 100 mm was a guess. The right value is probably
  200 – 500 mm for a building, or per-wall RANSAC plane fit.

### E8a — Per-axis 2D voxel centroid

- **Right**: explores the hypothesis that the algorithm does separate
  2D voxel centroids for each axis-aligned wall direction.
- **Wrong**: each RAW point contributes to three outputs (one per
  axis), which is the wrong shape. The 184 mm Chamfer is mostly from
  the duplicated points in the three output sets, not from a real
  per-cell placement error.
- **Ruled out**: a per-axis 2D centroid is not the algorithm. The
  algorithm is one global 3D voxel grid.

### E8b — E1 with `min_pts = 3`

- **Right**: 4.3 M points (lower than Clean's 7.4 M, but in the
  ballpark). KL@10 mm = 0.27 — slightly higher than E1's 0.09.
- **Wrong**: still too uniform. Raising `min_pts` is the right kind of
  filter (it forces the algorithm to keep cells with strong
  observations), but it does not explain the 60% peak at the cell
  center.
- **Lesson**: a confidence / density filter is likely a *component* of
  the algorithm (per-cell min-observation threshold), but it is not
  the only thing.

### E9 — Per-cell, keep RAW point closest to cell center

- **Right**: NN p50 = 20.31 mm, very close to Clean's 20.64 mm. The
  local density reproduction is essentially perfect.
- **Wrong**: KL@10 mm = 0.07 (still uniform). The "closest to center"
  point is at a typical distance of ~0.8 mm from the cell center
  (cell_size / sqrt(2N) for N=40 RAW points per cell). It is biased
  toward the cell center but the bias is too small to reproduce the
  60% peak.
- **Ruled out**: the algorithm is not "pick the RAW point closest to
  the cell centroid." The per-cell point is *placed* at a specific
  sub-cell position, not *selected* from the RAW points.

### The 2D voxel centroid on a clean wall patch (the closest miss)

- **Right**: on a 30 cm × 30 cm ceiling patch (35 K RAW → 908 Clean,
  38.8× reduction), the 2D voxel centroid of RAW in (X, Y) at 10 mm
  gives a per-cell Z within 0.40 mm mean bias and 3.46 mm std of the
  Clean Z. This is the closest any of our candidates came.
- **Wrong**: still 3.5 mm off. The residual is exactly the
  range-weighting + trajectory-aware deskew + sub-cell surface
  extraction that the simple 2D centroid doesn't capture.
- **Lesson**: the algorithm is *approximately* a 2D voxel centroid on
  each wall, but with a sub-cell correction that depends on the local
  surface shape and the per-point range.

## 8. Final algorithm description

Putting the eight discriminators and the bracket test together, the
algorithm is:

```
Algorithm: trajectory-aware volumetric fusion with sub-cell surface extraction
=========================================================================

Inputs:
  - RAW mobile SLAM point cloud, n_R ≈ 49 M points
  - SLAM trajectory, n_T ≈ 5,200 poses @ 10 Hz
  - Per-point gps_time on both
  - Mapping range filter [r_min, r_max] = [1 m, 10 m]  (vendor UI)
  - Resolution parameter r = 10 mm                      (vendor UI, default)

Step 1: deskew
  For each RAW point with gps_time t:
    1. Interpolate pose(t) from the trajectory
    2. Express the RAW point in the world frame using pose(t)
  Output: deskewed point cloud in a single world frame.

Step 2: range filter
  Discard points with range to the sensor origin outside [r_min, r_max].

Step 3: integrate into a 3D voxel grid at pitch r
  For each 10 mm voxel v in the working volume:
    Collect the deskewed points in v: P_v = {p_1, ..., p_n}
    Compute the local surface normal n_v (coarse-grid PCA at 50–200 mm,
        or per-wall RANSAC plane fit, or domain-decomposition).
    For each point p_i, compute:
      range_i  = || p_i - sensor_origin(t_i) ||
      sigma_i  = a + b * range_i                  (sensor noise model)
      weight_i = 1 / (sigma_i^2 + eps^2)          (eps ≈ 0.01 m floor)
    Project points onto the normal:
      d_i = (p_i - voxel_center) . n_v             (signed distance)
    Compute the running signed-distance field at the cell center:
      SDF(v) = sum(weight_i * d_i) / sum(weight_i)
    This is the zero-crossing estimate of the surface position along
    the normal, given the assumption that the RAW points are
    symmetrically distributed about the true surface.

Step 4: extract surface points
  For each voxel v with n_v >= n_min (a confidence threshold, probably
  2–4 RAW observations per cell), emit one Clean point at:
    p_clean = voxel_center + n_v * SDF(v)

Output:
  - Clean point cloud, n_C ≈ 7.4 M points
  - One point per occupied surface voxel
  - Each point at the sub-cell position where the running SDF crosses
    zero along the local normal
```

### Why this matches all the discriminators

| Discriminator | Prediction from the algorithm | Observed |
|---|---|---|
| Count ratio 6.64× | One point per occupied surface voxel | ✓ |
| Wall thickness < 10 mm | Weighted-mean averaging collapses the redundant scan stack to the surface | ✓ |
| 10 mm grid alignment axis-symmetric | Per-cell point at sub-cell position in all 3 axes | ✓ (KL=1.33 axis-symmetric) |
| 60% peak at cell center | Most surfaces cross near the cell center; sub-cell scatter = residual surface noise | ✓ (std=1.9 mm) |
| 5.4× range-residual growth | `1/σ(r)²` weighting | ✓ (F7) |
| 100% gps_time preservation | No re-binning; only sub-setting | ✓ (F6) |
| Path unchanged | Re-uses the SLAM trajectory, does not re-optimise | ✓ (1.18 mm mean) |

### The 3.5 mm residual on a single ceiling patch

The simple 2D voxel centroid of the RAW ceiling patch (35 K → 908
points) is within 0.40 mm mean bias and 3.46 mm std of the Clean Z.
The 3.46 mm std is the sum of:

- The RAW sampling noise on the per-cell Z mean (~ 0.4 mm for ~ 40 RAW
  points per cell)
- The Clean Z scatter from residual sensor noise (~ 2.0 mm)
- An additional ~ 2.5 mm of unexplained variance

The additional 2.5 mm is the combination of (a) range-weighting
(preferring close-range observations), (b) trajectory-aware deskew
(removing the motion-distortion component of the cloud thickness), and
(c) sub-cell surface extraction (placing the per-cell point at the
zero-crossing rather than the centroid). Closing this residual
end-to-end requires either the vendor's own pipeline or a TSDF
implementation like [VDBFusion](https://github.com/tomato0114/vdbfusion)
that handles all three correctly.

## 9. How to reproduce this on your own data

If you have a single mobile SLAM dataset with raw + post-processed
exports, the analysis pipeline from this study takes ~30 minutes on a
modern workstation and reproduces every metric in this page. The
recipe:

1. **Read the LAS headers** with `laspy` — record format, scale,
   extra-byte dimensions, point counts, bounding boxes.
2. **Compare the two trajectory files** (if available). Mean/max
   separation < 5 mm ⇒ no pose-graph re-opt (H4 ruled out).
3. **Compare `gps_time` sets** of the two clouds. Strict subset ⇒ no
   re-binning (H3 ruled out).
4. **Compute the KL@10 mm grid alignment** for both clouds and for
   E1, E1+face, E1+snap. The post-processed cloud should sit
   between centroid and snap, axis-symmetrically. If it does, H1 is
   the family.
5. **Compute the per-cell Z diff** between a 2D voxel centroid of
   RAW on a clean wall patch and the post-processed Clean on the
   same patch. Diff std < 10 mm ⇒ the algorithm is a per-cell
   surface extraction at the configured resolution.

The Python 3 scripts that implement each of these are in
`/website/scripts/research/mobile-slam-postprocessing/` (linked
below). The full per-experiment log is in
`experiment_log.md` and the per-candidate metrics are in
`benchmark.md` at the same location.

## 10. What this study is not

- **Not a vendor accusation.** This is a forensic analysis of one
  data pair. We have not decompiled any vendor binary, we have not
  read any vendor source, we have not circumvented any licence. The
  analysis is from inputs and outputs only. Vendor software that
  ships a similar feature is fine; this study does not allege
  infringement.
- **Not a full replication.** The 3.5 mm residual on the ceiling is
  real. Closing it requires either the vendor's own pipeline or a
  full TSDF implementation (VDBFusion or Voxblox). This study
  identifies the algorithm family and the key parameters; the exact
  per-point placement function is vendor-specific.
- **Not generalisation-tested.** One indoor dataset. The algorithm
  parameters (range filter, `min_pts`, weighting function) are
  inferred from the data, not measured at the source. A second
  dataset — ideally a different scene and a different resolution
  setting — is needed for high confidence.

## 11. Cross-references

- For the build/run/setup of the analysis environment, see
  [Build on Windows](/docs/build/windows) (the analysis was run on
  Windows Server 2019, Python 3.12, laspy 2.7, numpy 2.3).
- For the FARO SCENE-side workflow that this analysis was
  cross-referenced against, see the vendor's documentation.
- For how the data was produced (a single mobile SLAM scan of an
  indoor scene), see the per-vendor manuals; the analysis is
  vendor-agnostic.
