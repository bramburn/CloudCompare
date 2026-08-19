# Session: 02-kiddo-kdtree (ICP variant 2)

**Scenario:** [`../`](../) — ICP variants comparison
**Approach:** `kiddo` KD-tree for O(log n) nearest-neighbour
**Status:** STUB — kiddo 6.0 API migration needed

## What this is

ICP using the [`kiddo`](https://crates.io/crates/kiddo) KD-tree crate.
The original target was the 4.0 API (`kiddo::float::kdtree::KdTree`),
but `kiddo 6.0` (the version that supports rustc 1.89) changed the API:

- **Old (4.0):** `use kiddo::float::kdtree::KdTree; let tree: KdTree<f32, 3> = KdTree::new(); tree.add(&[x, y, z], idx);`
- **New (6.0):** `use kiddo::kd_tree::KdTree; let tree: KdTree<f64, u32, Eytzinger, VecOfArenas<f64, u32, 3, 256>, 3, 256> = ...`

The 6.0 API requires:

- Choosing a stem strategy (Eytzinger, Default, etc.)
- Choosing a leaf strategy (VecOfArenas, etc.)
- Specifying the arena block size (e.g. 256)
- The element type, the ID type, the dimension

This is a richer (and faster) API but needs a non-trivial migration.

## Build & test

```powershell
cd experimental/scenarios/2026-08-19-icp-variants/02-kiddo-kdtree
cargo test
cargo run --release --bin icp_bench -- 2000 42
```

`cargo test` will succeed (the test just checks the crate compiles).
`cargo run --bin icp_bench` will fail until the implementation is filled in.

## Why this is still useful as a session

The session exists to record:

- The kiddo 6.0 API is non-trivial to migrate to from earlier versions.
- `kiddo 4.0` requires `fixed 1.31` which needs rustc 1.93.
- The `kiddo` ecosystem is otherwise solid for production.

When the migration is done, the file `src/lib.rs` needs the ICP loop
from variant 01, with the NN search replaced by `tree.nearest_one(...)`.

## See also

- Scenario: [`../`](../) — the A/B/C comparison
- Sibling: [`../01-naive-on2/`](../01-naive-on2/), [`../03-handrolled-octree/`](../03-handrolled-octree/)
- kiddo docs: <https://docs.rs/kiddo>
- kiddo 6.0 examples: `~/.cargo/registry/src/.../kiddo-6.0.2/examples/`
