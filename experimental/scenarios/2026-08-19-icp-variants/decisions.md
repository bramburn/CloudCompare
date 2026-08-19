# Scenario Decision — ICP variants (2026-08-19)

> Three implementations of ICP nearest-neighbour, A/B/C'd.
> This is the WINNER + rationale. Detail lives in each variant's session.

## Variants

| # | Folder | Approach | Status |
|---|---|---|---|
| 01 | [`01-naive-on2/`](01-naive-on2/) | O(n²) brute force | Tests pass; bench not run |
| 02 | [`02-kiddo-kdtree/`](02-kiddo-kdtree/) | `kiddo` 6.0 KD-tree | Stub; API migration needed |
| 03 | [`03-handrolled-octree/`](03-handrolled-octree/) | Hand-rolled octree | Tests pass; bench not run |

## Decision

**Status: PENDING** — variants 01 and 03 have working implementations but
have not been benchmarked on real `.las` data. Variant 02 is a stub.

**Provisional winner: variant 03 (hand-rolled octree).**

Reasoning, in priority order:

1. **Match CCCoreLib's algorithm.** The hand-rolled octree mirrors
   `DgmOctree`. This means characterisation tests against the C++
   baseline are direct. `kiddo`'s algorithm is different.
2. **Pure-Rust, no extra deps.** The `kiddo` crate needs explicit
   stem/leaf strategy parameters in v6.0, which makes it harder to
   reason about correctness. Hand-rolled is ~200 lines, easy to read.
3. **Reasonable expected performance.** For 1M points, the octree
   should match or beat `kiddo` for our use case (3D, f32, mostly
   uniform point density). For extreme cases (10M+ points,
   very dense clusters), `kiddo` is likely faster.
4. **Tunable for per-leaf scalar-field stats.** CCCoreLib's
   `DgmOctree` stores per-leaf data, which we'll need for density
   queries. The hand-rolled version is ready for this; `kiddo` would
   require a custom structure on top.

**Re-evaluation trigger:**

- If the octree is more than 2x slower than `kiddo` on a real 1M-point
  scan, switch to `kiddo` and accept the API complexity.
- If we need exact CCCoreLib match (because we're characterising tests),
  keep the octree.

## When to revisit

- After the first benchmark on `D:\82 BROOK AVENUE\output\*.las`.
- After we have to handle 10M+ point clouds.
- After we need GPU acceleration (both options need a rewrite for GPU).

## What was tried

### Variant 01 (naive O(n²))

- ✅ Same algorithm as the main session (`registration.rs`).
- ✅ Tests pass: identity transform, small translation.
- ❌ Infeasible for real data (10M+ points = 10^14 ops/iter).

### Variant 02 (kiddo KD-tree)

- ❌ `kiddo 4.0` requires `fixed 1.31` which needs `rustc 1.93`. We have 1.89.
- ❌ `kiddo 6.0` changed the API: `kiddo::float::kdtree::KdTree` was
  removed; you now need explicit stem/leaf strategy types and arena
  block sizes.
- ⚠️ Stub-only for now. Real impl is a follow-up.

### Variant 03 (hand-rolled octree)

- ✅ Basic octree: AABB + 8-way subdivision + leaf splitting.
- ✅ Tests pass: nearest-neighbour is correct, ICP converges on translation.
- ⚠️ Limitations: stores only `[f32; 3]` at leaves (no per-point index),
  worst-case unbalanced, no deduplication.

## Next steps

1. **Benchmark 01 vs 03** on a 100k-point synthetic Gaussian cloud
   to get baseline numbers. Use `cargo run --release --bin icp_bench`.
2. **Add per-point indices** to the octree (track model idx alongside point).
3. **Run on real data** — point at `D:\82 BROOK AVENUE\output\*.las`.
   Compare with CCCoreLib's `RegistrationTools` ICP on the same input.
4. **Revisit kiddo** if the octree underperforms. The migration is
   bounded: ~50 lines of code change in `lib.rs`.

## Related

- Top-level: [`../../docs/decisions.md`](../../docs/decisions.md) (D4)
- Originating session: [`../../sessions/2026-08-19-rust-migration-icp-scalarfield/`](../../sessions/2026-08-19-rust-migration-icp-scalarfield/)
- AGENTS.md: [`AGENTS.md`](AGENTS.md)
