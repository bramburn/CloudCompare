# 01-naive-on2 — ICP variant 1 (baseline)

O(n²) brute-force nearest-neighbour ICP. The baseline; every other variant
beats this on speed.

## Quick start

```powershell
cargo test
cargo run --release --bin icp_bench -- 2000 42
```

## What it does

- Same ICP algorithm as the main session: Horn 1987 SVD-based registration
- NN search: brute-force scan of all model points per query
- O(n × m) per iteration; O(n² × iters) for two equal clouds of size n

## See also

- `AGENTS.md` — observations, expected performance, when to use
- [`../`](../) — the scenario A/B/C
