# Session: 01-sequential (ScalarField strategy 1)

**Scenario:** [`../`](../) — ScalarField strategies
**Approach:** Sequential single-threaded loops
**Status:** Implementation + tests done, benchmarked

## What this is

The baseline: one loop per statistic, `is_finite()` check, accumulate.
Mirrors the existing `experimental/sessions/2026-08-19-rust-migration-icp-scalarfield/src/scalar_field.rs`.

## Build & test

```powershell
cd experimental/scenarios/2026-08-19-scalarfield-strategies/01-sequential
cargo test
cargo bench --bench bench
```

## See also

- Scenario: [`../`](../) — the A/B/C comparison
- Sibling: [`../02-parallel-rayon/`](../02-parallel-rayon/), [`../03-portable-simd/`](../03-portable-simd/)
- Existing impl: [`../../../sessions/2026-08-19-rust-migration-icp-scalarfield/src/scalar_field.rs`](../../../sessions/2026-08-19-rust-migration-icp-scalarfield/src/scalar_field.rs)
