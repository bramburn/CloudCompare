# 01-sequential — ScalarField strategy 1 (baseline)

Sequential single-threaded ScalarField statistics. The baseline.

## Quick start

```powershell
cargo test
cargo bench --bench bench
```

## What it does

- One loop per statistic: `mean`, `std_dev`, `min_max`, `rms`, `valid_count`
- Per-element `is_finite()` filter (NaN + infinity are dropped)
- Population std (no Bessel correction) — matches CCCoreLib

## See also

- `AGENTS.md` — what this is
- [`../`](../) — the scenario
