# 02-parallel-rayon — ScalarField strategy 2

Parallel ScalarField statistics with rayon.

## Quick start

```powershell
cargo test
cargo bench --bench bench
```

## What it does

- Each statistic is a `par_iter().filter().map().reduce()` chain
- `compute_all_parallel()` does a single pass: sum + sum_sq + min + max + count
- Population std (no Bessel correction) — matches CCCoreLib

## See also

- `AGENTS.md`
- [`../`](../) — the scenario
