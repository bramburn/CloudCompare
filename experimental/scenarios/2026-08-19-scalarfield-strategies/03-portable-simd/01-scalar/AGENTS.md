# Session: 03-portable-simd (ScalarField strategy 3)

**Scenario:** [`../`](../) — ScalarField strategies
**Approach:** Portable SIMD via `std::simd` (Rust 1.78+, nightly-only as of 2026-08 — see notes)
**Status:** Code written; `std::simd` is on nightly. **Not buildable on stable** — kept for the record.

## What this is

Uses `std::simd::Simd<f32, 8>` to process 8 f32s at a time. Falls back to
scalar for the tail and for any lane where the value isn't finite.

## ⚠️ Toolchain note

`std::simd` is **nightly-only** as of Rust 1.89 (2026-08-19). The
`#![feature(portable_simd)]` attribute is required. This strategy
is a future-looking option — when `std::simd` stabilises (likely
Rust 1.85 or later), this is the path forward.

For now, this variant is a **reference implementation** — it
documents what the SIMD path looks like but doesn't compile on the
default toolchain.

## When this wins (when stable)

- **Any size**: SIMD helps even on small inputs because the per-element
  overhead is amortised over 8 lanes.
- **x86_64 with AVX2**: full 8x speedup for the body loops.
- **aarch64 (Apple Silicon, ARM servers)**: NEON gives 4x (4-wide f32).
- **wasm-simd**: portable; runtime-detected.

## When this loses

- **Hardware without SIMD**: compilers fall back to scalar — same speed
  as 01-sequential, no penalty.
- **Cache misses**: SIMD doesn't help if the data doesn't fit in L1.

## See also

- Scenario: [`../`](../)
- Sibling: [`../../01-sequential/`](../../01-sequential/), [`../../02-parallel-rayon/`](../../02-parallel-rayon/)
- Rust tracking issue for `std::simd`: <https://github.com/rust-lang/rust/issues/86656>
