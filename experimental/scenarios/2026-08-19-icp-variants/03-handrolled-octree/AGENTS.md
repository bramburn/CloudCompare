# Session: 03-handrolled-octree (ICP variant 3)

**Scenario:** [`../`](../) — ICP variants comparison
**Approach:** Hand-rolled octree, O(n) build, O(log n) average query
**Status:** Skeleton — implements octree + ICP, basic tests pass, not benchmarked yet

## What this is

A from-scratch octree implementation, modelled on CCCoreLib's
`DgmOctree`. The tree subdivides space into 8 octants recursively,
storing points at leaves.

- **Build:** O(n) (one pass, subdivide when leaf overflows)
- **Query:** O(log n) average, O(n) worst case
- **Per-ICP-iter:** O(n log n × iters)

## Why hand-rolled

- **Match CCCoreLib's algorithm exactly.** This makes characterisation
  tests against the C++ `DgmOctree` straightforward.
- **No external dependencies.** Pure-Rust, no `unsafe` (the octree
  uses `Box` smart pointers, not raw pointers).
- **Easy to add features** like per-leaf scalar-field statistics,
  density estimation, or level-of-detail rendering.

## Tradeoffs vs. `kiddo` (variant 02)

| | hand-rolled octree | `kiddo` KD-tree |
|---|---|---|
| Lines of code | ~200 | 0 (crate dep) |
| External deps | 0 | 1 |
| Balance | Worst case unbalanced | Always balanced |
| Match CCCoreLib | Exact | Different algorithm |
| Tunable per-leaf stats | Yes | No |
| `kiddo` ecosystem | No | Yes (many geo crates use it) |

## Build & test

```powershell
cd experimental/scenarios/2026-08-19-icp-variants/03-handrolled-octree
cargo test
cargo run --release --bin icp_bench -- 2000 42
```

## Known limitations

- `OctreeNode::Leaf` stores only `Vec<[f32; 3]>` — no per-point index.
  The `nearest` return value's `idx` is a placeholder. A real implementation
  would store `Vec<usize>` indices to the model array.
- Worst case: very dense clusters of points → deep trees, more memory.
- No deduplication of points at exact same coordinates.

## See also

- Scenario: [`../`](../) — the A/B/C comparison
- Sibling: [`../01-naive-on2/`](../01-naive-on2/), [`../02-kiddo-kdtree/`](../02-kiddo-kdtree/)
- CCCoreLib `DgmOctree`: `libs/qCC_db/extern/CCCoreLib/src/DgmOctree.cpp`
