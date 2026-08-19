# 02-kiddo-kdtree — ICP variant 2 (KD-tree)

ICP with `kiddo` KD-tree for O(log n) nearest-neighbour queries.

## Quick start

```powershell
cargo test
cargo run --release --bin icp_bench -- 2000 42
```

## What it does

- Same Horn 1987 SVD-based ICP as variant 01
- KD-tree built once before the iteration loop (O(n log n))
- Per-query: O(log n) average, O(n) worst case
- Total: O(n log n × iters)

## See also

- `AGENTS.md` — performance table, when to use, when not to use
- [`../`](../) — the scenario A/B/C
- [`../01-naive-on2/`](../01-naive-on2/) — the baseline (O(n²))
- [`../03-handrolled-octree/`](../03-handrolled-octree/) — the octree alternative
