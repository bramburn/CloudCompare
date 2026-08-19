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
├── shared/                ← cross-cutting CMake helpers, build scripts
├── sessions/              ← one folder per experiment, dated + named
├── scenarios/             ← one folder per A/B/C variant comparison
└── docs/                  ← index, patterns, decisions — the knowledge base
```

**Templates** are the starting point. Pick one. Don't reinvent the wheel.

**Sessions** are individual experiments. One session = one hypothesis.
Sessions can be deleted after graduation; their lesson lives on in `docs/decisions/`.

**Scenarios** are multi-session comparisons. If you're choosing between 3 ICP
implementations, put each in a session and the comparison in a scenario folder.

**Docs** are the durable memory. Three files, each with a fixed purpose:

- `index.md` — catalogue of all sessions and scenarios (date, name, status, link)
- `patterns.md` — "how we do X" notes (e.g. "Rust ↔ Qt via CXX")
- `decisions.md` — "why we chose Y" notes (e.g. "we picked KD-tree over octree for ICP")

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

1. **Search first.**
   - Read `docs/index.md`, `docs/patterns.md`, `docs/decisions.md`.
   - Look in `sessions/` and `scenarios/` for similar work.
   - If something close exists, propose **extending** it. Don't duplicate.
2. **Pick or create a template.**
   - `cpp_qt_console` — Qt CLI app
   - `cpp_qt_gui` — Qt GUI with 3D OpenGL viewport (links to Rust)
   - `rust_lib` — pure-Rust library crate
   - `rust_cxx_app` — Rust + CXX FFI app
3. **Scaffold the session.**
   - `cp -r templates/<chosen> sessions/<YYYY-MM-DD>-<topic>/`
   - Edit `README.md` and `AGENTS.md` (if present) for the session.
4. **Implement minimally.** Throwaway, but buildable.
5. **Build & test.** Provide exact commands. The session must end green.
6. **Document.**
   - Update `docs/index.md` (one row).
   - If a reusable pattern emerged, append to `docs/patterns.md`.
   - If an architectural decision was made, append to `docs/decisions.md` AND
     the scenario-level `decisions.md`.
7. **Cross-link from root `AGENTS.md`** if this is a decision the main codebase
   should know about.

## Multi-variant scenarios (the "compare and pick" pattern)

When the user (or you) think there are 2–3 viable approaches:

```
scenarios/
└── 2026-08-19-icp-variants/
    ├── AGENTS.md               ← what we're comparing
    ├── decisions.md            ← winner + rationale
    ├── 01-naive-on2/           ← first variant session
    ├── 02-kdtree-kiddo/        ← second variant
    └── 03-handrolled-octree/   ← third variant
```

Each `01-…`, `02-…`, `03-…` is a real session (a sibling of `experimental/sessions/`
in structure — same Cargo.toml, README.md, AGENTS.md rules). The scenario folder
just groups them and holds the final `decisions.md`.

When the winner is picked:

- Update `docs/decisions.md` (top-level) with one paragraph.
- Reference the scenario from root `AGENTS.md` so the next migration effort finds it.

## Conventions

- **Language:** English for code comments, English for docs.
- **Build:** `cargo` for Rust, `cmake --build` for C++.
- **Logging:** `log` crate (Rust), `spdlog` (C++) — wired in templates.
- **Sentry:** wired in templates; one DSN per repo (see root `AGENTS.md`).
- **Tests:** `cargo test`, `ctest`, or Qt Test — whichever fits the template.
- **Commits:** Conventional Commits with `experimental:` scope, e.g.
  `feat(experimental): add KD-tree ICP variant`. Reference the session in the body.

## Out of scope

- Production code. If you find yourself wanting to "refactor for production",
  move to `cc-rust/` or `plugins/`.
- Long-form documentation. If a decision needs more than 1 page, link to a session.
- CI integration. Experimental builds are local-only; CI is in `.github/workflows/`.
- Performance benchmarks in CI. Run them locally, record the result, move on.

## Quick checklist for an agent finishing a session

- [ ] Session builds green
- [ ] Tests pass
- [ ] `README.md` updated (what, why, build, run, observations)
- [ ] `docs/index.md` has a row for this session
- [ ] If a pattern emerged, `docs/patterns.md` updated
- [ ] If a decision was made, `docs/decisions.md` updated
- [ ] Scenario decisions are also at the scenario level
- [ ] `git add` only source + docs (not `target/`, not `build/`, not `Cargo.lock`)

## Related

- **Root:** `AGENTS.md`
- **Templates:** `experimental/templates/AGENTS.md`
- **Knowledge base:** `experimental/docs/`
