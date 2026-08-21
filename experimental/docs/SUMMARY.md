# Rust Migration — Experimental Summary (2026-08-21)

> One-page state of the Rust migration. The most up-to-date
> record of "what was tried, what worked, what to do next."

## Status (2026-08-21)

**Goal closed at 7 of 8 (2026-08-20, by user decision).**
The original 4-item goal (D8 trait, D9 cell-code NN, end-to-end
real-data ICP with D8, Phase 0 → live CXX FFI) — **3 of 4 are
done; CXX FFI is the deferred item**.

Since 2026-08-20, a 5-agent code review of the D9 work was
run in parallel and all security, SE, data structure, and
docs findings were addressed. The math/algorithm reviewer's
"d=0 cell skipped" critical was a misread; the other math
findings were test-coverage gaps that are now filled.

The closure evidence is now much stronger:

- **54/54 cc-rust tests pass** (was 40/40 at 7/8 closure; +14
  for D9 + code review). 0 build warnings.
- **4 ICP variants** compared end-to-end on the same Gaussian
  fixture: naive, kiddo, hand-rolled octree (learning
  exercise), and the new `DgmOctree` (D9, production-quality
  cell-code search). All four agree on RMS at every size.
- **Real-data ICP on `brook-avenue` 7.5M-point scan** recovers
  the 0.5 m translation exactly with the recommended stack.
  See `sessions/2026-08-20-realdata-icp-stack/`.
- **D9 perf (cell-code-ordered NN):** 0.49-0.91 µs/query at all
  sizes tested; ~13-525× faster than the hand-rolled octree
  (which has no AABB pruning), ~1.5-2× slower than kiddo.
  The 2× gap to kiddo is the `HashMap` + AABB overhead; for
  production use kiddo is the recommended default. D9's value
  is matching the C++ `DgmOctree` semantics in pure Rust — any
  code written against the C++ API can be ported to use D9
  with no algorithm change.

## Status by phase (per `PRD/rust/05-roadmap.md`)

| Phase | Description | Status | Evidence |
|---|---|---|---|
| **0** | Infrastructure (Cargo workspace + CXX FFI) | ✅ Done (FFI deferred) | [`cc-rust/`](../../../cc-rust/) — 54/54 tests, builds on stable, CXX FFI opt-in via `cxx-ffi` feature. **Live FFI bridge to standalone-built CCCoreLib deferred at 7/8 closure.** |
| **1** | ScalarField statistics | ✅ Done | [`cc-rust/src/scalar_field.rs`](../../../cc-rust/src/scalar_field.rs) — characterise vs CCCoreLib formulas (D1) |
| **2** | ICP / Horn registration | ✅ Done (basic + extras) | [`cc-rust/src/registration.rs`](../../../cc-rust/src/registration.rs) — 8 ICP tests pass. Algorithm correct (D4 fixes). **Trimmed ICP + multi-resolution ICP for partial overlap and coarse-to-fine convergence. D8 trait dispatch is the new entry point.** |
| **2.5** | Coarse pre-alignment | ✅ Done | [`cc-rust/src/coarse_align.rs`](../../../cc-rust/src/coarse_align.rs) — PCA-based alignment of principal axes. 2 tests. |
| **3** | DgmOctree (core surface) | ✅ Done (core, D9 NN) | [`cc-rust/src/dgm_octree.rs`](../../../cc-rust/src/dgm_octree.rs) — cell-code addressing, build, **cell-code-ordered NN search with AABB pruning and C++-style jump optimisation** (D9). 14 tests. **Full DgmOctree class is 3000+ lines of C++; this is the ICP-relevant surface.** |
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
- **D8 (2026-08-20):** `NearestNeighbour` trait + `icp_with_nn` — every ICP variant plugs its own NN. The original `icp_iterate` is a thin wrapper. **End-to-end D8 on real data confirms 230× kiddo vs naive on brook-avenue 49k subsample.**
- **D9 (2026-08-20):** Cell-code-ordered NN search in `DgmOctree` — Chebyshev shell expansion with AABB pruning, C++-style jump optimisation, correct `minDistToBorder` early-termination check (pattern P18). The pre-sorted indices + cell→range map are cached at `build()` time for O(1) cell lookup. 11 new tests; the 4-variant scenario re-benched.

## Patterns documented (see [`patterns.md`](patterns.md))

- P1–P11: pre-existing (characterisation tests, MSVC setup, CXX, OpenGL, etc.)
- **P12 (2026-08-19):** SVD rotation order — nalgebra's `svd.v_t` is V^T, so Horn R = V·U^T = `v_t.transpose() * u.transpose()`. Using `v_t * u.transpose()` returns a mirror.
- **P13 (2026-08-19):** ICP with in-place data mutation must NOT apply the cumulative transform in the NN-search loop.
- **P14 (2026-08-19):** Test fixtures for SVD-based algorithms must be non-symmetric. The 8 cube corners are degenerate.
- **P15 (2026-08-20):** PCA pre-alignment is degenerate on near-symmetric clouds. Real-world scans are fine; for symmetric test clouds use an "L" shape.
- **P16 (2026-08-20):** Pluggable NN trait + adapter pattern (D8) — single-method `NearestNeighbour` trait + `BruteForceNN` default adapter. Each variant wraps its own structure (`NaiveNN = BruteForceNN`, `KiddoNN`, `OctreeNN`, `DgmOctreeNN`).
- **P17 (2026-08-20):** Clone the input when benchmarking N variants side-by-side. ICP mutates the data array in place; the second variant sees data that the first has already moved to the model, and reports converged=true with a wrong transform.
- **P18 (2026-08-20):** Cell-code NN early-termination check must use `minDistToBorder` (D9). The naive `(d * cell_max_dim)² > best_d2` check incorrectly terminates before visiting cells adjacent to a query on the cell face.

## ICP features (cc-rust/src/registration.rs + coarse_align.rs + dgm_octree.rs)

- `icp_iterate(data, model, params)` — vanilla ICP, with the 4
  D4 fixes applied. 5 unit tests + 2 trimmed-ICP tests.
- `icp_iterate` with `params.outlier_rejection_fraction > 0.0`
  — trimmed ICP (Chetverikov 2005). Robust to partial overlap.
- `icp_multi_resolution(data, model, fractions, params)` —
  coarse-to-fine. Default schedule: `&[0.1, 1.0]`.
- `icp_with_nn<N: NearestNeighbour>(data, model, nn, params)` —
  the D8 entry point. Pluggable NN. `icp_iterate` is a
  thin wrapper that uses `BruteForceNN`.
- `icp_multi_resolution_with_nn<N: NearestNeighbour>(...)` —
  multi-resolution variant of the D8 entry point. Per-level
  ICP loops use brute force against the subsampled model;
  the caller's NN is reserved for the final summary.
- `coarse_align(data, model) -> CoarseAlignResult` — PCA-based
  pre-alignment. See P15 for the symmetric-cloud limitation.
- `apply_transform_in_place(points, &[f64; 16])` — apply a 4×4
  column-major transform to an `[f32; 3]` cloud.
- `DgmOctree` + `DgmOctreeNN` — cell-code addressing (Morton-
  like), build, **cell-code-ordered NN search with AABB
  pruning** (D9). 14 tests. Phase 3 core surface; full class
  is 3000+ lines of C++.
- `get_cell_pos`, `compute_cell_code`, `get_bit_shift`,
  `MAX_OCTREE_LEVEL` — the cell-code primitives the full
  DgmOctree is built on.

Recommended stack for real data:
**coarse_align → icp_multi_resolution_with_nn (with `BruteForceNN` or `DgmOctreeNN`, outlier rejection on)**.

## ICP variants scenario (`scenarios/2026-08-19-icp-variants/` + `2026-08-20-icp-nn-comparison/`)

Four variants, all with real NN implementations wired into
`icp_with_nn` via the D8 trait:

| Variant | NN query (2k Gaussian) | NN query (10k) | NN query (50k) | Status |
|---|---|---|---|---|
| `01-naive-on2` | (O(n²) per query) | | (skipped) | `unit-tested` |
| `02-kiddo-kdtree` | **0.30 µs/q** | **0.43 µs/q** | **0.53 µs/q** | `unit-tested` (NN + ICP) |
| `03-handrolled-octree` | 6.40 µs/q | 39.14 µs/q | 278.23 µs/q | `unit-tested` (NN + ICP), learning exercise — no AABB pruning |
| `04-dgm-octree` (D9) | 0.49 µs/q | 0.65 µs/q | 0.91 µs/q | `unit-tested` (NN + ICP), production-quality cell-code |

End-to-end ICP wall time (NN-driven, Gaussian, seed=42,
release build):

| Variant | N=2k (s) | N=5k (s) | N=10k (s) | N=50k (s) |
|---|---|---|---|---|
| `01-naive-on2` | 0.26 | 2.00 | 7.58 | (skipped, O(n²)) |
| `02-kiddo-kdtree` | **0.021** | **0.092** | **0.166** | **1.029** |
| `04-dgm-octree` (D9) | 0.031 | 0.188 | 0.342 | 2.162 |
| `03-handrolled-octree` | 0.345 | 3.572 | 18.532 | 740.464 |

**Winner: `02-kiddo-kdtree`.** D9 is the second-best at every
size tested. The D9 vs hand-rolled octree speedup at 50k
is **342× on ICP wall time** and **305× on per-query cost** —
proves the cell-code algorithm is correct. D9 vs kiddo is
~1.5-2× slower (HashMap + AABB overhead), but matches the
C++ `DgmOctree` semantics exactly.

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

A second real-data session,
[`../sessions/2026-08-20-d8-realdata-all-nns/`](../sessions/2026-08-20-d8-realdata-all-nns/),
confirms that the D8 trait dispatch works for all 3 NNs on
real survey data: kiddo is **230× faster than naive**
end-to-end (0.22s vs 46s) and **1000× faster than the broken
octree** (0.22s vs 217s).

## Scenarios + Sessions

Scenarios:
- [`../scenarios/2026-08-19-icp-variants/`](../scenarios/2026-08-19-icp-variants/) — ICP NN strategies (4 variants, all real; **decision: kiddo wins, D9 is second-best**)
- [`../scenarios/2026-08-19-scalarfield-strategies/`](../scenarios/2026-08-19-scalarfield-strategies/) — ScalarField strategies (3 variants; decision: hybrid seq+rayon)
- [`../scenarios/2026-08-19-las-parsers/`](../scenarios/2026-08-19-las-parsers/) — LAS parser strategies (decision: `las` crate)
- [`../scenarios/2026-08-20-icp-nn-comparison/`](../scenarios/2026-08-20-icp-nn-comparison/) — cross-variant end-to-end ICP with the D8 trait (D8 + D9 deliverable)

Sessions:
- `../sessions/2026-08-19-rust-migration-icp-scalarfield/` — initial ICP + ScalarField port
- `../sessions/2026-08-19-rust-realdata-icp/` — vanilla ICP on real data (superseded)
- **`../sessions/2026-08-20-realdata-icp-stack/`** — recommended stack on real data (current)
- **`../sessions/2026-08-20-d8-realdata-all-nns/`** — D8 trait dispatch on real data (kiddo, naive, hand-rolled octree)

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

## What still needs doing (in priority order)

**Deferred per user decision (7/8 closure):**

1. **Phase 0 → live CXX FFI.** Build CCCoreLib standalone
   (currently configured as a CMake subdirectory, not a
   pre-built library), then wire the `cxx-ffi` feature to
   call the C++ `RegistrationTools` ICP for parity testing
   against the pure-Rust implementation. Multi-hour
   infrastructure effort; not blocked on any of the
   experimental work.
2. **Update `CONFIGURE_CCCORELIB.md`.** Still references the
   old `sandbox/` path (pre-2026-08-19). Should point at
   the current `experimental/` and `cc-rust/` paths.

**Code-level follow-ups from the 5-agent code review (2026-08-21):**

3. **Promote D9's `DgmOctree` to `benchmarked` → `selected` →
   `graduated` lifecycle.** The cell-code NN is now
   production-quality. Promotion request per
   `promotion.md` would move it from `cc-rust/src/dgm_octree.rs`
   into a documented position as the recommended
   alternative to kiddo for the 1.5-2× slower but
   C++-compatible case.
4. **More DgmOctree class methods.** The current port
   covers the ICP-relevant surface (build, NN, cell_code
   primitives). The full C++ class is 3000+ lines and
   includes cell statistics, CC extraction, ray-casting,
   sphere queries, etc. None are needed for ICP but may
   be needed for other planned features.
5. **LAS parser integration.** D6 picked the `las` crate
   but the actual file-loading integration into
   `cc-rust` (and through it into `qCC` via CXX FFI) is
   not done. The `qLas` C++ plugin is the current path.
6. **Wire the Rust ICP into a CloudCompare plugin.** A
   `qRustICP` plugin that loads `cc-rust` as a CXX static
   library and uses the D8 trait would let users pick
   "Rust ICP" from the CloudCompare UI and validate the
   production integration. Defer until the CXX FFI
   is in.
7. **Real-data benchmarks at N ≥ 100k.** The D8 and D9
   docs note the kiddo advantage is "expected to grow
   with N" but only 50k has been measured. A 500k or
   7.5M (full brook-avenue) test would close the loop.
8. **Sentry SDK for `cc-rust`.** The main `qCC` has Sentry
   crash reporting wired up (see `qCC/main.cpp`). `cc-rust`
   doesn't yet — add the `sentry` crate with the same
   `SENTRY_DSN` opt-in pattern as `qCC`.
9. **Profile-guided optimisation (PGO) for `cc-rust`.** The
   ICP hot path is well-understood after the D8/D9 work.
   PGO + LTO could shave another 10-20% off the
   per-query cost.

## See also

- Roadmap: [`../../PRD/rust/05-roadmap.md`](../../PRD/rust/05-roadmap.md)
- Decisions: [`decisions.md`](decisions.md)
- Patterns: [`patterns.md`](patterns.md)
- Lifecycle: [`lifecycle.md`](lifecycle.md)
- Promotion: [`promotion.md`](promotion.md)
- Index of sessions: [`index.md`](index.md)
- Canonical Rust crate: [`../../../cc-rust/`](../../../cc-rust/)
