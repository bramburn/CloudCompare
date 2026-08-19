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
