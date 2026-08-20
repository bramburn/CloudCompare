# Decisions — ICP variants scenario

> This file is the unit-test-level decisions for the three
> variants. The cross-variant end-to-end comparison (D8, the
> D8 deliverable) lives in
> [`../2026-08-20-icp-nn-comparison/decisions.md`](../2026-08-20-icp-nn-comparison/decisions.md).

## Status: `selected` (D8 done, 2026-08-20)

**Winner: `02-kiddo-kdtree`.** Picked on end-to-end ICP wall
time at 2k / 5k / 10k Gaussian points (see the comparison
scenario for the bench numbers — kiddo is 43× faster than naive
at 10k).

This scenario is now **`selected`** because:

1. cc-rust exposes `icp_with_nn` and the `NearestNeighbour`
   trait (D8, 2026-08-20).
2. All three variants implement the trait and the ICP loop is
   now genuinely NN-driven for each.
3. The cross-variant comparison scenario
   (`../2026-08-20-icp-nn-comparison/`) picks the winner with
   evidence (43× speedup end-to-end at 10k).

The original "Cannot pick a winner yet" section below is kept
for the historical record; the "D8 candidate" steps have all
landed.

## Current state (D8 done, 2026-08-20)

| Variant | Status | Trait adapter | End-to-end ICP @ 10k | NN @ 10k (µs/q) |
|---|---|---|---|---|
| `01-naive-on2` | `selected` (brute-force reference) | `BruteForceNN` (alias) | 7.54 s | n/a |
| `02-kiddo-kdtree` | **`selected` (winner)** | `KiddoNN` (f32→f64 wrap) | **0.175 s** | **0.40** |
| `03-handrolled-octree` | `selected` (D-phase 3 prototype) | `OctreeNN` (delegate) | 18.5 s | 35.9 |

## ICP wall times (release, Gaussian, seed=42, NN-driven)

| Variant | N=2k | N=5k | N=10k |
|---|---|---|---|
| `01-naive-on2` | 0.247 s | 2.03 s | 7.54 s |
| `02-kiddo-kdtree` | **0.021 s** | **0.080 s** | **0.175 s** |
| `03-handrolled-octree` | 0.359 s | 3.34 s | 18.5 s |

**kiddo is 43× faster than naive at 10k** end-to-end. The
hand-rolled octree is *slower* than naive because its `search()`
falls back to depth-first traversal without AABB pruning (the
per-child AABB isn't preserved through the recursion). This is
expected: the hand-rolled octree was a learning exercise for the
DgmOctree port (D-phase 3), not a tuned implementation. The
DgmOctree port in `cc-rust/src/dgm_octree.rs` will be the
production-quality version; the D9 candidate is the
cell-code-ordered NN search in that octree.

## NN query times (release, Gaussian, 1000 queries)

| Variant | N=2k | N=5k | N=10k | Scaling |
|---|---|---|---|---|
| `02-kiddo-kdtree` | **0.28 µs/q** | **0.39 µs/q** | **0.40 µs/q** | O(log n), flat |
| `03-handrolled-octree` | 6.51 µs/q | 16.8 µs/q | 35.9 µs/q | O(n) (broken pruning) |

## Pre-D8 (SUPERSEDED): "ICP wall times are similar across variants"

The pre-D8 entry said: "the ICP wall time is dominated by
cc-rust's brute force, not the NN". That was true because
each variant's wrapper built its tree for timing and then fell
back to cc-rust's brute force for the actual iteration — the
wall-time numbers were measuring cc-rust's brute force, not
the variant's NN. D8 fixed this by adding the `NearestNeighbour`
trait and `icp_with_nn` entry point; the post-D8 numbers above
are the real comparison.

## Original (SUPERSEDED) decision

> Retained for the historical record. Do not use these numbers
> to make a decision.

The original claim was:

- **Naive wins for now.** On random Gaussian point clouds of
  2k–10k points, the naive implementation was **2–3× faster**
  than the hand-rolled octree (measured 2026-08-19 18:00 UTC:
  0.36s vs 0.60s on 2k, 2.4s vs 4.1s on 5k, 9.2s vs 23.3s on 10k).
- **Reason:** the hand-rolled octree was naive (no early-out,
  no per-leaf bounds, all 8 children recursed).

That was true, but both variants were running the buggy ICP, so
the *correctness* of the comparison was not load-bearing for any
ICP feature work. Re-bench with the corrected algorithm before
reinstating or reversing the claim.

## Source

- `cc-rust/src/registration.rs` (corrected ICP + D8 trait)
- `scenarios/2026-08-19-icp-variants/01-naive-on2/` (brute-force adapter + bench)
- `scenarios/2026-08-19-icp-variants/02-kiddo-kdtree/` (kiddo adapter + bench)
- `scenarios/2026-08-19-icp-variants/03-handrolled-octree/` (octree adapter + bench)
- `../2026-08-20-icp-nn-comparison/` (D8 deliverable: cross-variant end-to-end bench)
- `experimental/docs/decisions.md` D4 (history), D8 (D8 trait + dispatch decision)
- `experimental/docs/patterns.md` P12, P13, P14 (the three ICP fixes), P16 (the trait + adapter pattern)
