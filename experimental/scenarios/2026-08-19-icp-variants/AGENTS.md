# Scenario: ICP variants (2026-08-19)

> Compare 3 ICP implementations and pick a winner.

## Status

- [x] scaffolded
- [x] buildable
- [x] unit-tested
- [ ] reference-validated
- [ ] benchmarked         ← **SUPERSEDED — see "Decision status" below**
- [ ] selected
- [ ] graduated / abandoned

Source of truth: `experiment.toml` at this level.

## The question

ICP needs a fast nearest-neighbour (NN) search. The naive O(n²) brute-force
NN in `cc-rust/src/registration.rs` works for tests but is too slow for
real `.las` data (10M+ points). We have three options:

1. **Naive O(n²)** — simple, no dependencies, O(n²) per ICP iteration
2. **`kiddo` KD-tree** — pure-Rust KD-tree crate, O(log n) per NN
3. **Hand-rolled octree** — matches CCCoreLib's `DgmOctree`, octree subdivision

This scenario implements all three, benchmarks them on the same input, and
records the winner in [`decisions.md`](decisions.md).

## The variants

| # | Folder | Approach | Status | Per-variant manifest |
|---|---|---|---|---|
| 01 | `01-naive-on2/` | O(n²) brute force | `unit-tested` | [`01-naive-on2/experiment.toml`](01-naive-on2/experiment.toml) |
| 02 | `02-kiddo-kdtree/` | `kiddo` KD-tree | `buildable` (skeleton) | [`02-kiddo-kdtree/experiment.toml`](02-kiddo-kdtree/experiment.toml) |
| 03 | `03-handrolled-octree/` | hand-rolled octree | `buildable` (skeleton) | [`03-handrolled-octree/experiment.toml`](03-handrolled-octree/experiment.toml) |

## Decision status — IMPORTANT

The original "naive wins" claim in [`decisions.md`](decisions.md) is
**SUPERSEDED**. It was made with a buggy ICP (three distinct bugs fixed on
2026-08-19; see D4 in [`../../docs/decisions.md`](../../docs/decisions.md) and
patterns P12, P13, P14 in [`../../docs/patterns.md`](../../docs/patterns.md)).
Before any new "selected" decision, all variants must be re-bench'd against
the corrected algorithm.

## How to read this scenario

Each variant is a **sibling session** under `experimental/scenarios/.../`. They
follow the same `rust_lib` template structure:

```
01-naive-on2/
├── AGENTS.md               ← what this variant is
├── README.md               ← build/run, observations
├── experiment.toml         ← machine-readable contract for this variant
├── Cargo.toml
└── src/
    ├── lib.rs              ← the ICP implementation
    └── main.rs             ← CLI / bench binary
```

The `decisions.md` at this level is the **winner + rationale**. It links to
each variant session. The `experiment.toml` at this level is the **machine-
readable contract** that the runner consumes.

The fixtures referenced from this scenario live in
[`../../fixtures/`](../../fixtures/) (asymmetric-9, cube-8, brook-avenue-splice).
The scenario's `experiment.toml` references them by relative path; the
runner (`../../run.ps1`) resolves them at run time.

## Workflow

1. **Implement variant 01** (naive) — already done in
   `cc-rust/src/registration.rs`, copy the relevant logic here.
2. **Implement variant 02** (`kiddo`) — add the dep, build the KD-tree,
   benchmark.
3. **Implement variant 03** (octree) — fix the `nearest()` placeholder,
   add early-out, benchmark.
4. **Re-benchmark all three** with the corrected ICP, on the
   `asymmetric-9` fixture (the canonical non-degenerate test cloud).
5. **Write `decisions.md`** with the winner, the time, the memory
   cost, the LOC, and any "we'll re-evaluate at N=1M" notes.
6. **Update `docs/decisions.md` (top-level)** with a one-paragraph
   summary, linked back to this scenario.
7. **Update root `AGENTS.md`** with "the current Rust ICP implementation
   uses X (see scenario)".

## Run the scenario

```powershell
& 'C:\dev\CloudCompare\experimental\run.ps1' `
    -Scenario 'C:\dev\CloudCompare\experimental\scenarios\2026-08-19-icp-variants' `
    -Profile release
```

The runner validates each variant's `experiment.toml`, runs the build and
test for each, looks for a `bench` binary, and writes
`results/run-<timestamp>/{results.json,RESULTS.md}` plus a `latest` symlink.

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
- Patterns: [`../../docs/patterns.md`](../../docs/patterns.md) (P12, P13, P14)
- Lifecycle: [`../../docs/lifecycle.md`](../../docs/lifecycle.md)
- Index: [`../../docs/index.md`](../../docs/index.md)
