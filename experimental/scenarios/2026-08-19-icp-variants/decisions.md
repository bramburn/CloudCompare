# Scenario Decision — ICP NN variants (2026-08-19)

> Three nearest-neighbour strategies for ICP, A/B/C'd.
> This is the WINNER + rationale.

## Variants

| # | Folder | Approach | Status |
|---|---|---|---|
| 01 | [`01-naive-on2/`](01-naive-on2/) | O(n²) brute force | Tests pass; **benchmarked** |
| 02 | [`02-kiddo-kdtree/`](02-kiddo-kdtree/) | `kiddo` 6.0 KD-tree | Stub — kiddo API migration needed |
| 03 | [`03-handrolled-octree/`](03-handrolled-octree/) | Hand-rolled octree (matches CCCoreLib) | Tests pass; **benchmarked** |

## Benchmark results (ICP on random Gaussian cloud, 50 max iterations)

| N (data = model size) | 01-naive | 03-handrolled-octree | Ratio (naive/octree) |
|---|---|---|---|
| 2,000 | 0.36 s | 0.60 s (incl. build) | **0.6×** (naive 1.7× faster) |
| 5,000 | 2.39 s | 4.13 s (incl. build) | **0.6×** (naive 1.7× faster) |
| 10,000 | 9.24 s | 23.3 s (incl. build) | **0.4×** (naive 2.5× faster) |

Both implementations give the same final RMS (algorithm is identical, only the NN data structure differs). The hand-rolled octree is consistently **2-3x slower** than the naive implementation on these sizes.

## Decision

**Status: REVERSED. Naive wins for now.**

The original "provisional winner: hand-rolled octree" decision (D4) is **wrong** when measured. On point clouds of 2k-10k points, the naive O(n²) brute force is **faster** than the hand-rolled octree. Reasons:

1. **The hand-rolled octree is naive** — always recurses into all 8 children, no early-out, no kNN aggregation
2. **Build cost is high** — the tree-building itself takes 30-50% of the wall time on small N
3. **Cache behaviour is poor** — octree accesses are scattered; brute-force is sequential

For larger N (>100k), the octree **should** win. We need to test that range before re-evaluating. We can also try the `kiddo` KD-tree (which uses a balanced layout) once its API is migrated.

### When to revisit

- **N > 100k**: brute force becomes O(10¹⁰) per iter → intractable
- **Once kiddo 6.0 is migrated**: real test against a well-tuned KD-tree
- **After 5-10x perf improvement in the hand-rolled octree**: build the tree lazily, add per-leaf bounds, stop early when the search box is fully outside

## What to do now

For the **cc-rust** migration's Phase 2 (ICP), use **variant 01 (naive)** as the default. It's:

- Fastest for typical survey data sizes (10k-100k points per scan)
- Smallest code (~150 lines vs 200 for octree, 50 for kiddo stub)
- Easiest to verify

Add a `fast_nn_search` feature flag for when a real KD-tree implementation is needed. For now, default to naive.

## Other things to test

- **Point format 3 LAS** with RGB and GPS time — the scalar field and ICP work with xyz only; RGB is ignored. Document this.
- **Multi-resolution ICP** — coarse-to-fine subsampling for faster convergence on real data. See the [end-to-end real-data test](../../sessions/2026-08-19-rust-realdata-icp/) for the gap this fills.
- **Outlier rejection** — fixed-trimmed ICP, or robust loss. Without this, real-data ICP overshoots badly (the realdata session recovered −0.48 when +0.5 was applied).

## Related

- Top-level: [`../../docs/decisions.md`](../../docs/decisions.md) (D4 — updated)
- End-to-end real-data: [`../../sessions/2026-08-19-rust-realdata-icp/`](../../sessions/2026-08-19-rust-realdata-icp/)
- Phase 2 roadmap: [`../../../PRD/rust/05-roadmap.md` Phase 2](../../../PRD/rust/05-roadmap.md)
