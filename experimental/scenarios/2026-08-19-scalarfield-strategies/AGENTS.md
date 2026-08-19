# Scenario: ScalarField strategies (2026-08-19)

> Test 3 implementation strategies for ScalarField statistics. Pick the winner.
> The "right answer" comes from CCCoreLib (we characterise against the C++).

## Phase 1 of the Rust migration roadmap

This scenario tests three different ways to compute the same ScalarField
statistics (mean, std, min/max, rms):

1. **01-sequential** — single-threaded baseline (current `experimental/sessions/2026-08-19-rust-migration-icp-scalarfield/src/scalar_field.rs`)
2. **02-parallel-rayon** — parallel fold/reduce with rayon
3. **03-portable-simd** — explicit SIMD via the `std::simd` (portable SIMD, stable in Rust 1.78+)

## The variants

| # | Folder | Approach | Why test it |
|---|---|---|---|
| 01 | `01-sequential/` | Single-threaded loops, `is_finite()` check per element | Baseline; matches our existing `scalar_field.rs` |
| 02 | `02-parallel-rayon/` | `par_chunks` + per-chunk fold + reduce | Scales with cores; could give 4-8x on 1M-point scalars |
| 03 | `03-portable-simd/` | `std::simd::Simd<f32, 8>` (AVX2) for the body, scalar for the is_finite check | SIMD gives 2-4x for hot loops on modern CPUs |

## What's a "win" here?

For **Phase 1 of the Rust migration**, the criteria (per the roadmap)
are:

1. **Match CCCoreLib's results to 1e-6** (functional correctness)
2. **Performance within ±10% of C++** (or better)
3. **Code clarity** — Rust is supposed to be a productivity win, not a maintenance burden

So the winner isn't "fastest" — it's "fastest that still passes
correctness + has a clean implementation that the next maintainer
won't fight."

## Workflow

1. **Run 01-sequential** — confirms the baseline. Already passes
   characterisation against CCCoreLib.
2. **Run 02-parallel-rayon** — measure speedup on 1M points.
3. **Run 03-portable-simd** — measure SIMD speedup; document any
   portability issues (the `std::simd` API is still stabilising).
4. **Write `decisions.md`** with the winner, the speedup table, and
   any "we'll wait for std::simd stabilisation" notes.
5. **Update `docs/decisions.md` (top-level)** with one paragraph
   summary.
6. **Update root `AGENTS.md`** so the next migration effort knows
   "the canonical scalar field implementation is at X."

## What we are NOT comparing

- **Algorithm variants** (numerical stability, Bessel correction, etc.)
  — those are already settled in [`../../docs/decisions/2026-08-19-rust-scalarfield-formulas.md`](../../docs/decisions/2026-08-19-rust-scalarfield-formulas.md).
- **CXX FFI surface** — the function signatures are fixed by the
  C++ side. We can change the internals freely; the extern "Rust"
  block stays the same.

## Related

- Top-level decision: [`../../docs/decisions.md`](../../docs/decisions.md) (D5 TBD)
- Phase 1 roadmap: [`../../../PRD/rust/05-roadmap.md` Phase 1](../../../PRD/rust/05-roadmap.md)
- Existing impl: [`../../../sessions/2026-08-19-rust-migration-icp-scalarfield/src/scalar_field.rs`](../../../sessions/2026-08-19-rust-migration-icp-scalarfield/src/scalar_field.rs)
