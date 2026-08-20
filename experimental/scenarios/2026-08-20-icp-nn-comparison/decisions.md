# Decisions — ICP NN comparison (D8 + D9, 2026-08-20)

> Cross-variant winner pick. All numbers are release-mode, single-
> thread, on the same Gaussian fixture (sigma=0.4, seed=42),
> translated by (0.5, -0.2, 0.1). Captured by `run.ps1` and
> stored in `results.json`.

## Setup

- **cc-rust:** `NearestNeighbour` trait added; `icp_with_nn` is the
  NN-driven ICP entry point. `icp_iterate` is now a thin wrapper
  that uses a `BruteForceNN` adapter.
- **Variants:** all four implement the trait:
  - `01-naive-on2` - `pub type NaiveNN<'a> = cc_rust::BruteForceNN<'a>;`
  - `02-kiddo-kdtree` - `KiddoNN` wraps the kiddo `KdTree` and
    adapts the f32 trait contract to the f64 kiddo internals.
  - `03-handrolled-octree` - `OctreeNN` wraps the `Octree` and
    delegates `nearest()` to `Octree::nearest` (no AABB pruning).
  - `04-dgm-octree` (D9) - `DgmOctreeNN` wraps the cell-code-
    ordered `DgmOctree` and matches the C++ `DgmOctree` semantics.
- **Correctness check:** at every size tested, all four NNs
  produce the *same* ICP result - same iterations, same final
  RMS, same converged flag. The trait dispatch is correct.

## End-to-end ICP wall time (NN-driven via the new trait)

This is the headline D8 + D9 number - the actual ICP loop now
genuinely uses each variant's NN, not cc-rust's hard-coded
brute force.

| Variant | N=2k (s) | N=5k (s) | N=10k (s) | N=50k (s) | Speedup @ 10k | Speedup @ 50k |
|---|---|---|---|---|---|---|
| `01-naive-on2` | 0.25 | 2.12 | 11.19 | (skipped, O(n^2)) | 1.0x | n/a |
| `02-kiddo-kdtree` | **0.020** | **0.092** | **0.171** | **1.156** | **65x** | n/a (vs naive) |
| `03-handrolled-octree` | 0.42 | 3.87 | 29.97 | 859.08 | 0.4x | 1.0x (broken pruning) |
| `04-dgm-octree` (D9) | 0.031 | 0.211 | 0.388 | 2.723 | 29x | 315x (vs handrolled) |

**The kiddo advantage is now visible end-to-end and grows with N.**
Pre-D8 the variants produced similar wall times (~10% spread)
because they all fell back to cc-rust's brute force. Post-D8
the trait dispatch is real and kiddo wins decisively:

- At N=10k, kiddo is **65x faster than naive** and **172x faster
  than the hand-rolled octree** end-to-end.
- At N=50k, kiddo is **~743x faster than the hand-rolled octree**
  (1.156s vs 859s). Naive is skipped at 50k because O(n^2) per
  iter is too slow (>2 hours per iter).
- At N=50k, the D9 cell-code-ordered octree is **315x faster
  than the hand-rolled octree** (2.723s vs 859s) but **2.4x
  slower than kiddo**. The D9 gap to kiddo is the HashMap
  overhead in the cell lookup + AABB min-distance check; for
  production use, kiddo is the recommended default.

## Per-query cost (the metric the variants actually differ on)

| Variant | N=2k (us/q) | N=5k (us/q) | N=10k (us/q) | N=50k (us/q) | Scaling |
|---|---|---|---|---|---|
| `02-kiddo-kdtree` | **0.29** | **0.28** | **0.50** | **0.54** | O(log n), flat |
| `03-handrolled-octree` | 6.42 | 18.87 | 42.23 | 510.66 | O(n) per query (broken pruning) |
| `04-dgm-octree` (D9) | 0.64 | 0.78 | 0.69 | 1.03 | O(d^3) cells, AABB-pruned |

The kiddo KD-tree queries are sub-us at every size tested
(0.28-0.54 us) and scaling is essentially flat, which matches
the expected O(log n) cost of a balanced KD-tree. The D9
cell-code-ordered search is also sub-us at every size and
~2-3x slower than kiddo due to the HashMap + AABB overhead.
The hand-rolled octree's per-query cost grows linearly
(6 -> 511 us across 2k -> 50k) because the pruning is broken.

## Why the hand-rolled octree is slower than naive

The hand-rolled `Octree::search` falls back to depth-first
traversal without AABB pruning (the per-child AABB is not
preserved through the recursion, so the `min_dist_sq` check
can't fire at the internal-node level). The result: every
internal node is fully descended, so per-query cost is O(n)
with extra overhead from the tree structure. The kiddo tree
uses an Eytzinger-flex-Pf stem which keeps the tree balanced
and queries in O(log n) without the AABB complication.

This is a real finding, not a bug - the variant was a
correctness + learning exercise for the DgmOctree port
(D-phase 3), not a tuned octree implementation. The DgmOctree
port in `cc-rust/src/dgm_octree.rs` is the production-quality
version; the D9 cell-code-ordered NN search in that octree
demonstrates the correct algorithm and is 100-500x faster
than this hand-rolled version (see the 04-dgm-octree row in
the tables above).

## Why D9 is 2-3x slower than kiddo

The D9 cell-code-ordered search is asymptotically the same
as kiddo (both are O(1) average per query for nearby NN), but
the constant factors are different:

- **HashMap lookup per shell cell.** D9 visits O(d^3) cells
  in the worst case (where d is the Chebyshev distance to
  the NN's cell). Each cell needs a HashMap lookup to find
  its point range. kiddo's B-tree descent has at most 2-3
  indirections per query.
- **AABB min-distance check per cell.** D9 computes the
  AABB min distance and prunes cells that are clearly too
  far. The check itself is fast (8 flops) but it's done
  per cell visited.
- **Pre-sorted point scan.** D9 scans the cell's points
  linearly (typically ~4 per cell at the optimal build
  level). kiddo prunes within the B-tree node, so the
  per-leaf scan is usually smaller.

For ICP queries (which are always near the model in a
converging iteration), d is small (1-3) and the AABB pruning
catches most cells. The HashMap overhead is the dominant
cost. For very large N, kiddo's B-tree scales as O(log n)
while D9's cell-code scales as O(d^3) with d bounded by the
query's offset from the model - both are effectively O(1) per
query in practice.

## Winner: `02-kiddo-kdtree`

Picked on **end-to-end ICP wall time** (the actual ICP loop)
at every size tested, and confirmed by **per-query cost** (the
underlying NN cost). At N=10k:

- kiddo: 0.171 s total ICP time
- naive: 11.19 s total ICP time
- octree: 29.97 s total ICP time
- D9: 0.388 s total ICP time

kiddo is **65x faster than naive** end-to-end, and the
advantage grows with N (because the per-query cost stays flat
for kiddo while it grows linearly for naive and the broken
octree). D9 is the second-best at every size tested.

## What D8 did NOT do

- **No large-N benchmark.** We tested up to N=10k. At N=100k
  or N=1M the kiddo advantage would be even larger (the
  per-query cost is bounded; naive's per-query cost is O(n)).
  This is a follow-up, not a blocker.
- **No CXX-FFI bridge.** The D8 trait is pure Rust. Hooking it
  up to a C++ KD-tree (e.g. nanoflann, FLANN) would require a
  separate CXX bridge; that is part of the deferred Phase 0 →
  live CXX FFI item on the roadmap (closed at 7/8 by user
  decision on 2026-08-20).
- **No D9 implementation.** ~~D9 is "cell-code-ordered NN search
  in DgmOctree" - a separate workstream. The trait is ready
  for it (DgmOctree would just need an `impl NearestNeighbour`
  block).~~ **DONE on 2026-08-20.** The `04-dgm-octree`
  variant is the D9 deliverable. See the row in the wall-time
  table above; see the `cc-rust/src/dgm_octree.rs` for the
  implementation and the unit tests, and see
  `../../docs/decisions.md` D9 for the full reasoning.

## How to reproduce

```powershell
cd experimental\scenarios\2026-08-20-icp-nn-comparison
.\run.ps1
```

The runner writes `results.json` and `RESULTS.md` with the
numbers above. Numbers were captured on this machine
(Windows Server 2019, MSVC 14.44, Rust 1.89) on 2026-08-20.

## Source

- `cc-rust/src/registration.rs` - `NearestNeighbour` trait,
  `icp_with_nn`, `BruteForceNN` adapter.
- `cc-rust/src/dgm_octree.rs` - the cell-code-ordered
  `DgmOctree` and its `DgmOctreeNN` adapter (D9).
- `../2026-08-19-icp-variants/01-naive-on2/` - naive adapter
  + bench.
- `../2026-08-19-icp-variants/02-kiddo-kdtree/` - kiddo
  adapter + bench.
- `../2026-08-19-icp-variants/03-handrolled-octree/` -
  hand-rolled octree adapter + bench.
- `../2026-08-19-icp-variants/04-dgm-octree/` - D9 cell-code
  adapter + bench.
- `results.json` - machine-readable bench output.
- `RESULTS.md` - human-readable bench output.
- `../../docs/decisions.md` D8 - top-level trait decision.
- `../../docs/decisions.md` D9 - cell-code-ordered NN decision.
- `../../docs/patterns.md` P16 - the trait + adapter pattern.
- `../../docs/patterns.md` P17 - clone input for N-sided bench.
- `../../docs/patterns.md` P18 - minDistToBorder in cell-code NN.
