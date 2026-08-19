# Rust Migration — Experimental Summary (2026-08-19)

> One-page state of the Rust migration. The most up-to-date
> record of "what was tried, what worked, what to do next."

## Status by phase (per `PRD/rust/05-roadmap.md`)

| Phase | Description | Status | Evidence |
|---|---|---|---|
| **0** | Infrastructure (Cargo workspace + CXX FFI) | ✅ Done | [`cc-rust/`](../../../cc-rust/) — 30/30 tests, builds on stable, CXX FFI opt-in |
| **1** | ScalarField statistics | ✅ Done | [`cc-rust/src/scalar_field.rs`](../../../cc-rust/src/scalar_field.rs) — characterise vs CCCoreLib formulas (D1) |
| **2** | ICP / Horn registration | ✅ Done (basic) | [`cc-rust/src/registration.rs`](../../../cc-rust/src/registration.rs) — 5 ICP tests pass; SVD + double-transform + degenerate-fixture bugs fixed 2026-08-19; **naive-vs-octree benchmark needs re-run with corrected algorithm** (D4) |
| **3** | DgmOctree / KdTree | ⏳ Skeleton | [`cc-rust/src/octree.rs`](../../../cc-rust/src/octree.rs) — types only; KD-tree impl deferred |
| **4** | LAS / PLY parsers | ✅ Strategy selected | D6: pure-Rust `las` crate wins (7.2M pts/s on real data) |

## Decisions made (see [`decisions.md`](decisions.md))

- **D1:** Population std (no Bessel correction) — matches `ScalarField::computeMeanAndVariance`. IEEE NaN, not `-1e-30`. `computeMeanSquareScalarValue` returns `Σx²/n`, not RMS itself.
- **D2:** Pure-Rust first, CXX second. CXX is opt-in via `cxx-ffi` feature.
- **D3:** `experimental/` workspace structure is the right shape (templates, sessions, scenarios, docs).
- **D4 (revised 2026-08-19):** **Three bugs in series** in the ICP code: (1) SVD rotation order was V^T·U^T, not V·U^T; (2) NN search double-applied the cumulative transform; (3) test fixture was degenerate (8 cube corners → H rank-deficient → SVD returned a reflection). All three fixed. **Original "naive wins" benchmark is invalid until re-run with corrected code.**
- **D4-original (SUPERSEDED):** "Naive ICP NN wins over hand-rolled octree 2-3×" was measured with the buggy ICP. Kept for the historical record.
- **D5:** ScalarField uses **hybrid sequential (small N) + rayon (large N)** — 15× speedup at 1M elements.
- **D6:** **Pure-Rust `las` crate replaces `LASzip` C++ dep** for Phase 4. 7.2M pts/s release throughput.
- **D7:** `cc-rust/` Cargo workspace created. 28/28 tests pass. CLI scaffold with `status`, `scalar-stats`, `icp` subcommands.

## Patterns documented (see [`patterns.md`](patterns.md))

- P1: Characterisation tests against CCCoreLib formulas
- P2: `vcvars64` env capture in PowerShell (saved as `experimental/shared/scripts/get-vcvars.ps1`)
- P3: Windows SDK lib path for `Qt6::OpenGLWidgets` (d3d11.lib)
- P4: CXX FFI requires `unsafe extern "C++"` for safe-to-call functions
- P5: CXX auto-generated header at `<crate>/src/lib.rs.h`
- P6: `crate-type = ["staticlib", "rlib"]` for hybrid crates
- P7: CXX FFI is opt-in (feature flag) for portability
- P8: Experimental session lifecycle (now formalised — see [`lifecycle.md`](lifecycle.md))
- P9: `glEnable(GL_PROGRAM_POINT_SIZE)` for point clouds in OpenGL Core
- P10: QOpenGLWidget mouse event delivery (focus policy, setMouseTracking)
- P11: Pan math: use camera right/up vectors, not MVP inversion
- **P12 (new 2026-08-19):** SVD rotation order — nalgebra's `svd.v_t` is V^T, so Horn R = V·U^T = `v_t.transpose() * u.transpose()`. Using `v_t * u.transpose()` returns a mirror transformation.
- **P13 (new 2026-08-19):** ICP with in-place data mutation must NOT apply the cumulative transform in the NN-search loop — that double-transforms the data and breaks correspondences.
- **P14 (new 2026-08-19):** Test fixtures for SVD-based algorithms must be non-symmetric. The 8 cube corners are degenerate under axis-aligned translations (H is rank 2 in 3D). Add an off-axis point.

## Scenarios tested

- [`../scenarios/2026-08-19-icp-variants/`](../scenarios/2026-08-19-icp-variants/) — ICP NN strategies (3 variants; **decision SUPERSEDED** — needs re-bench)
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
- [`../docs/lifecycle.md`](lifecycle.md) — explicit status state machine: `scaffolded` → `buildable` → `unit-tested` → `reference-validated` → `benchmarked` → `selected` → `graduated`.
- [`../docs/promotion.md`](promotion.md) — promotion request template; gates the move from `experimental/` to `cc-rust/`.

## Real data results (2026-08-19)

End-to-end test on `D:\82 BROOK AVENUE\output\2026-08-13-09-46-35_82 brook avenue.splice.las` (253 MB, 7.5M points, LAS 1.4, point format 3):

| Phase | Result |
|---|---|
| 4 (LAS read, 7.5M points) | 1.23 s in release, 7.2M pts/s |
| 1 (ScalarField stats) | X mean=−9.16, rms=11.08 (matches expected indoor scan shape) |
| 2 (ICP, 50k subsample, 10 iters) | 4.3 s; recovered translation −0.48 vs expected +0.5 (overshoots — needs outlier rejection; **also test was before the ICP bug fixes**) |

The pipeline runs end-to-end on real data. Translation recovery is poor because of missing pre-processing (no outlier rejection, no coarse pre-alignment) — that's the next concrete step after this summary.

## Next concrete steps (in priority order)

1. **Re-benchmark ICP variants** with the corrected algorithm (D4-revised)
2. **Outlier rejection in ICP** — robust loss or fixed-trimmed ICP
3. **Coarse pre-alignment** — bounding-box alignment before ICP
4. **Multi-resolution ICP** — coarse-to-fine subsampling
5. **`kiddo` 6.0 API migration** — for N > 100k cases
6. **Phase 0 → live CXX FFI** — when CCCoreLib standalone build is configured
7. **Phase 3: DgmOctree** — full Rust port once Phase 2 characterisation is done
8. **Fix hand-rolled octree `nearest()` placeholder** in `cc-rust/src/octree.rs`

## See also

- Roadmap: [`../../PRD/rust/05-roadmap.md`](../../PRD/rust/05-roadmap.md)
- Decisions: [`decisions.md`](decisions.md)
- Patterns: [`patterns.md`](patterns.md)
- Lifecycle: [`lifecycle.md`](lifecycle.md)
- Promotion: [`promotion.md`](promotion.md)
- Index of sessions: [`index.md`](index.md)
- Canonical Rust crate: [`../../../cc-rust/`](../../../cc-rust/)
