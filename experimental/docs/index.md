# Experimental Sessions — Index

> Catalogue of every session and scenario. If a session isn't here, it doesn't exist.

## How to read this file

- **Date** — when the session was created (YYYY-MM-DD)
- **Name** — kebab-case, no spaces
- **Status** — Active / Done / Abandoned
- **Template** — which template it was based on
- **What** — one-line summary
- **Outcome** — what we learnt / what to do next

## Sessions

| Date | Name | Status | Template | What | Outcome |
|---|---|---|---|---|---|
| 2026-08-19 | [rust-migration-icp-scalarfield](../sessions/2026-08-19-rust-migration-icp-scalarfield/) | Active | `rust_lib` | Pure-Rust port of CCCoreLib ScalarField + ICP | 28/28 tests pass. Population std and NaN handling match CCCoreLib. |

## Scenarios

A **scenario** is a folder of 2–3 sessions that explore variants of the same problem, plus a `decisions.md` that picks a winner.

| Date | Name | Status | What |
|---|---|---|---|
| 2026-08-19 | [icp-variants](../scenarios/2026-08-19-icp-variants/) | Active | Compare 3 ICP implementations: naive O(n²) NN, `kiddo` KD-tree, hand-rolled octree |

## Templates

| Template | What |
|---|---|
| [`rust_lib`](../templates/rust_lib/) | Pure-Rust library, no FFI |
| [`rust_cxx_app`](../templates/rust_cxx_app/) | Rust + CXX FFI (opt-in via `cxx-ffi` feature) |
| [`cpp_qt_console`](../templates/cpp_qt_console/) | Qt 6 console app, no GUI |
| [`cpp_qt_gui`](../templates/cpp_qt_gui/) | Qt 6 desktop view with 3D OpenGL viewport |

## Knowledge base

- [`patterns.md`](patterns.md) — "how we do X" notes (RCP loader, Sentry, etc.)
- [`decisions.md`](decisions.md) — "why we chose Y" notes (KD-tree over octree, etc.)

## Adding a row

When you create a session:

1. Add a row to the **Sessions** table.
2. Update the template column if you used a different one.
3. Link to the session's `README.md` (the link target is the session folder, not its README — README paths break easily).

When you graduate a session (move code to `cc-rust/`, `plugins/`, or the main codebase):

1. Change the Status to **Done** and add a "Graduated to" note in the Outcome column.

When you abandon a session:

1. Change the Status to **Abandoned** and link to the **decision** that explains why.

## See also

- Workflow: [`../AGENTS.md`](../AGENTS.md)
- Root: [`../../AGENTS.md`](../../AGENTS.md)
