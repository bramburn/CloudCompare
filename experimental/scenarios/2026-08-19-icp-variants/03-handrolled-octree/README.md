# 03-handrolled-octree — ICP variant 3 (octree)

ICP with a from-scratch octree (matches CCCoreLib's `DgmOctree`).

## Quick start

```powershell
cargo test
cargo run --release --bin icp_bench -- 2000 42
```

## What it does

- Same Horn 1987 SVD-based ICP
- NN search: hand-rolled octree, O(log n) per query average
- Tree built once per ICP run, O(n)

## See also

- `AGENTS.md` — performance tradeoffs, when to use
- [`../`](../) — the scenario A/B/C
- [`../01-naive-on2/`](../01-naive-on2/) — O(n²) baseline
- [`../02-kiddo-kdtree/`](../02-kiddo-kdtree/) — KD-tree alternative
