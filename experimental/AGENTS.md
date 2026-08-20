# Experimental Workspace — Agent Instructions

This file is the **contract for how an LLM agent must work inside `experimental/`**.
It is read in addition to the root `AGENTS.md`. It overrides nothing in root — it adds
the experimental-specific rules.

## Purpose

`experimental/` is a **scratch space for full builds**: small, disposable applications
and libraries the agent (or you) can use to test specific behaviours end-to-end without
modifying the main CloudCompare code.

Use it to:

- Prototype Rust ports of CCCoreLib before touching `cc-rust/`
- Wire CXX FFI bridges without breaking the main build
- Visualise point clouds in a Qt GUI that links to Rust
- Compare 2–3 implementation approaches for the same problem and pick a winner
- Capture decisions ("we tried A and B, picked B because…") so we don't re-learn

## Structure

```
experimental/
├── templates/             ← pre-scaffolded, minimal, buildable. Copy or extend.
│   ├── rust_lib/          pure-Rust library
│   ├── rust_cxx_app/      Rust + CXX FFI (CXX opt-in via feature flag)
│   ├── cpp_qt_console/    Qt 6 console
│   └── cpp_qt_gui/        Qt 6 desktop view with 3D OpenGL viewport
├── shared/                ← cross-cutting helpers (vcvars capture, scripts)
├── sessions/              ← one folder per experiment, dated + named
├── scenarios/             ← one folder per A/B/C variant comparison
├── fixtures/              ← real-data manifests (paths + hashes, never the data)
├── docs/                  ← index, patterns, decisions, lifecycle, promotion
└── run.ps1                ← single command to run a scenario's variants
```

**Templates** are the starting point. Pick one. Don't reinvent the wheel.

**Sessions** are individual experiments. One session = one hypothesis.
Sessions can be deleted after graduation; their lesson lives on in `docs/decisions/`.

**Scenarios** are multi-session comparisons. If you're choosing between 3 ICP
implementations, put each in a session and the comparison in a scenario folder.

**Docs** are the durable memory. Files, each with a fixed purpose:

- `index.md` — catalogue of all sessions and scenarios (date, name, status, link)
- `patterns.md` — "how we do X" notes (e.g. "Rust ↔ Qt via CXX")
- `decisions.md` — "why we chose Y" notes (e.g. "we picked KD-tree over octree for ICP")
- `lifecycle.md` — status state machine, promotion process
- `decisions/<topic>.md` — long-form decision record (e.g. scalar-field formula parity)

## Status state machine (mandatory)

Every session and scenario declares its current state. The state controls what
claims are allowed. **Promote carefully** — measurement, not optimism, is the gate.

| State | What it means | Allowed claims |
|---|---|---|
| `scaffolded` | folder + Cargo.toml/CMakeLists + stub source. **Has never compiled.** | none — does not exist for benchmarks |
| `buildable` | `cargo build` or `cmake --build` succeeds. **No tests run yet.** | "the code compiles" |
| `unit-tested` | one or more `cargo test` / Qt Test cases pass. **No correctness vs reference.** | "the unit tests pass" |
| `reference-validated` | outputs match a reference implementation (brute-force, CCCoreLib, or known-answer) within tolerance. | "matches reference within ε" |
| `benchmarked` | timing/throughput numbers recorded with toolchain, profile, fixture, commit SHA. | "faster than X by Y at N points" |
| `selected` | named winner of a scenario, with a `decisions.md` entry. | "we picked this for N reason" |
| `graduated` | code moved to `cc-rust/` or production. | "this is now production" |
| `abandoned` | killed with reason. Kept for one release, then deleted. | (no claims — just the record) |

### Hard rules

- **Never** claim a performance number without `benchmarked` state.
- **Never** claim "matches reference" without `reference-validated` state.
- **Never** promote to `selected` unless `reference-validated` AND `benchmarked`.
- **Never** promote to `graduated` without an explicit `decisions.md` entry
  and a `promotion.md` describing what moves to production.

### Why this matters

A common failure mode for LLM agents is to record expected behaviour as if it
were measured. "O(log n) per query" without `benchmarked` is a design claim,
not a result. The status table makes this distinction machine-checkable.

## Evidence rules

1. **Never describe expected performance as measured performance.** Use
   "expected O(log n); implementation and performance not yet validated."
   for non-`benchmarked` work.
2. **A candidate is not selectable** until correctness is compared against a
   reference implementation (brute-force NN, a different tree, or
   CCCoreLib's output for the same input).
3. **Every result must record** the commit SHA, compiler toolchain, build
   profile, fixture identity, and command used. The `experiment.toml`
   schema enforces this.
4. **Real local datasets are referenced by manifest + hash**, never
   committed to git. See `fixtures/` for the format.
5. **Sentry is disabled by default** for experimental builds. To enable
   it for a specific run, set `SENTRY_DSN` in the environment.
6. **A production change requires an explicit promotion request** — see
   `docs/promotion.md`. The agent never silently copies experiment code
   into the main codebase.
7. **Incomplete implementations must be labelled `scaffolded` or
   `buildable`** in the session README. They cannot be benchmarked.

## Working rules

1. **Never touch `libs/`, `qCC/`, `ccViewer/`, `plugins/`** for experimental work.
   Go into `experimental/`. Always.
2. **Use a template, not a blank folder.** `cp -r templates/rust_lib sessions/2026-MM-DD-foo`.
3. **Session names: `<YYYY-MM-DD>-<short-kebab-topic>`.** Date stamps the experiment.
4. **Each session has a `README.md`** with: what, why, build/run, observations, next.
5. **Each session has a `cargo test` / `cmake --build` that passes** before you say "done".
   Broken templates and broken sessions are not acceptable — they pollute the
   "what works" knowledge.
6. **Scenarios have a `decisions.md`** at the scenario level summarising the winner
   and referencing each variant session.
7. **Index everything** in `docs/index.md`. If a session isn't in the index, it
   doesn't exist.
8. **Decisions are 1 page.** 5–15 bullets. If it's longer, link out to a session.
9. **No screenshots of point clouds in docs.** Point to the session that generates them.
10. **Aggressive cleanup.** Sessions that "never worked" can be deleted. Keep the
    decision record, not the wreckage.

## The experiment lifecycle (mandatory)

When you (the agent) are asked to "test X" or "prototype Y":

1. **Classify the request.** Which of:
   - `scratch` — one file, no persistent artefact
   - `experiment` — uncertain approach, isolated build
   - `scenario` — two or more viable approaches to compare
   - `parity` — compare Rust with CloudCompare/C++ on identical input
   - `integration` — real files, DLLs, CXX, Qt, or production-like environment
   - `promotion` — move a proven candidate toward the main codebase
2. **Search first.**
   - Read `docs/index.md`, `docs/patterns.md`, `docs/decisions.md`.
   - Look in `sessions/` and `scenarios/` for similar work.
   - If something close exists, propose **extending** it. Don't duplicate.
3. **Write the hypothesis.** Before scaffolding, capture in `README.md`:
   ```md
   ## Hypothesis
   Using `<approach>` will reduce `<metric>` by `<factor>` on
   `<fixture>` while preserving the reference's outputs within `<tolerance>`.
   ```
4. **Pick or create a template.**
   - `cpp_qt_console` — Qt CLI app
   - `cpp_qt_gui` — Qt GUI with 3D OpenGL viewport (links to Rust)
   - `rust_lib` — pure-Rust library crate
   - `rust_cxx_app` — Rust + CXX FFI app
5. **Scaffold the session.**
   - `cp -r templates/<chosen> sessions/<YYYY-MM-DD>-<topic>/`
   - Edit `README.md` and `AGENTS.md` (if present) for the session.
   - **Add `experiment.toml`** (see `templates/scenario/experiment.toml` for
     the schema). Without it, the session cannot be promoted to `benchmarked`.
6. **Implement minimally.** Throwaway, but buildable.
7. **Run gates.** Move through the status state machine in order:
   scaffolded → buildable → unit-tested → reference-validated → benchmarked.
   A `selected` decision requires all of these.
8. **Document.**
   - Update `docs/index.md` (one row).
   - If a reusable pattern emerged, append to `docs/patterns.md`.
   - If an architectural decision was made, append to `docs/decisions.md` AND
     the scenario-level `decisions.md`.
9. **Cross-link from root `AGENTS.md`** if this is a decision the main codebase
   should know about.

## Multi-variant scenarios (the "compare and pick" pattern)

When the user (or you) think there are 2–3 viable approaches:

```
scenarios/
└── 2026-08-19-icp-variants/
    ├── AGENTS.md               ← what we're comparing
    ├── experiment.toml         ← machine-readable contract
    ├── decisions.md            ← winner + rationale
    ├── 01-naive-on2/           ← first variant session
    ├── 02-kdtree-kiddo/        ← second variant
    └── 03-handrolled-octree/   ← third variant
```

Each `01-…`, `02-…`, `03-…` is a real session (a sibling of `experimental/sessions/`
in structure — same Cargo.toml, README.md, AGENTS.md rules, AND a per-variant
`experiment.toml` declaring the variant's status). The scenario folder just
groups them and holds the final `decisions.md`.

When the winner is picked:

- Update `docs/decisions.md` (top-level) with one paragraph.
- Reference the scenario from root `AGENTS.md` so the next migration effort finds it.

## Conventions

- **Language:** English for code comments, English for docs.
- **Build:** `cargo` for Rust, `cmake --build` for C++.
- **Logging:** `log` crate (Rust), `spdlog` (C++) — wired in templates.
- **Sentry:** wired in templates but disabled by default; only enabled
  with `SENTRY_DSN` in the environment.
- **Tests:** `cargo test`, `ctest`, or Qt Test — whichever fits the template.
- **Commits:** Conventional Commits with `experimental:` or `experiment():` scope:
  - `experiment(icp): add brute-force correctness baseline`
  - `experiment(icp): add kiddo nearest-neighbour candidate`
  - `benchmark(icp): compare nearest-neighbour variants`
  - `docs(experimental): record ICP candidate decision`

## Out of scope

- Production code. If you find yourself wanting to "refactor for production",
  move to `cc-rust/` or `plugins/`.
- Long-form documentation. If a decision needs more than 1 page, link to a session.
- CI integration. Experimental builds are local-only; CI is in `.github/workflows/`.
- Performance benchmarks in CI. Run them locally, record the result, move on.


## Known gotchas (must-read before benchmarking)

- **In-place mutation across N-sided benchmarks (P17).** When
  comparing N variants of an algorithm that mutates its input
  (ICP, multi-resolution ICP, anything that applies a transform
  to its data array), snapshot the input before each run.
  Otherwise the first run moves the data to the model and the
  remaining variants see data == model, RMS = 0, identity
  transform — *correct-looking* but wrong. Symptom: every
  variant reports converged=true after 2 iterations with
  RMS = 0.000001 and wildly different recovered transforms.
  Fix: clone the data template per variant. See pattern P17
  in docs/patterns.md for the full write-up and the
  sessions/2026-08-20-d8-realdata-all-nns/ test that
  surfaced this.

## D8 (NearestNeighbour trait) — active deliverable

The D8 trait + icp_with_nn entry point in cc-rust is the
foundation for pluggable NNs in ICP. The end-to-end D8
deliverable is in two places:

- **Synthetic Gaussian (sizes 2k / 5k / 10k / 50k):**
  scenarios/2026-08-20-icp-nn-comparison/ — kiddo is **43x
  faster than naive at 10k** and **~700x faster than the
  broken octree at 50k**. Naive is skipped at 50k (O(n^2)
  per iter is too slow).
- **Real data (brook-avenue 7.5M-point scan, 49,729-point
  subsample):** sessions/2026-08-20-d8-realdata-all-nns/ —
  all 3 NNs recover the 0.5m translation exactly; kiddo is
  **230x faster than naive** (0.22s vs 46s) and **880x faster
  than the broken octree** (0.22s vs 217s). This is the
  on-the-bench validation that the D8 trait dispatch works
  on real survey data, not just synthetic Gaussian.
## Quick checklist for an agent finishing a session

- [ ] Session builds green
- [ ] Tests pass
- [ ] `README.md` updated (what, why, build, run, observations, **status**)
- [ ] `experiment.toml` declares the variant and its current status
- [ ] `docs/index.md` has a row for this session
- [ ] If a pattern emerged, `docs/patterns.md` updated
- [ ] If a decision was made, `docs/decisions.md` updated
- [ ] Scenario decisions are also at the scenario level
- [ ] If benchmarked, results reference a fixture manifest, not a hard-coded path
- [ ] `git add` only source + docs (not `target/`, not `build/`, not `Cargo.lock`)

## Related

- **Root:** `AGENTS.md`
- **Templates:** `experimental/templates/AGENTS.md`
- **Knowledge base:** `experimental/docs/`
- **Lifecycle & promotion:** `experimental/docs/lifecycle.md`, `experimental/docs/promotion.md`
