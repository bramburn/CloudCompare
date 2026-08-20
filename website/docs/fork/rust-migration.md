---
title: Rust migration
sidebar_label: Rust migration
sidebar_position: 2
---

# Rust migration

The headline addition of the Icelabz Surveying fork. The goal is to
**port the compute-heavy algorithms of CloudCompare to pure Rust**,
validate them against the C++ reference, and bridge them back into
the C++/Qt app via CXX FFI. Phase-by-phase, with measurement
over optimism.

## Where the work lives

- **`cc-rust/`** at the repo root — the production Cargo
  workspace. Crate-type `["staticlib", "rlib"]` so the C++ side
  can link the staticlib and Rust binaries can use the rlib
  directly. CXX FFI is opt-in via the `cxx-ffi` feature flag.
- **`experimental/`** — the sandbox where the Rust work is
  prototyped. Templates (`rust_lib/`, `rust_cxx_app/`), sessions
  (one folder per experiment), scenarios (A/B/C variant
  comparisons).
- **`cc-rust/docs/PHASES.md`** — the live status of all 4
  phases. Updated as phases complete.

## What's ported (as of 2026-08-20)

| Phase | Module | Status | Tests |
|---|---|---|---|
| **Phase 0** | Cargo workspace + CXX FFI scaffold | ✅ done | 28/28 → 40/40 → **43/43** |
| **Phase 1** | `ScalarField` statistics (`mean`, `std`, `rms`, `min_max`, parallel `compute_stats`) | ✅ done (15× speedup at 1M via rayon) | parity vs CCCoreLib formulas |
| **Phase 2** | ICP / Horn registration (vanilla + trimmed + multi-resolution) | ✅ done | 8 ICP tests pass on the asymmetric-9 fixture |
| **Phase 2.5** | PCA coarse pre-alignment | ✅ done | 2 tests |
| **Phase 3** | `DgmOctree` cell-code core (the ICP-relevant surface) | ✅ done (core) | 5 tests |
| **Phase 4** | LAS / PLY parsers (pure-Rust `las` crate) | ✅ strategy selected | 7.2 M pts/s on real data |

**40 → 43 cc-rust unit tests pass.** The D8 trait refactor
(`NearestNeighbour` + `icp_with_nn`) made the three ICP variants
plug into the same ICP loop and end-to-end tested on the
brook-avenue 7.5M-point `.las`:

- `01-naive-on2` (brute force): 13 iters, 46s on 49,729 pts
- `02-kiddo-kdtree` (KD-tree): 13 iters, **0.22s** on 49,729 pts
- `03-handrolled-octree` (octree): 13 iters, 217s on 49,729 pts

All 3 NNs recover the 0.5m translation exactly. kiddo is
**230× faster than naive end-to-end on real data**.

## The trait + adapter pattern (D8)

The ICP loop now takes a `&dyn NearestNeighbour` so the NN is
pluggable:

```rust
pub trait NearestNeighbour {
    fn nearest(&self, query: &[f32; 3]) -> (usize, f32);
}

pub fn icp_with_nn<N: NearestNeighbour + ?Sized>(
    data: &mut [f32], model: &[f32], nn: &N, params: &IcprParamsRust,
) -> Result<IcprResultRust, IcprErrorRust>;
```

The default `BruteForceNN` adapter is the fallback; legacy
`icp_iterate` is now a thin wrapper that uses the trait. The
3 ICP variants each implement the trait by wrapping their own
NN. See [`patterns.md` P16](https://github.com/bramburn/CloudCompare/blob/master/experimental/docs/patterns.md)
and [D8 in `decisions.md`](https://github.com/bramburn/CloudCompare/blob/master/experimental/docs/decisions.md).

## What's next (deferred by user decision on 2026-08-20)

- **Phase 0 → live CXX FFI.** The CCCoreLib C++ side calling
  the Rust functions via a CXX bridge module. Requires a
  standalone-built CCCoreLib (multi-hour infrastructure setup).
  The CXX FFI surface is already designed; this is the
  CMake-side wiring. **Deferred** — the fork's other 7 of 8
  roadmap items are done, and the user accepted closure at 7/8.
- **D9 (cell-code-ordered NN in DgmOctree).** The DgmOctree
  cell-code core is in `cc-rust/src/dgm_octree.rs` but the
  NN search is still brute force. Cell-code-ordered descent
  with AABB pruning would close the gap with kiddo on the
  ICP wall time.
- **Larger-N bench** (N=100k, 1M) on real data. Current
  cap is N=50k (naive skipped).

## Build and test

```powershell
# Build pure-Rust only (default — works on any toolchain)
& 'C:\dev\CloudCompare\cc-rust>cargo build --release'

# Build with CXX FFI (requires MSVC + vcpkg)
& 'C:\dev\CloudCompare\cc-rust>cargo build --release --features cxx-ffi'

# Run all 43 tests
& 'C:\dev\CloudCompare\cc-rust>cargo test --release'
```

## How the work was validated

Each phase has a session and/or scenario in
`experimental/sessions/` and `experimental/scenarios/`. The
full chain is documented in
[`experimental/docs/SUMMARY.md`](https://github.com/bramburn/CloudCompare/blob/master/experimental/docs/SUMMARY.md)
and
[`experimental/docs/decisions.md`](https://github.com/bramburn/CloudCompare/blob/master/experimental/docs/decisions.md)
(15 patterns, 8 architectural decisions including the
corrected ICP algorithm, the D4 series of bug fixes, the
D5 ScalarField parallel strategy, the D6 LAS parser
selection, the D7 cc-rust workspace, and D8 the trait).
