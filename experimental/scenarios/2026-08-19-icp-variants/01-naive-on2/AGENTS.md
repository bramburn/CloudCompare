# Session: 01-naive-on2 (ICP variant 1)

**Scenario:** [`../`](../) — ICP variants comparison
**Approach:** O(n²) brute-force nearest-neighbour
**Status:** Skeleton — implements ICP, tests pass, but not benchmarked yet

## What this is

The baseline ICP implementation. Every iteration, for every data point, scans
all model points to find the closest. O(n × m) per iteration, with m = n
for two equal-sized clouds.

## Build & test

```powershell
cd experimental/scenarios/2026-08-19-icp-variants/01-naive-on2
cargo test
cargo run --release --bin icp_bench -- 2000 42
```

## Expected performance

For N points:

| N | Time per ICP iteration | Total time (50 iters) |
|---|---|---|
| 1,000 | 1 ms | 50 ms |
| 10,000 | 100 ms | 5 s |
| 100,000 | 10 s | 8 min |
| 1,000,000 | 17 min | 14 hours |

**Verdict:** infeasible for real `.las` data (typically 10M+ points).

## When to use

- Tiny clouds (< 10k points)
- Unit tests
- Sanity check before trying the heavier variants
- Profiling (the O(n²) hot path is easy to spot)

## When NOT to use

- Real survey data
- Anything > 10k points
- Anything time-sensitive

## Observations

*To be filled in once we run the benchmark and compare with variants 02 and 03.*

## See also

- Scenario: [`../`](../) — the A/B/C comparison
- Sibling: [`../02-kiddo-kdtree/`](../02-kiddo-kdtree/), [`../03-handrolled-octree/`](../03-handrolled-octree/)
- Originating implementation: [`../../../sessions/2026-08-19-rust-migration-icp-scalarfield/src/registration.rs`](../../../sessions/2026-08-19-rust-migration-icp-scalarfield/src/registration.rs)
