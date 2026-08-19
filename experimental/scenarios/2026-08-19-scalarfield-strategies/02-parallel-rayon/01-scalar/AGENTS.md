# Session: 02-parallel-rayon (ScalarField strategy 2)

**Scenario:** [`../`](../) — ScalarField strategies
**Approach:** Parallel `par_iter` + reduce with rayon
**Status:** Implementation + tests done, benchmarked

## What this is

Same ScalarField statistics, computed in parallel using rayon. Each
statistic becomes a `par_iter().filter().map().reduce()` chain.
The `compute_all_parallel` function does a single pass that yields
all four statistics + valid count.

## Build & test

```powershell
cd experimental/scenarios/2026-08-19-scalarfield-strategies/02-parallel-rayon/01-scalar
cargo test
cargo bench --bench bench
```

## When this wins

- **Many cores + large data** (1M+ points): rayon should give a 4-8x
  speedup on a typical 8-core CPU.
- **Filter-heavy workloads** (lots of NaN): the filter pass is
  embarrassingly parallel.

## When this loses

- **Tiny data** (< 10k points): thread pool spinup + chunk overhead
  eats the speedup.
- **Single-core machines** (CI runners, low-spec VMs): no parallel
  benefit, just overhead.

## See also

- Scenario: [`../`](../)
- Sibling: [`../../01-sequential/`](../../01-sequential/), [`../../03-portable-simd/`](../../03-portable-simd/)
