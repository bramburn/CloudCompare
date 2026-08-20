# Decisions — "Why we chose Y"

> Architectural decisions made in the experimental workspace. Each decision
> is 5–15 bullets. If it's longer, link out to a session.

---

## D1. 2026-08-19 — CCCoreLib formulas for Rust ScalarField port

**Context:** Porting `ScalarField::computeMeanAndVariance()`,
`computeMeanSquareScalarValue()`, and `ValidValue()` to Rust. The test
suite initially failed because the formulas didn't match.

**Decisions:**

- **Population variance, not Bessel-corrected.** CCCoreLib's formula is
  `var = Σx²/n − mean²`. Rust was using `Σx²/(n-1)` (sample variance).
  Switched to match. Argument: CCCoreLib is the reference; sample
  variance is statistical folklore, not what CloudCompare computes.
- **`NAN_VALUE = std::numeric_limits<ScalarType>::quiet_NaN()`.** Real
  IEEE NaN, NOT a sentinel like `-1.0e-30`. The original test comment
  was wrong. Switched the test to use `f32::NAN`.
- **RMS = sqrt(Σx²/n).** `computeMeanSquareScalarValue` returns the
  mean of squares, not RMS itself. The caller takes sqrt. Documented
  the two-step pattern in `scalar_field.rs`.

**Source:** `sessions/2026-08-19-rust-migration-icp-scalarfield/`

**Verification:** `cargo test` in that session — all 28 tests pass.

---

## D2. 2026-08-19 — Pure-Rust first, CXX second

**Context:** We have a Rust crate that needs to call CCCoreLib functions
(ultimately, to load `.las` files and to compare against the C++
algorithms directly). The CXX FFI bridge had multiple issues during the
first attempt.

**Decisions:**

- **Build pure-Rust first, validate the algorithms, then add CXX.**
  Reasoning: the algorithm logic (ScalarField, ICP, octree) doesn't
  require C++. Pure-Rust lets us iterate fast (no MSVC, no link
  failures, <1s test runs). Once the algorithm is right, CXX is a
  thin transport layer.
- **CXX is opt-in via `cxx-ffi` feature flag.** Default build is
  pure-Rust and works on any toolchain. CXX is enabled with
  `cargo build --features cxx-ffi` and requires MSVC.
- **MSVC d3d11.lib fix is a CMake `add_link_options`, not a
  `target_link_libraries` workaround.** Reasoning: the issue is the
  linker search path, not a missing library. `add_link_options` is the
  right tool.

**Source:** `sessions/2026-08-19-rust-migration-icp-scalarfield/`,
`sessions/2026-08-19-convert-cxx-windows-bridge/` (TODO)

**Verification:** Pure-Rust test pass: 28/28. CXX build: TODO.

---

## D3. 2026-08-19 — `experimental/` workspace structure

**Context:** We need a place for throwaway builds that doesn't pollute
the main codebase. `sandbox/` was gitignored wholesale; we wanted
AGENTS.md files, decision records, and templates to be tracked.

**Decisions:**

- **Move `sandbox/` under `experimental/sessions/`.** The whole point
  of `experimental/` is the structure (templates, sessions, docs).
  Tracking the structure is part of the value.
- **Track AGENTS.md, templates/, docs/, and source code. Ignore
  build outputs.** Updated `.gitignore` with `**/target/`, `**/build/`,
  `**/.cargo/`, etc. — narrow rules, not blanket.
- **Templates live at `experimental/templates/<name>/`.** Each
  template has its own `AGENTS.md` and `README.md`. Templates are
  versioned alongside the main codebase.
- **Session names are `<YYYY-MM-DD>-<short-topic>`.** Date + topic
  reads naturally in `ls` output and on commits.
- **Scenarios group multiple sessions for A/B/C comparisons.**
  Each scenario is a folder under `experimental/scenarios/`
  containing 2-3 sibling session folders and a `decisions.md` that
  picks the winner.

**Source:** `experimental/AGENTS.md`, `experimental/templates/AGENTS.md`

**Verification:** `experimental/sandbox -> sessions/2026-08-19-rust-migration-icp-scalarfield/`
move succeeded; all 28 tests still pass.

---

## D4. 2026-08-19 — ICP algorithm correctness fixes (3 bugs in series)

**Context:** External review of the ICP implementation found three
distinct bugs, all in `cc-rust/src/registration.rs` and the variant
ICP code. This entry supersedes the original "naive wins" D4 below;
re-benchmarking with the corrected algorithm is in progress.

**Decisions:**

- **Bug 1 — SVD order (Horn 1987).** The rotation was computed as
  `svd.v_t * svd.u.transpose()`. nalgebra's `svd.v_t` is already V^T
  (the right-singular vectors transposed), so the correct Horn
  rotation R = V · U^T is `v_t.transpose() * u.transpose()`. The old
  code computed V^T · U^T = (U·V)^T, which is the wrong matrix. With
  this fix, the rotation matrix is correct in all tests.
- **Bug 2 — Double transformation.** The NN-search loop was
  applying the cumulative `(rotation, translation)` to the data
  *before* searching, even though the data array was already
  mutated in place. This double-transformation broke the
  correspondences after the first iteration, so the algorithm
  diverged as soon as the RMS dropped close to zero. Fix: use
  `data_points[i]` directly as the query, no extra transform.
- **Bug 3 — Degenerate test fixture.** The original 8-cube-corner
  fixture produces a rank-2 cross-covariance H under any axis-aligned
  translation. The SVD of H = diag(0, 2, 2) has no unique answer
  for the X axis, and nalgebra returned R[0,0] = -1 (a reflection).
  Replaced with an "L-shaped" 9-point cloud that has one off-axis
  point to make H full-rank.
- **Bug 4 — Convergence was delta-only, not absolute.** Without an
  absolute-RMS check, ICP kept iterating after the data was already
  at the model. The next iteration's H was again degenerate
  (data = model), and the SVD returned a reflection that moved the
  data away. Added `IcprParamsRust::min_rms_absolute` (default 1e-3)
  to stop early.

**Source:** `cc-rust/src/registration.rs` after 2026-08-19 22:00 UTC.

**Verification:** 5/5 ICP tests pass with the corrected code:
`translation_only`, `rotation_only`, `rotation_and_translation`,
`identity_converges_quickly`, `too_few_points`. The
`asymmetric-9.toml` fixture is the canonical test cloud; the
deprecated `cube-8.toml` is kept as a regression test for the
degenerate-H path.

**Re-benchmark (corrected ICP, 2026-08-19 23:00 UTC):** the naive
ICP variant `01-naive-on2` re-bench'd at 0.37 s / 2.4 s / 10.0 s
for 2k / 5k / 10k Gaussian points. RMS 0.0002 at 2k (converged);
0.06 at 5k and 10k (max-iter hit, not diverged — the algorithm is
correctly making progress but needs more iterations for larger N).
The variant now uses the corrected ICP via a `cc_rust` path
dependency; the "naive" in the name refers to the NN search, not
the surrounding algorithm.

**Status of the original "naive wins" claim:** SUPERSEDED. The
previous benchmark (0.36s vs 0.60s on 2k points) was run with the
*buggy* ICP. The new `01-naive-on2` benchmark above is correct,
but `02-kiddo-kdtree` and `03-handrolled-octree` are still
skeletons (no real KD-tree or octree `nearest()`), so a winner
cannot be picked yet. See the scenario-level
`decisions.md` for the re-bench plan.

**When to revisit:** `02-kiddo-kdtree` and `03-handrolled-octree`
need real implementations. Then re-bench all three on the same
fixture at 2k / 5k / 10k / 50k. The new numbers will inform the
final `selected` decision.

---

## D4-original. 2026-08-19 — ICP NN: naive wins (octree needs improvement) — SUPERSEDED

> Superseded by the entry above. Retained for the historical record.
> The numbers below were measured with the buggy ICP; do not use
> them to make a decision.

**Context:** ICP needs a fast nearest-neighbour search. The naive
O(n²) implementation in `registration.rs` works for tests but is too
slow for real `.las` data (10M+ points). Three options:

1. Naive O(n²) brute force
2. `kiddo` crate (KD-tree, pure Rust, well-tested)
3. Hand-rolled octree (matches CCCoreLib's `DgmOctree`)

**Decisions (under the original, buggy ICP):**

- **Naive wins for now.** On random Gaussian point clouds of
  2k–10k points, the naive implementation is **2–3× faster** than
  the hand-rolled octree (measured: 0.36s vs 0.60s on 2k, 2.4s vs
  4.1s on 5k, 9.2s vs 23.3s on 10k).
- **Reason:** the hand-rolled octree is naive (always recurses all 8
  children, no early-out, no per-leaf bounds). For larger N (>100k)
  it should still win; we haven't tested that range yet.
- **`kiddo` 6.0 deferred.** API migration is bounded (~50 lines)
  but not done in this round.
- **Default for `cc-rust` Phase 2: naive.** Fastest, smallest,
  easiest to verify. Add a `fast_nn_search` feature flag later.

**Source:** `scenarios/2026-08-19-icp-variants/decisions.md`

**Verification:** `cargo run --release --bin icp_bench -- N 42` for
N ∈ {2000, 5000, 10000}. Both variants give the same final RMS
(algorithm is identical) — only the wall time differs.

**Status:** SUPERSEDED. Re-bench with the corrected ICP before
upholding or reversing the "naive wins" pick.

---

## D5. 2026-08-19 — ScalarField parallel strategy: hybrid sequential + rayon

**Context:** For Phase 1 of the Rust migration (porting `ScalarField`),
the C++ implementation does its single-pass stats with a tight loop.
We tested 3 Rust strategies to find the best speedup.

**Decisions:**

- **Sequential below 50K elements, rayon above.** Rayon gives a
  **15x speedup on 1M elements** (4.34 ms → 296 µs) but **loses below
  10K** (thread-pool overhead exceeds the work).
- **Cross-over heuristic at 50K.** A small `if` in the public
  `compute_all` function picks the right backend.
- **`std::simd` deferred.** The portable SIMD path is nightly-only
  on Rust 1.89. Code is staged in `scenarios/2026-08-19-scalarfield-strategies/03-portable-simd/`
  for when the API stabilises.

**Source:** `scenarios/2026-08-19-scalarfield-strategies/decisions.md`

**Verification:** criterion benchmarks at 1k / 10k / 100k / 1M.

| Backend | 1k | 10k | 100k | 1M |
|---|---|---|---|---|
| 01-sequential | 4.4 µs | 51 µs | 470 µs | 4.34 ms |
| 02-parallel-rayon | 36 µs | 63 µs | 103 µs | **296 µs** |
| 03-portable-simd | n/a | n/a | n/a | n/a (nightly) |

---

## D6. 2026-08-19 — LAS file parser: pure-Rust `las` crate replaces `LASzip`

**Context:** Phase 4 of the Rust migration — replace
`libs/qCC_db/extern/CCCoreLib/src/LasOpenFilter.cpp` (which uses
`LASzip`, a C++ library) with a pure-Rust equivalent.

**Decisions:**

- **Use the `las` crate (pure-Rust).** 7.2M points/sec on a real
  253 MB `.las` (point format 3) — **at least competitive with
  LASzip**, often faster.
- **All-into-memory is fine for now** (max `.las` is 250 MB in our
  test corpus; in-memory ≈ 1 GB). Streaming variant is a follow-up
  if/when we hit 1+ GB files.
- **`.laz` (compressed) deferred** — add `laz` crate on top of
  `las` when we need it. None of the current survey data is LAZ.

**Source:** `scenarios/2026-08-19-las-parsers/decisions.md`

**Verification:** read `D:\82 BROOK AVENUE\output\2026-08-13-09-46-35_82 brook avenue.splice.las`
(7.5M points) in 1.04 s release; first point matches expected indoor-survey coords.

**Migration impact:**

- Removes `LASzip` from the CloudCompare build entirely (one less
  C++ dep to vendor, build, audit).
- The CXX FFI surface becomes `read_all_xyz(path: &Path) -> Vec<f64>`
  — single function, no extra C++ glue.
- The C++ side calls into Rust from `LasOpenFilter::loadFile`, gets
  back `Vec<f64>`, narrows to `float` per point, populates
  `ccPointCloud`.

---

## D7. 2026-08-19 — `cc-rust/` Cargo workspace created (Phase 0 done)

**Context:** The roadmap called for a `cc-rust/` directory at the
repo root with the Cargo workspace that becomes the production Rust
crate. Until now, all Rust code lived in `experimental/`.

**Decisions:**

- **Created `cc-rust/`** with `crate-type = ["staticlib", "rlib"]` so
  the C++ side can link against the staticlib and Rust binaries in
  the same crate can use the rlib directly.
- **CXX FFI gated behind a `cxx-ffi` feature** — same pattern as
  `experimental/templates/rust_cxx_app/`. Default is pure-Rust so
  `cargo test` works on any toolchain.
- **28/28 tests pass** in `cc-rust/` (copied from
  `experimental/sessions/2026-08-19-rust-migration-icp-scalarfield/`).
- **CLI scaffold** at `cc-rust/src/main.rs` with `status`,
  `scalar-stats`, `icp` subcommands (the latter two are stubs
  until Phase 0's live FFI is done).

**Source:** `cc-rust/docs/PHASES.md` — live status of all 4 phases.

**Next:** Add `cc-rust/` to the CMake build as a custom target
(`cc_rust_lib`) that runs `cargo build --release`. Hook the
resulting `.lib` into `CloudCompare.exe` linking. That's the
Phase 0 deliverable in the roadmap.

---

## D8. 2026-08-20 — `NearestNeighbour` trait + `icp_with_nn` (ICP NN pluggability)

**Context:** The three ICP variants under
`scenarios/2026-08-19-icp-variants/` (naive, kiddo, hand-rolled
octree) all wanted to plug their own NN into the ICP loop, but
cc-rust's `icp_iterate` signature did not accept a custom NN.
Each variant built its tree for timing and then fell back to
cc-rust's brute force for the actual ICP iteration — so the
"ICP wall time" column in the old `decisions.md` was measuring
cc-rust's brute force, not the variant's NN. The scenario was
"benchmarked" but could not be promoted to "selected" because
the wall-time numbers were not a real comparison.

**Decisions:**

- **Add a `NearestNeighbour` trait** to
  `cc-rust/src/registration.rs`:

  ```rust
  pub trait NearestNeighbour {
      fn nearest(&self, query: &[f32; 3]) -> (usize, f32);
  }
  ```

  Single method, object-safe, no lifetime. The index is the
  position of the matched point in the original model slice the
  structure was built from. The distance is squared (matches
  the RMS convention used everywhere else in the file).
- **Add `icp_with_nn<N: NearestNeighbour + ?Sized>(data, model,
  nn, params)`** as the new NN-driven entry point. The original
  `icp_iterate` is now a thin wrapper that uses a `BruteForceNN`
  adapter — so all 40 existing tests pass unchanged.
- **Add a `BruteForceNN<'a>` adapter** over an `&[f32]` model
  slice. This is the default NN for `icp_iterate` and is
  exported for callers who want to use brute force explicitly
  with the new entry point.
- **Add `icp_multi_resolution_with_nn`** that takes a
  caller-supplied NN. The per-level ICP loops use brute force
  against the *subsampled* model (rebuilding a fresh
  `BruteForceNN` per level) to avoid the match-the-NN-to-the-
  wrong-model bug that the obvious refactor would have
  introduced. The caller's NN is reserved for the final
  summary RMS, which is the only step that actually touches
  the full-resolution data. See the doc comment on
  `icp_multi_resolution_with_nn` for the rationale.
- **Each variant implements the trait:**
  - `01-naive-on2`: `pub type NaiveNN<'a> = BruteForceNN<'a>;`
    (zero-cost alias — naive has no per-instance state).
  - `02-kiddo-kdtree`: `KiddoNN` wraps the kiddo `KdTree` and
    adapts the f32 trait contract to the f64 kiddo internals.
  - `03-handrolled-octree`: `OctreeNN` wraps the `Octree` and
    delegates `nearest()` to `Octree::nearest`.
- **Each variant's `icp_iterate` now calls `icp_with_nn` with
  its own adapter.** Pre-D8 the wrapper fell back to cc-rust's
  brute force internally. Post-D8 the trait dispatch is real
  and the per-iteration NN search is genuinely the variant's
  NN.

**Source:**
- `cc-rust/src/registration.rs` — trait, adapter, refactored
  `icp_iterate` and `icp_multi_resolution`.
- `scenarios/2026-08-19-icp-variants/01-naive-on2/src/lib.rs` —
  naive adapter.
- `scenarios/2026-08-19-icp-variants/02-kiddo-kdtree/src/lib.rs` —
  kiddo adapter.
- `scenarios/2026-08-19-icp-variants/03-handrolled-octree/src/lib.rs` —
  hand-rolled octree adapter.
- `scenarios/2026-08-20-icp-nn-comparison/` — cross-variant
  end-to-end bench (D8 deliverable).

**Verification (D8 deliverable, 2026-08-20, run.ps1):**

- **43/43 tests pass** in `cc-rust` (was 40/40, +3 for D8
  trait tests: `icp_with_nn_matches_icp_iterate`,
  `icp_with_nn_dispatches_to_trait`,
  `icp_multi_resolution_with_nn_matches_legacy`).
- **All three variants agree on correctness** at every size
  tested (identical RMS, identical iteration count, identical
  converged flag) — the trait dispatch is correct.
- **End-to-end ICP wall time (NN-driven, Gaussian, seed=42):**

  | Variant | N=2k | N=5k | N=10k | Speedup @ 10k |
  |---|---|---|---|---|
  | `01-naive-on2` | 0.247 s | 2.03 s | 7.54 s | 1.0× |
  | `02-kiddo-kdtree` | **0.021 s** | **0.080 s** | **0.175 s** | **43×** |
  | `03-handrolled-octree` | 0.359 s | 3.34 s | 18.5 s | 0.41× |

  kiddo is **43× faster than naive at 10k** end-to-end. The
  hand-rolled octree is *slower* than naive because its
  `search()` falls back to depth-first traversal without AABB
  pruning (the per-child AABB isn't preserved through the
  recursion, so `min_dist_sq` can't fire at the internal-node
  level). The hand-rolled octree was a learning exercise for
  the DgmOctree port (D-phase 3), not a tuned implementation.
  The DgmOctree port in `cc-rust/src/dgm_octree.rs` will be
  the production-quality version; the D9 candidate is the
  cell-code-ordered NN search in that octree.

**Promotion of the scenario:** the scenario
`2026-08-19-icp-variants` can now move from `benchmarked` to
**`selected`**: the new comparison scenario
`2026-08-20-icp-nn-comparison` provides the end-to-end
numbers, the kiddo variant is picked as the winner, and
`scenarios/2026-08-19-icp-variants/decisions.md` will be
updated to reflect that.

**When to revisit:**
- D9 (cell-code-ordered NN in DgmOctree) is the next work
  item. The trait is ready; DgmOctree just needs an
  `impl NearestNeighbour` block.
- For real-data (N ≥ 100k) we have not yet measured; the
  speedup is expected to grow because kiddo's per-query cost
  is bounded while naive's is O(n).

---

## Adding a new decision

When you make an architectural decision in a session:

1. Append a section to this file with the format `## D<n>. <date> — <title>`.
2. State the **Context** (what was the problem?).
3. State the **Decisions** (what did we choose, and why? — 1 bullet each).
4. **Source** the session folder.
5. **Verify** with a one-line proof (test passed, benchmark was X, etc.).

If the decision is too long (>15 bullets), link to a session for the
detail and keep this entry to a 3-bullet summary.

---

## Out of scope here

- "Why use Rust at all" — that's in `../../PRD/rust/05-roadmap.md`.
- "Why a sandbox" — that's in `../../AGENTS.md` (this file's parent).
- Decisions about the main codebase — those go in commit messages
  and PR descriptions, not here.
