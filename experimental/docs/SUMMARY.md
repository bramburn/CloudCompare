# Rust Migration — Experimental Summary (2026-08-20)

> One-page state of the Rust migration. The most up-to-date
> record of "what was tried, what worked, what to do next."

## Status by phase (per `PRD/rust/05-roadmap.md`)

| Phase | Description | Status | Evidence |
|---|---|---|---|
| **0** | Infrastructure (Cargo workspace + CXX FFI) | ✅ Done | [`cc-rust/`](../../../cc-rust/) — 40/40 tests, builds on stable, CXX FFI opt-in |
| **1** | ScalarField statistics | ✅ Done | [`cc-rust/src/scalar_field.rs`](../../../cc-rust/src/scalar_field.rs) — characterise vs CCCoreLib formulas (D1) |
| **2** | ICP / Horn registration | ✅ Done (basic + extras) | [`cc-rust/src/registration.rs`](../../../cc-rust/src/registration.rs) — 8 ICP tests pass. ICP algorithm is correct (D4 fixes). **Trimmed ICP + multi-resolution ICP for partial overlap and coarse-to-fine convergence.** |
| **2.5** | Coarse pre-alignment | ✅ Done | [`cc-rust/src/coarse_align.rs`](../../../cc-rust/src/coarse_align.rs) — PCA-based alignment of principal axes. 2 tests. |
| **3** | DgmOctree (core surface) | ✅ Done (core) | [`cc-rust/src/dgm_octree.rs`](../../../cc-rust/src/dgm_octree.rs) — cell-code addressing, build, NN. 5 tests. **Full DgmOctree class is 3000+ lines of C++; this is the ICP-relevant surface.** |
| **4** | LAS / PLY parsers | ✅ Strategy selected | D6: pure-Rust `las` crate wins (7.2M pts/s on real data) |

## Decisions made (see [`decisions.md`](decisions.md))

- **D1:** Population std (no Bessel correction) — matches `ScalarField::computeMeanAndVariance`. IEEE NaN, not `-1e-30`. `computeMeanSquareScalarValue` returns `Σx²/n`, not RMS itself.
- **D2:** Pure-Rust first, CXX second. CXX is opt-in via `cxx-ffi` feature.
- **D3:** `experimental/` workspace structure is the right shape (templates, sessions, scenarios, docs).
- **D4 (revised 2026-08-19):** Three ICP bugs in series: (1) SVD rotation order, (2) NN search double-applied the cumulative transform, (3) test fixture was degenerate. Plus trimmed ICP, multi-resolution ICP, and absolute-RMS convergence. All fixed.
- **D4-original (SUPERSEDED):** "Naive ICP NN wins over hand-rolled octree 2-3×" — kept for the historical record.
- **D5:** ScalarField uses hybrid sequential (small N) + rayon (large N) — 15× speedup at 1M elements.
- **D6:** Pure-Rust `las` crate replaces `LASzip` C++ dep for Phase 4. 7.2M pts/s release throughput.
- **D7:** `cc-rust/` Cargo workspace created. 40/40 tests pass.

## Patterns documented (see [`patterns.md`](patterns.md))

- P1–P11: pre-existing (characterisation tests, MSVC setup, CXX, OpenGL, etc.)
- **P12 (2026-08-19):** SVD rotation order — nalgebra's `svd.v_t` is V^T, so Horn R = V·U^T = `v_t.transpose() * u.transpose()`. Using `v_t * u.transpose()` returns a mirror.
- **P13 (2026-08-19):** ICP with in-place data mutation must NOT apply the cumulative transform in the NN-search loop.
- **P14 (2026-08-19):** Test fixtures for SVD-based algorithms must be non-symmetric. The 8 cube corners are degenerate.
- **P15 (2026-08-20):** PCA pre-alignment is degenerate on near-symmetric clouds. Real-world scans are fine; for symmetric test clouds use an "L" shape.

## ICP features (cc-rust/src/registration.rs + coarse_align.rs + dgm_octree.rs)

- `icp_iterate(data, model, params)` — vanilla ICP, with the 4
  D4 fixes applied. 5 unit tests + 2 trimmed-ICP tests.
- `icp_iterate` with `params.outlier_rejection_fraction > 0.0`
  — trimmed ICP (Chetverikov 2005). Robust to partial overlap.
- `icp_multi_resolution(data, model, fractions, params)` —
  coarse-to-fine. Default schedule: `&[0.1, 1.0]`.
- `coarse_align(data, model) -> CoarseAlignResult` — PCA-based
  pre-alignment. See P15 for the symmetric-cloud limitation.
- `apply_transform_in_place(points, &[f64; 16])` — apply a 4×4
  column-major transform to an `[f32; 3]` cloud.
- `DgmOctree` — cell-code addressing (Morton-like), build, NN.
  Phase 3 core surface; full class is 3000+ lines of C++.
- `get_cell_pos`, `compute_cell_code`, `get_bit_shift`,
  `MAX_OCTREE_LEVEL` — the cell-code primitives the full
  DgmOctree is built on.

Recommended stack for real data:
**coarse_align → icp_multi_resolution (with outlier rejection)**.

## ICP variants scenario (`scenarios/2026-08-19-icp-variants/`)

Three variants, all with real NN implementations:

| Variant | NN query (2k Gaussian) | NN query (10k) | Status |
|---|---|---|---|
| `01-naive-on2` | ~50 µs/q | ~500 µs/q | `unit-tested` |
| `02-kiddo-kdtree` | **0.36 µs/q** | **0.39 µs/q** | `unit-tested` |
| `03-handrolled-octree` | 4.6 µs/q | 40.0 µs/q | `unit-tested` (NN) + `buildable` (integration) |

The full ICP wall time is similar across all three because
**cc-rust's `icp_iterate` does not yet accept a custom NN**. To
unlock the 10-100× ICP speedup that the KD-trees enable,
cc-rust needs a `NearestNeighbour` trait + `icp_with_nn` entry
point. **This is D8, the next architectural decision.**

## Real data results (2026-08-20)

End-to-end test on the brook-avenue `.las` (7.5M points,
subsampled to ~50k):

| Approach | Recovered tx | Error | RMS |
|---|---|---|---|
| Vanilla ICP (D4 fixed, no pre-align) | −0.48 | 0.4+ (overshoots) | 0.000008 |
| **Recommended stack** (coarse_align + multi_res + trimmed) | **−0.50** | **0.000000** | **0.000000** |

The recommended stack recovers the known 0.5m translation
**exactly** on real data. Vanilla ICP (post-D4 fixes, pre-
recommended-stack) overshoots by 4×. The session is
[`../sessions/2026-08-20-realdata-icp-stack/`](../sessions/2026-08-20-realdata-icp-stack/).

## Scenarios + Sessions

Scenarios:
- [`../scenarios/2026-08-19-icp-variants/`](../scenarios/2026-08-19-icp-variants/) — ICP NN strategies (3 variants, all real; **decision pending D8**)
- [`../scenarios/2026-08-19-scalarfield-strategies/`](../scenarios/2026-08-19-scalarfield-strategies/) — ScalarField strategies (3 variants; decision: hybrid seq+rayon)
- [`../scenarios/2026-08-19-las-parsers/`](../scenarios/2026-08-19-las-parsers/) — LAS parser strategies (decision: `las` crate)

Sessions:
- `../sessions/2026-08-19-rust-migration-icp-scalarfield/` — initial ICP + ScalarField port
- `../sessions/2026-08-19-rust-realdata-icp/` — vanilla ICP on real data (superseded)
- **`../sessions/2026-08-20-realdata-icp-stack/`** — recommended stack on real data (current)

## Templates available

[`../templates/`](../templates/)

- `rust_lib/` — pure-Rust library
- `rust_cxx_app/` — Rust + CXX FFI (CXX opt-in)
- `cpp_qt_console/` — Qt 6 console
- `cpp_qt_gui/` — Qt 6 desktop view with 3D OpenGL viewport
- `scenario/` — A/B/C variant comparison (new 2026-08-19, includes `experiment.toml` schema)

## Infrastructure additions (2026-08-19)

- [`../run.ps1`](../run.ps1) — single command to run all variants of a scenario. Builds, tests, benchmarks, writes `results.json` + `RESULTS.md`.
- [`../fixtures/`](../fixtures/) — manifest-based dataset references. Real data is referenced by SHA-256, never committed.
- [`../docs/lifecycle.md`](lifecycle.md) — explicit status state machine.
- [`../docs/promotion.md`](promotion.md) — promotion request template.

## Next concrete steps (in priority order)

1. **D8 — refactor cc-rust ICP to accept a custom NN** (unlocks
   end-to-end NN benchmark of kiddo vs octree vs naive on
   identical inputs)
2. **D9 — cell-code-ordered NN search in DgmOctree** (replaces
   the current brute-force `nearest_neighbor`)
3. **End-to-end real-data ICP with D8** — same test as
   2026-08-20-realdata-icp-stack but with the chosen NN
4. **Phase 0 → live CXX FFI** — when CCCoreLib standalone build
   is configured (separate effort, not blocked on us)

## See also

- Roadmap: [`../../PRD/rust/05-roadmap.md`](../../PRD/rust/05-roadmap.md)
- Decisions: [`decisions.md`](decisions.md)
- Patterns: [`patterns.md`](patterns.md)
- Lifecycle: [`lifecycle.md`](lifecycle.md)
- Promotion: [`promotion.md`](promotion.md)
- Index of sessions: [`index.md`](index.md)
- Canonical Rust crate: [`../../../cc-rust/`](../../../cc-rust/)
