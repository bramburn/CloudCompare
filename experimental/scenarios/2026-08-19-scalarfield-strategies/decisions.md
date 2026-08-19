# Scenario Decision — ScalarField strategies (2026-08-19)

> Three strategies for ScalarField statistics. This is the WINNER + rationale.

## Variants

| # | Folder | Approach | Status |
|---|---|---|---|
| 01 | [`01-sequential/`](01-sequential/) | Single-threaded loops | Tests pass; benchmarked |
| 02 | [`02-parallel-rayon/01-scalar/`](02-parallel-rayon/01-scalar/) | `par_iter` + reduce | Tests pass; benchmarked |
| 03 | [`03-portable-simd/01-scalar/`](03-portable-simd/01-scalar/) | `std::simd` (portable SIMD) | **Stub** — `std::simd` is nightly-only on Rust 1.89; not buildable on stable. Code is here as a reference for when the API stabilises. |

## Benchmark results (1M f32 elements, compute_all = mean+std+min_max+rms+count)

| Strategy | 1k | 10k | 100k | 1M |
|---|---|---|---|---|
| 01-sequential | ~4.4 µs | ~51 µs | ~470 µs | **4.34 ms** |
| 02-parallel-rayon | ~36 µs | ~63 µs | ~103 µs | **296 µs** |
| 03-portable-simd | n/a | n/a | n/a | n/a (nightly) |

Speedup of rayon over sequential at 1M: **~15x**.

## Decision

**Winner: hybrid — sequential below 50K, rayon above.**

The "crossover" is between 10K (where rayon's thread overhead loses)
and 100K (where rayon's parallelism wins by 4.5x). A reasonable
threshold is **50K elements** — below that, sequential; above, rayon.

For `ccPointCloud::computeScalarFieldStatistics` and friends in
CCCoreLib, the typical scalar field has 100K-10M points. **Rayon
wins for the realistic workload.**

### Implementation notes for the hybrid

```rust
pub fn compute_all(values: &[f32]) -> Option<Stats> {
    if values.len() < 50_000 {
        // Sequential path (avoids rayon thread-pool overhead)
        compute_all_sequential(values)
    } else {
        // Parallel path (single pass, all stats)
        compute_all_parallel_rayon(values)
    }
}
```

The threshold is heuristic — fine-tune with a benchmark on real data
once the CXX bridge is live.

## Why not portable SIMD (yet)

`std::simd` is **nightly-only** as of Rust 1.89 (2026-08-19). The
`#![feature(portable_simd)]` gate is required. We have:

- Code written (8-wide SIMD body, scalar tail + is_finite)
- Tests written
- No build possible on stable

When `std::simd` stabilises (likely Rust 1.85+ based on the
tracking issue), this is the path forward for single-threaded
workloads. For multi-threaded + SIMD, combine with rayon: each
thread does SIMD on its chunk.

## What we'd do differently if doing this again

- Pick a single backend (rayon or SIMD, not both) and stick with it
  for the first cut. The hybrid is an optimisation, not a starting
  point.
- The 50K threshold should be a `const` tunable, not a magic number
  embedded in `if`. Make it the first thing the maintainer sees.
- For the FFI surface, expose only the `compute_all` entry point.
  Internally pick sequential vs parallel. This keeps the C++ side
  simple and lets the Rust side evolve.

## References

- Top-level: [`../../docs/decisions.md`](../../docs/decisions.md) (D5)
- Phase 1 roadmap: [`../../../PRD/rust/05-roadmap.md` Phase 1](../../../PRD/rust/05-roadmap.md)
- Pattern: [`../../docs/patterns.md`](../../docs/patterns.md) (TBD)
