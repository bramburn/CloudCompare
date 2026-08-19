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

## D4. 2026-08-19 — `kiddo` vs. hand-rolled octree for ICP

**Context:** ICP needs a fast nearest-neighbour search. The naive
O(n²) implementation in `registration.rs` works for tests but is too
slow for real `.las` data (10M+ points). Three options:

1. Naive O(n²) brute force
2. `kiddo` crate (KD-tree, pure Rust, well-tested)
3. Hand-rolled octree (matches CCCoreLib's `DgmOctree`)

**Decisions:** *pending — see `scenarios/2026-08-19-icp-variants/`.*

To be decided by the scenario once all three are implemented and
benchmarked.

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
