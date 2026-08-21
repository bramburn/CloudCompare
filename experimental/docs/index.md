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
| 2026-08-19 | [rust-migration-icp-scalarfield](../sessions/2026-08-19-rust-migration-icp-scalarfield/) | Done | `rust_lib` | Pure-Rust port of CCCoreLib ScalarField + ICP | 28/28 tests pass. Population std and NaN handling match CCCoreLib. **Graduated to `cc-rust/src/`.** |
| 2026-08-19 | [rust-realdata-icp](../sessions/2026-08-19-rust-realdata-icp/) | Superseded | `rust_lib` | Vanilla ICP on real data | Overshot 4× on the brook-avenue 0.5m translation. Superseded by `2026-08-20-realdata-icp-stack`. |
| 2026-08-20 | [realdata-icp-stack](../sessions/2026-08-20-realdata-icp-stack/) | Done | `rust_lib` | Recommended stack (coarse_align + multi_res + trimmed) on real data | **Recovers the 0.5m translation exactly on brook-avenue.** This is the canonical "production recipe." |
| 2026-08-20 | [d8-realdata-all-nns](../sessions/2026-08-20-d8-realdata-all-nns/) | Done | `rust_lib` | D8 trait dispatch on real data with all 3 NNs (naive, kiddo, hand-rolled octree) | All 3 NNs recover the 0.5m translation exactly. kiddo is **230× faster than naive** (0.22s vs 46s) and **1000× faster than the broken octree** (0.22s vs 217s) on 49k points. The trait dispatch is production-ready. |

## Scenario variants

A **scenario** is a folder of 2-3 (or more) sibling sessions that explore variants of the same problem, plus a `decisions.md` that picks a winner.

| Date | Name | Status | What |
|---|---|---|---|
| 2026-08-19 | [icp-variants/01-naive-on2](../scenarios/2026-08-19-icp-variants/01-naive-on2/) | Done | ICP with O(n²) brute-force NN. Baseline. 2/2 tests. Infeasible for real data (>2 hours per iter at 50k). |
| 2026-08-19 | [icp-variants/02-kiddo-kdtree](../scenarios/2026-08-19-icp-variants/02-kiddo-kdtree/) | Done | ICP with `kiddo` 6.0 KD-tree. **Winner.** 3/3 tests. Sub-µs per query. |
| 2026-08-19 | [icp-variants/03-handrolled-octree](../scenarios/2026-08-19-icp-variants/03-handrolled-octree/) | Done | ICP with hand-rolled octree. 4/4 tests. **Learning exercise** (no AABB pruning — much slower than brute force). |
| 2026-08-20 | [icp-variants/04-dgm-octree](../scenarios/2026-08-19-icp-variants/04-dgm-octree/) | Done | ICP with `DgmOctree` cell-code-ordered NN (D9). 2/2 tests. **Second-best at every size tested.** Production-quality, matches C++ `DgmOctree` semantics. |

## Scenarios

| Date | Name | Status | What |
|---|---|---|---|
| 2026-08-19 | [icp-variants](../scenarios/2026-08-19-icp-variants/) | **selected** | Compare ICP NN implementations: naive / `kiddo` / hand-rolled octree / `DgmOctree` (D9). **Winner: kiddo. Second: D9 (cell-code).** 4 variants re-benched end-to-end on 2k/5k/10k/50k Gaussian. |
| 2026-08-19 | [scalarfield-strategies](../scenarios/2026-08-19-scalarfield-strategies/) | **selected** | Compare 3 ScalarField strategies: sequential / rayon / portable-SIMD. **Winner: hybrid seq+rayon (15× speedup at 1M).** |
| 2026-08-19 | [las-parsers](../scenarios/2026-08-19-las-parsers/) | **selected** | Compare 2 LAS parser strategies: pure-Rust `las` crate / C++ LASzip. **Winner: pure-Rust `las` (7.2M pts/s, drops LASzip dep).** |
| 2026-08-20 | [icp-nn-comparison](../scenarios/2026-08-20-icp-nn-comparison/) | **benchmarked** | Cross-variant end-to-end ICP using the D8 trait dispatch. Re-benched after the 2026-08-21 code review. |

## Templates

| Template | What |
|---|---|
| [`rust_lib`](../templates/rust_lib/) | Pure-Rust library, no FFI |
| [`rust_cxx_app`](../templates/rust_cxx_app/) | Rust + CXX FFI (opt-in via `cxx-ffi` feature) |
| [`cpp_qt_console`](../templates/cpp_qt_console/) | Qt 6 console app, no GUI |
| [`cpp_qt_gui`](../templates/cpp_qt_gui/) | Qt 6 desktop view with 3D OpenGL viewport |
| [`scenario`](../templates/scenario/) | A/B/C variant comparison (includes `experiment.toml` schema) |

## Knowledge base

- [`SUMMARY.md`](SUMMARY.md) — one-page state of the Rust migration (best entry point)
- [`patterns.md`](patterns.md) — "how we do X" notes (P12-P18 are the latest from the D8/D9 work)
- [`decisions.md`](decisions.md) — "why we chose Y" notes (D8 and D9 are the latest)
- [`lifecycle.md`](lifecycle.md) — explicit status state machine (`scaffolded` → `buildable` → `unit-tested` → `reference-validated` → `benchmarked` → `selected` → `graduated`)
- [`promotion.md`](promotion.md) — promotion request template (move code from `experimental/` to `cc-rust/` or `plugins/`)

## Adding a row

When you create a session:

1. Add a row to the **Sessions** table.
2. Update the template column if you used a different one.
3. Link to the session's folder (not its `README.md` — README paths break easily).

When you graduate a session (move code to `cc-rust/`, `plugins/`, or the main codebase):

1. Change the Status to **Done** and add a "Graduated to" note in the Outcome column.
2. Remove the row from this index (the canonical home is now `cc-rust/`, not `experimental/`).

When you abandon a session:

1. Change the Status to **Abandoned** and link to the **decision** that explains why.

## See also

- Workflow: [`../AGENTS.md`](../AGENTS.md)
- Root: [`../../AGENTS.md`](../../AGENTS.md)
- Production status: [`SUMMARY.md`](SUMMARY.md) — the one-page state
