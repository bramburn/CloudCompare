# 2026-08-20 — ICP NN comparison (D8)

> End-to-end comparison of the three ICP nearest-neighbour
> structures (naive, kiddo, hand-rolled octree) after the
> `NearestNeighbour` trait refactor in cc-rust.

## Status

**`benchmarked`** — all three variants produce identical ICP
results (correctness proven), NN query timings captured at 2k /
5k / 10k. Winner picked on per-query cost.

## What this is

Pre-D8, cc-rust's `icp_iterate` did not accept a custom NN, so
the three ICP variants under `2026-08-19-icp-variants/` could
not be compared end-to-end. D8 added the `NearestNeighbour`
trait and `icp_with_nn` entry point, and each variant now
implements that trait.

This scenario is the cross-variant benchmark that exercises that
new API on identical inputs (same seed, same Gaussian fixture,
same translation) at three sizes.

## How to run

```powershell
cd experimental\scenarios\2026-08-20-icp-nn-comparison
.\run.ps1
```

Outputs:

- `results.json` — one row per (variant, size) with build/test
  success, RMS, NN query µs, ICP wall time, iterations, converged.
- `RESULTS.md` — human-readable summary with the winner called out.

## Files

- `experiment.toml` — manifest (id, question, status, owner,
  fixtures, variants, metrics, promotion gates, tags).
- `AGENTS.md` — agent-facing scenario description (what to expect,
  how to run, how the winner is picked).
- `README.md` — this file (human-facing short version).
- `decisions.md` — winner pick + evidence (D8 entry).
- `run.ps1` — runner that builds and benches all three variants.

## See also

- `../2026-08-19-icp-variants/` — the three unit-tested
  implementations
- `../../../docs/decisions.md` D8 — the trait + dispatch decision
- `../../../docs/patterns.md` P16 — the trait + adapter pattern
- `../../../cc-rust/src/registration.rs` — the `NearestNeighbour`
  trait and `icp_with_nn` entry point
