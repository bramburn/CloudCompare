# 03-portable-simd — ScalarField strategy 3

Portable SIMD via `std::simd` (Rust 1.78+, **nightly-only** as of 2026-08).

## Status: stub (doesn't build on stable)

`std::simd` requires `#![feature(portable_simd)]`. This is currently
nightly-only. The code is here for the record and as a reference for
when the API stabilises.

## What it does (when stable)

- Process 8 f32s per cycle via `Simd<f32, 8>`
- Per-lane `is_finite` mask; non-finite lanes are zeroed before reducing
- Scalar fallback for non-multiple-of-8 tail

## See also

- `AGENTS.md` — full rationale + toolchain note
- [`../`](../) — the scenario
