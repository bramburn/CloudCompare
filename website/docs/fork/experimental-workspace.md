---
title: Experimental workspace
sidebar_label: Experimental workspace
sidebar_position: 3
---

# Experimental workspace

`experimental/` is the **sandbox for new features**. Every
non-trivial idea — including all Rust work, all new plugins, any
new tool — is prototyped here first, validated, and only then
promoted to the main codebase (or, in the Rust case, to
`cc-rust/`).

## The structure

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

- **Templates** are the starting point. Pick one. Don't reinvent
  the wheel.
- **Sessions** are individual experiments. One session = one
  hypothesis. Sessions can be deleted after graduation; their
  lesson lives on in `docs/decisions/`.
- **Scenarios** are multi-session comparisons. If you're choosing
  between 3 ICP implementations, put each in a session and the
  comparison in a scenario folder.
- **Docs** are the durable memory. `patterns.md`, `decisions.md`,
  `lifecycle.md`, `promotion.md`, `SUMMARY.md`, `index.md`.
- **Fixtures** are real-data manifests (paths + hashes, never
  the data itself).

## The 8-state lifecycle

Every session and scenario declares its current state. The
state controls what claims are allowed. **Promote carefully** —
measurement, not optimism, is the gate.

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

**Hard rules:**

- **Never** claim a performance number without `benchmarked`.
- **Never** claim "matches reference" without `reference-validated`.
- **Never** promote to `selected` unless `reference-validated` AND
  `benchmarked`.
- **Never** promote to `graduated` without an explicit
  `decisions.md` entry and a `promotion.md` describing what moves
  to production.

The status table makes this distinction machine-checkable.

## What's in there right now

- **4 templates** built and tested:
  - `rust_lib` (pure-Rust library)
  - `rust_cxx_app` (Rust + CXX FFI, CXX opt-in via feature flag)
  - `cpp_qt_console` (Qt 6 console)
  - `cpp_qt_gui` (Qt 6 desktop view with 3D OpenGL viewport)
- **Sessions** for every Rust port + the real-data ICP bench.
  Recent ones:
  - `2026-08-19-rust-migration-icp-scalarfield/` (Phase 1+2 baseline)
  - `2026-08-19-rust-realdata-icp/` (Phase 2 on brook-avenue scan)
  - `2026-08-19-convert-cxx-windows-bridge/` (CXX FFI work)
  - `2026-08-20-realdata-icp-stack/` (recommended stack: coarse_align
    + multi-res + trimmed, on real data)
  - `2026-08-20-d8-realdata-all-nns/` (D8 trait dispatch on real
    data, all 3 NNs side by side)
- **Scenarios** for the A/B/C comparisons:
  - `2026-08-19-icp-variants/` (3 NN implementations, each a
    session, with a `decisions.md` picking the winner — kiddo)
  - `2026-08-20-icp-nn-comparison/` (cross-variant end-to-end
    bench on Gaussian at 2k/5k/10k/50k, plus the real-data
    cross-link)
  - `2026-08-19-scalarfield-strategies/` (sequential vs parallel
    vs SIMD)
  - `2026-08-19-las-parsers/` (pure-Rust `las` vs `LASzip` C++)
- **Fixtures** for the real data:
  - `synthetic/asymmetric-9.toml` (the canonical non-degenerate
    test cloud; 8 cube corners + 1 off-axis point)
  - `synthetic/cube-8.toml` (deprecated, kept as regression)
  - `real/brook-avenue-splice.toml` (the 7.5M-point scan)

## How a new feature is prototyped here

The lifecycle in
[`experimental/AGENTS.md`](https://github.com/bramburn/CloudCompare/blob/master/experimental/AGENTS.md)
walks through it step by step. Short version:

1. Classify the request (`scratch` / `experiment` / `scenario` /
   `parity` / `integration` / `promotion`).
2. Search first — read `docs/index.md`, `docs/patterns.md`,
   `docs/decisions.md`; look in `sessions/` and `scenarios/` for
   similar work; if something close exists, propose extending
   it.
3. Write the hypothesis in `README.md` before scaffolding.
4. Pick a template, scaffold the session, add `experiment.toml`.
5. Implement minimally.
6. Move through the status state machine: scaffolded → buildable
   → unit-tested → reference-validated → benchmarked.
7. Document — update `docs/index.md`, add to `patterns.md` if
   a reusable pattern emerged, add to `decisions.md` if an
   architectural decision was made.
8. If a production change is needed, write a `promotion.md`
   describing what moves and where.

## What's the relationship to the main codebase?

`experimental/` is for **prototypes**. The main codebase is
**production**. A promotion from `experimental/` to the main
codebase is gated by:

1. `status = selected` (or higher) in the session's
   `experiment.toml`.
2. A `promotion.md` describing what moves and where.
3. A `decisions.md` entry explaining why.
4. The agent never silently copies — the user approves the
   promotion.

For the Rust work, the destination is `cc-rust/` (not the main
C++ codebase). For new C++/Qt plugins, the destination is
`plugins/`. For documentation, the destination is `website/docs/`.

## When in doubt, work in `experimental/`

If you're not sure whether a change belongs in the main codebase
or in a sandbox, the answer is **the sandbox**. The cost of a
throwaway `experimental/` session is small (one folder, one
`experiment.toml`, one README). The cost of merging an
under-tested change into `qCC/` or `ccViewer/` is a long
rebuild and a hard-to-revert patch.
