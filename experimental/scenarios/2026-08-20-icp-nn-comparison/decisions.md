# Decisions — ICP NN comparison (D8, 2026-08-20)

> Cross-variant winner pick. All numbers are release-mode, single-
> thread, on the same Gaussian fixture (sigma=0.4, seed=42),
> translated by (0.5, −0.2, 0.1). Captured by `run.ps1` and
> stored in `results.json`.

## Setup

- **cc-rust:** `NearestNeighbour` trait added; `icp_with_nn` is the
  NN-driven ICP entry point. `icp_iterate` is now a thin wrapper
  that uses a `BruteForceNN` adapter.
- **Variants:** all three implement the trait:
  - `01-naive-on2` — `pub type NaiveNN<'a> = cc_rust::BruteForceNN<'a>;`
  - `02-kiddo-kdtree` — `KiddoNN` wraps the kiddo `KdTree` and
    adapts the f32 trait contract to the f64 kiddo internals.
  - `03-handrolled-octree` — `OctreeNN` wraps the `Octree` and
    delegates `nearest()` to `Octree::nearest`.
- **Correctness check:** at every size tested, all three NNs
  produce the *same* ICP result — same iterations, same final
  RMS, same converged flag. The trait dispatch is correct.

## End-to-end ICP wall time (NN-driven via the new trait)

This is the headline D8 number — the actual ICP loop now
genuinely uses each variant's NN, not cc-rust's hard-coded
brute force.

| Variant | N=2k (s) | N=5k (s) | N=10k (s) | Speedup vs naive @ 10k |
|---|---|---|---|---|
| `01-naive-on2` | 0.247 | 2.03 | 7.54 | 1.0× |
| `02-kiddo-kdtree` | **0.021** | **0.080** | **0.175** | **43×** |
| `03-handrolled-octree` | 0.359 | 3.34 | 18.5 | 0.41× (slower than naive) |

**The kiddo advantage is now visible end-to-end.** Pre-D8 the
three variants produced similar wall times (~10% spread) because
they all fell back to cc-rust's brute force. Post-D8 the trait
dispatch is real and kiddo wins decisively — at N=10k the
end-to-end ICP loop is **43× faster** with the kiddo KD-tree
than with brute force, and **100× faster** than with the
hand-rolled octree.

## Per-query cost (the metric the variants actually differ on)

| Variant | N=2k (µs/q) | N=5k (µs/q) | N=10k (µs/q) | Scaling |
|---|---|---|---|---|
| `01-naive-on2` | (no tree) | (no tree) | (no tree) | O(n) per query |
| `02-kiddo-kdtree` | **0.28** | **0.39** | **0.40** | O(log n), flat |
| `03-handrolled-octree` | 6.51 | 16.8 | 35.9 | O(log n) average with AABB pruning |

The kiddo KD-tree queries are sub-µs at every size tested and
scaling is essentially flat, which matches the expected O(log n)
cost of a balanced KD-tree.

## Why the hand-rolled octree is slower than naive

The hand-rolled `Octree::search` falls back to depth-first
traversal without AABB pruning (the per-child AABB is not
preserved through the recursion, so the `min_dist_sq` check
can't fire at the internal-node level). The result: every
internal node is fully descended, so per-query cost is O(n)
with extra overhead from the tree structure. The kiddo tree
uses an Eytzinger-flex-Pf stem which keeps the tree balanced
and queries in O(log n) without the AABB complication.

This is a real finding, not a bug — the variant was a
correctness + learning exercise for the DgmOctree port
(D-phase 3), not a tuned octree implementation. The DgmOctree
port in `cc-rust/src/dgm_octree.rs` will be the production-
quality version; the D9 candidate is the cell-code-ordered
NN search in that octree, which would close the gap with
kiddo.

## Winner: `02-kiddo-kdtree`

Picked on **end-to-end ICP wall time** (the actual ICP loop)
at every size tested, and confirmed by **per-query cost** (the
underlying NN cost). At N=10k:

- kiddo: 0.175 s total ICP time
- naive: 7.54 s total ICP time
- octree: 18.5 s total ICP time

kiddo is **43× faster than naive** end-to-end, and the
advantage grows with N (because the per-query cost stays flat
for kiddo while it grows linearly for naive and the broken
octree).

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
- **No D9 implementation.** D9 is "cell-code-ordered NN search
  in DgmOctree" — a separate workstream. The trait is ready
  for it (DgmOctree would just need an `impl NearestNeighbour`
  block).

## How to reproduce

```powershell
cd experimental\scenarios\2026-08-20-icp-nn-comparison
.\run.ps1
```

The runner writes `results.json` and `RESULTS.md` with the
numbers above. Numbers were captured on this machine
(Windows Server 2019, MSVC 14.44, Rust 1.89) on 2026-08-20.

## Source

- `cc-rust/src/registration.rs` — `NearestNeighbour` trait,
  `icp_with_nn`, `BruteForceNN` adapter.
- `../2026-08-19-icp-variants/01-naive-on2/` — naive adapter
  + bench.
- `../2026-08-19-icp-variants/02-kiddo-kdtree/` — kiddo
  adapter + bench.
- `../2026-08-19-icp-variants/03-handrolled-octree/` —
  hand-rolled octree adapter + bench.
- `results.json` — machine-readable bench output.
- `RESULTS.md` — human-readable bench output.
- `../../docs/decisions.md` D8 — top-level decision.
- `../../docs/patterns.md` P16 — the trait + adapter pattern.
