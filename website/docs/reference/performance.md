---
title: Performance and scalability
sidebar_label: Performance
sidebar_position: 6
description: Getting 100M+ point clouds to behave on 2 GB RAM, where the time goes, and the knobs that actually help.
---

# Performance and scalability

The fork targets 2 GB-RAM workstations processing 100M+ point terrestrial
and mobile laser scans. This page is the practical playbook: where the
time goes, which knobs help, and the failure modes to look for.

## The two bottlenecks

Almost every performance problem in CloudCompare is one of two things:

1. **Memory** — how much RAM the working set uses. The fork's
   self-contained plugins are chosen in part because they don't blow
   the 2 GB ceiling (the heavyweight plugins — PCL, PDAL, OpenCASCADE —
   can each add 1-2 GB on their own).
2. **Single-threaded CPU** — most of the older algorithms (ICP, CSF,
   Poisson reconstruction) are not parallelised. Multi-core helps the
   GUI and the I/O, not the algorithm.

GPU rendering and LOD (level of detail) are **render-time** optimisations
that help the viewport stay smooth; they do **not** help the algorithm
run faster.

## RAM budget

Approximate working-set for the common operations:

| Operation | Per-point memory | 100M points |
|---|---|---|
| Raw cloud (xyz) | 12 B | 1.2 GB |
| + intensity (1 dim) | 16 B | 1.6 GB |
| + colour RGB (3 dims) | 28 B | 2.8 GB |
| + normals (3 dims) | 40 B | 4.0 GB |
| + 1 scalar field (1 dim) | 16 B | 1.6 GB |
| + 5 scalar fields (typical M3C2 output) | 52 B | 5.2 GB |
| + octree overhead | +10-20% | +0.5-1.0 GB |
| + M3C2 intermediate (cores, normals) | +30-50% | +1.5-2.5 GB |

**Rule of thumb:** a raw 100M-point cloud with one scalar field and an
octree is about 4 GB working set. Add colour, normals, and a few
scalar fields and you are at 6-7 GB. The 2 GB-RAM target is for the
**decimated** working set, not the raw cloud.

## Decimation strategies

For an M3C2 or ICP run on a 100M+ cloud, the working set has to come
down. The fork's typical recipe:

1. **Random subsample** to 50M (cuts RAM by 2×, keeps shape).
2. **Spatial subsample** at 25-50 mm to remove point density outliers
   from over-scanned regions.
3. **CSF or RANSAC classification** on the subsampled cloud. The
   classification result is then propagated to the full cloud via
   `Edit > Scalar fields > Transfer`.

The result is a 5-10 GB working set at the end of a full monitoring
pipeline on a 100M-point source scan. Comfortable on a 16 GB machine,
tight on 8 GB.

## Algorithm-specific tips

### ICP

ICP scales as O(N log N) for a uniform-density cloud, dominated by the
nearest-neighbour search. With 1M points in each cloud, ICP is
seconds; with 50M, it is minutes; with 500M, it is hours and may run
out of memory on the search index.

**Tip:** subsample both clouds to ~1M points for ICP. The alignment
quality is dominated by the **shape** the search index sees, not the
point count.

### M3C2

M3C2 scales roughly as O(N × M) where N is the comparison cloud
point count and M is the normal-scale grid. With 10M points and a
10 cm normal scale, expect 5-15 minutes.

**Tip:** crop to the area of interest. M3C2 on the whole scan is
usually wasted compute.

### CSF (ground filtering)

CSF scales linearly with cloud size. The cloth simulation is
memory-light, but the octree build is the bottleneck.

**Tip:** spatial subsample to 50-100 mm before CSF. CSF on a 5 mm cloud
gives the same DTM quality as CSF on a 50 mm cloud, just 100× slower.

### Poisson reconstruction

Poisson is the most memory-hungry of the standard algorithms. A 10M-point
cloud needs 4-8 GB for the octree alone; the solver is another 2-4 GB.

**Tip:** use [`qPoissonRecon`](/docs/plugins/local-set)
on a subsampled cloud (1-2M points) and use the **low** octree depth
(6-8) by default. The reconstructed mesh can be refined with a second
Poisson pass at higher depth if needed.

## Where the viewport stutters

The viewport stutters for different reasons at different point counts:

| Symptom | Likely cause | Fix |
|---|---|---|
| Stutter on 1-5M points | GPU driver, shader compilation | Update driver, disable `qEDL` / `qSSAO` |
| Stutter on 5-50M points | LOD not active | Enable `Display > LOD` (or default-on) |
| Stutter on 50-200M points | Display settings | Drop `Point size`, use simpler colour mapping |
| Stutter when panning | Octree traversal | Force `Octree > Rebuild` |

`qEDL` and `qSSAO` are post-filters that re-render the viewport once
per frame. On a 50M+ point cloud with either enabled, the GPU becomes
the bottleneck. Disable them for navigation; re-enable for the
screenshot.

## When to enable a heavyweight plugin

The disabled plugins add real capabilities — but at a cost:

| Plugin | RAM cost | Disk cost | Build time cost | Worth enabling when… |
|---|---|---|---|---|
| `qPCL` | 1-2 GB | 500 MB+ | 1-2 h | You need ICP variants, segmentation, or feature estimation |
| `qPDALIO` | 50 MB | 200 MB | 30 m | You need LAZ, COPC, or GDAL rasters |
| `qLASIO` | 30 MB | 50 MB | 5 m | You need LAZ compression (cheap) |
| `qE57IO` | 100 MB | 200 MB | 30 m | You need fast E57 round-tripping |
| `qFBXIO` | 200 MB | 1 GB | 30 m | You need FBX export to CAD |
| `qStepCADImport` | 500 MB | 1 GB | 1 h | You need STEP / IGES import |

The vcpkg recipe for each is at
[Disabled priority](/docs/plugins/disabled-priority). Enable one at a
time, re-run the build, and benchmark before adding the next.

## Profiling

For a quick profile of where time goes on a specific operation:

1. `Display > Console` (or `View > Console`) — every action logs
   duration to the console.
2. The console shows per-step timings: `ICP took 12.4 s`, `M3C2 took
   5m 32s`. Use this to find the slow step.
3. For deeper profiling on Windows, attach Visual Studio's
   performance profiler to `CloudCompare.exe`. On Linux, use `perf`.

The console is the cheapest profiler and usually enough. If ICP
"feels slow" on a small cloud, the console will tell you whether it's
ICP itself or the previous merge step.

## Gotchas

- **More RAM is the single biggest win.** The fork's 2 GB target is a
  *minimum*, not a comfortable working set. 16 GB is comfortable; 32
  GB is luxury.
- **Multi-threaded algorithms are rare.** Most plugins are
  single-threaded. CPU frequency matters more than core count.
- **GPU does not help algorithms.** Only the viewport rendering.
- **Decimation is destructive.** Save the full-resolution cloud as
  `.bin` before any decimate / crop / subsample pass.
- **CloudCompare does not stream from disk.** Everything in the DB
  tree is in memory. A 500M-point cloud needs 8 GB just for the
  points, before any processing.
