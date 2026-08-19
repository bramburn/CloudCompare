# Scenario: ICP variants (2026-08-19)

> Compare 3 ICP implementations and pick a winner.

## The question

ICP needs a fast nearest-neighbour (NN) search. The naive O(n²) brute-force
NN in `sessions/2026-08-19-rust-migration-icp-scalarfield/src/registration.rs`
works for tests but is too slow for real `.las` data (10M+ points). We have
three options:

1. **Naive O(n²)** — simple, no dependencies, O(n²) per ICP iteration
2. **`kiddo` KD-tree** — pure-Rust KD-tree crate, O(log n) per NN
3. **Hand-rolled octree** — matches CCCoreLib's `DgmOctree`, octree subdivision

This scenario implements all three, benchmarks them on the same input, and
records the winner in [`decisions.md`](decisions.md).

## The variants

| # | Folder | Approach | Status |
|---|---|---|---|
| 01 | `01-naive-on2/` | O(n²) brute force | Done — passes tests |
| 02 | `02-kiddo-kdtree/` | `kiddo` KD-tree | Skeleton |
| 03 | `03-handrolled-octree/` | hand-rolled octree | Skeleton |

## How to read this scenario

Each variant is a **sibling session** under `experimental/scenarios/.../`. They
follow the same `rust_lib` template structure:

```
01-naive-on2/
├── AGENTS.md               ← what this variant is
├── README.md               ← build/run, observations
├── Cargo.toml
└── src/
    ├── lib.rs              ← the ICP implementation
    └── main.rs             ← CLI
```

The `decisions.md` at this level is the **winner + rationale**. It links to
each variant session.

## Why scenarios

A single ICP implementation might be best for tiny clouds, a different one
for huge clouds. We want to see the **tradeoff curve**, not pick one
implementation. Scenarios let us A/B/C in parallel and record what we
actually measured.

## Workflow

1. **Implement variant 01** (naive) — already done in
   `sessions/2026-08-19-rust-migration-icp-scalarfield/`, copy the
   relevant `registration.rs` here.
2. **Implement variant 02** (`kiddo`) — add the dep, build the KD-tree,
   benchmark.
3. **Implement variant 03** (octree) — hand-roll the subdivision.
4. **Benchmark all three** on the same input (a small cube + a real
   `.las` file).
5. **Write `decisions.md`** with the winner, the time, the memory
   cost, the LOC, and any "we'll re-evaluate at N=1M" notes.
6. **Update `docs/decisions.md` (top-level)** with a one-paragraph
   summary, linked back to this scenario.
7. **Update root `AGENTS.md`** with "the current Rust ICP implementation
   uses X (see scenario)".

## What we are NOT comparing

- **Algorithm variants** (point-to-point vs. point-to-plane ICP). That's
  a different scenario (`scenarios/2026-08-20-icp-algorithm-variants/` —
  TODO).
- **Rotation estimation** (SVD vs. quaternions vs. dual quaternions).
  Also a different scenario.
- **Convergence criteria** (fixed iterations vs. RMS-based vs. delta
  threshold). Yet another.

We are ONLY comparing the **nearest-neighbour** data structure.

## Related

- Top-level decisions: [`../../docs/decisions.md`](../../docs/decisions.md) (D4)
- Patterns: [`../../docs/patterns.md`](../../docs/patterns.md)
- Index: [`../../docs/index.md`](../../docs/index.md)
