# Rust Migration — Experimental Summary (2026-08-20)

> One-page state of the Rust migration. The most up-to-date
> record of "what was tried, what worked, what to do next."

## Status by phase (per `PRD/rust/05-roadmap.md`)

| Phase | Description | Status | Evidence |
|---|---|---|---|
| **0** | Infrastructure (Cargo workspace + CXX FFI) | ✅ Done | [`cc-rust/`](../../../cc-rust/) — 35/35 tests, builds on stable, CXX FFI opt-in |
| **1** | ScalarField statistics | ✅ Done | [`cc-rust/src/scalar_field.rs`](../../../cc-rust/src/scalar_field.rs) — characterise vs CCCoreLib formulas (D1) |
| **2** | ICP / Horn registration | ✅ Done (basic + extras) | [`cc-rust/src/registration.rs`](../../../cc-rust/src/registration.rs) — 8 ICP tests pass. ICP algorithm is correct (D4 fixes). **New: trimmed ICP for partial overlap, multi-resolution ICP for coarse-to-fine convergence.** |
| **2.5** | Coarse pre-alignment | ✅ Done | [`cc-rust/src/coarse_align.rs`](../../../cc-rust/src/coarse_align.rs) — PCA-based alignment of principal axes. 2 tests. (See P15 for the symmetric-cloud limitation.) |
| **3** | DgmOctree / KdTree | ✅ All 3 variants real | All 3 ICP NN variants have working nearest-neighbour. NN timings captured. See "ICP variants scenario" below. |
| **4** | LAS / PLY parsers | ✅ Strategy selected | D6: pure-Rust `las` crate wins (7.2M pts/s on real data) |

## Decisions made (see [`decisions.md`](decisions.md))

- **D1:** Population std (no Bessel correction) — matches `ScalarField::computeMeanAndVariance`. IEEE NaN, not `-1e-30`. `computeMeanSquareScalarValue` returns `Σx²/n`, not RMS itself.
- **D2:** Pure-Rust first, CXX second. CXX is opt-in via `cxx-ffi` feature.
- **D3:** `experimental/` workspace structure is the right shape (templates, sessions, scenarios, docs).
- **D4 (revised 2026-08-19):** **Three bugs in series** in the ICP code: (1) SVD rotation order was V^T·U^T, not V·U^T; (2) NN search double-applied the cumulative transform; (3) test fixture was degenerate (8 cube corners → H rank-deficient → SVD returned a reflection). All three fixed. The 2026-08-20 follow-up added trimmed ICP (outlier rejection) and multi-resolution ICP (coarse-to-fine).
- **D4-original (SUPERSEDED):** "Naive ICP NN wins over hand-rolled octree 2-3×" was measured with the buggy ICP. Kept for the historical record.
- **D5:** ScalarField uses **hybrid sequential (small N) + rayon (large N)** — 15× speedup at 1M elements.
- **D6:** **Pure-Rust `las` crate replaces `LASzip` C++ dep** for Phase 4. 7.2M pts/s release throughput.
- **D7:** `cc-rust/` Cargo workspace created. 35/35 tests pass. CLI scaffold with `status`, `scalar-stats`, `icp` subcommands.

## Patterns documented (see [`patterns.md`](patterns.md))

- P1: Characterisation tests against CCCoreLib formulas
- P2: `vcvars64` env capture in PowerShell
- P3: Windows SDK lib path for `Qt6::OpenGLWidgets` (d3d11.lib)
- P4: CXX FFI requires `unsafe extern "C++"` for safe-to-call functions
- P5: CXX auto-generated header at `<crate>/src/lib.rs.h`
- P6: `crate-type = ["staticlib", "rlib"]` for hybrid crates
- P7: CXX FFI is opt-in (feature flag) for portability
- P8: Experimental session lifecycle (now formalised — see [`lifecycle.md`](lifecycle.md))
- P9: `glEnable(GL_PROGRAM_POINT_SIZE)` for point clouds in OpenGL Core
- P10: QOpenGLWidget mouse event delivery (focus policy, setMouseTracking)
- P11: Pan math: use camera right/up vectors, not MVP inversion
- **P12 (2026-08-19):** SVD rotation order — nalgebra's `svd.v_t` is V^T, so Horn R = V·U^T = `v_t.transpose() * u.transpose()`. Using `v_t * u.transpose()` returns a mirror.
- **P13 (2026-08-19):** ICP with in-place data mutation must NOT apply the cumulative transform in the NN-search loop.
- **P14 (2026-08-19):** Test fixtures for SVD-based algorithms must be non-symmetric. The 8 cube corners are degenerate.
- **P15 (2026-08-20):** PCA pre-alignment is degenerate on near-symmetric clouds. Real-world scans are fine; for symmetric test clouds use an "L" shape.

## ICP variants scenario (`scenarios/2026-08-19-icp-variants/`)

Three variants, all with real NN implementations:

| Variant | NN query (2k Gaussian) | NN query (10k) | Status |
|---|---|---|---|
| `01-naive-on2` | ~50 µs/q | ~500 µs/q | `unit-tested` |
| `02-kiddo-kdtree` | **0.36 µs/q** | **0.39 µs/q** | `unit-tested` |
| `03-handrolled-octree` | 4.6 µs/q | 40.0 µs/q | `unit-tested` (NN) + `buildable` (integration) |

The full ICP wall time is similar across all three because
**cc-rust's `icp_iterate` does not accept a custom NN**. The
NN timings above are measured standalone. To unlock the
10-100× ICP speedup that the KD-trees enable, cc-rust needs a
`NearestNeighbour` trait + `icp_with_nn` entry point. **This is
D8, the next architectural decision.**

## ICP features (cc-rust/src/registration.rs + coarse_align.rs)

- `icp_iterate(data, model, params)` — vanilla ICP, with the 4
  D4 fixes applied.
- `icp_iterate` with `params.outlier_rejection_fraction > 0.0`
  — trimmed ICP (Chetverikov 2005). Robust to partial overlap.
  Verified on the asymmetric-9 fixture: 9 inliers + 4 ghosts,
  vanilla overshoots to (7.99, -2.19, 3.45), trimmed (50% trim)
  recovers the correct offset.
- `icp_multi_resolution(data, model, fractions, params)` —
  coarse-to-fine. Default schedule: `&[0.1, 1.0]`. Returns the
  cumulative transform from original data to model.
- `coarse_align(data, model) -> CoarseAlignResult` — PCA-based
  pre-alignment. Matches principal axes via the same SVD-Horn
  formula as ICP. See P15 for the symmetric-cloud limitation.
- `apply_transform_in_place(points, &[f64; 16])` — apply a 4×4
  column-major transform to an `[f32; 3]` cloud.

Recommended stack for real data:
**coarse_align → icp_multi_resolution (with outlier rejection)**.

## Scenarios tested

- [`../scenarios/2026-08-19-icp-variants/`](../scenarios/2026-08-19-icp-variants/) — ICP NN strategies (3 variants, all real; **decision pending D8**)
- [`../scenarios/2026-08-19-scalarfield-strategies/`](../scenarios/2026-08-19-scalarfield-strategies/) — ScalarField strategies (3 variants; decision: hybrid seq+rayon)
- [`../scenarios/2026-08-19-las-parsers/`](../scenarios/2026-08-19-las-parsers/) — LAS parser strategies (1 variant tested, 1 stubbed; decision: `las` crate)

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

## Real data results (2026-08-19)

End-to-end test on `D:\82 BROOK AVENUE\output\2026-08-13-09-46-35_82 brook avenue.splice.las` (253 MB, 7.5M points):

| Phase | Result |
|---|---|
| 4 (LAS read, 7.5M points) | 1.23 s in release, 7.2M pts/s |
| 1 (ScalarField stats) | X mean=−9.16, rms=11.08 |
| 2 (ICP, 50k subsample, vanilla) | 4.3 s; **recovered translation −0.48 vs expected +0.5 — overshoots**. Test was before the ICP bug fixes. |

The pipeline runs end-to-end on real data. The next concrete
real-data win is the recommended stack above (coarse_align +
multi-resolution + trimmed ICP) on the brook-avenue fixture.

## Next concrete steps (in priority order)

1. **D8 — refactor cc-rust ICP to accept a custom NN** (unlocks
   end-to-end NN benchmark of kiddo vs octree vs naive on
   identical inputs)
2. **End-to-end real-data ICP** with the recommended stack on
   the brook-avenue fixture; record before/after numbers
3. **Phase 0 → live CXX FFI** — when CCCoreLib standalone build
   is configured (separate effort, not blocked on us)
4. **Phase 3: DgmOctree** — full Rust port of `libs/qCC_db/extern/CCCoreLib/src/DgmOctree.cpp`
   once Phase 2 is fully benchmarked

## See also

- Roadmap: [`../../PRD/rust/05-roadmap.md`](../../PRD/rust/05-roadmap.md)
- Decisions: [`decisions.md`](decisions.md)
- Patterns: [`patterns.md`](patterns.md)
- Lifecycle: [`lifecycle.md`](lifecycle.md)
- Promotion: [`promotion.md`](promotion.md)
- Index of sessions: [`index.md`](index.md)
- Canonical Rust crate: [`../../../cc-rust/`](../../../cc-rust/)
