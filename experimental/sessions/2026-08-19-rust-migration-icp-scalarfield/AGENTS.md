# Session AGENTS — 2026-08-19-rust-migration-icp-scalarfield

This file is the **session-level contract**. It overrides defaults from `experimental/AGENTS.md` only where explicitly stated.

## What this session is

A pure-Rust port of the hottest path in CCCoreLib:

- `ScalarField` statistics (mean, std, min/max, RMS)
- ICP point-cloud registration (Horn 1987, SVD-based)
- Octree + KD-tree (for ICP acceleration and density queries)

**Goal:** Characterise Rust behaviour against CCCoreLib so we know the migration is faithful before touching the real codebase.

## What this session is NOT

- **Not a production crate.** No semver, no docs.rs, no public API stability.
- **Not a CloudCompare plugin.** No Qt, no CXX, no .rcp loading.
- **Not the final Rust implementation.** When it stabilises, it graduates to `cc-rust/`.

## Working rules

1. **Pure-Rust only.** No `unsafe` blocks. No CXX. No native FFI.
2. **Match CCCoreLib formulas exactly.** Read `libs/qCC_db/extern/CCCoreLib/src/ScalarField.cpp` and `RegistrationTools.cpp` before adding tests. Document the formula in the test comment.
3. **Tests must be characterised, not invented.** The "right answer" comes from the C++ source — not from statistical folklore.
4. **No silent NaN handling.** Every `is_finite()` check has a comment explaining what CCCoreLib's `ValidValue` does.
5. **Don't optimise prematurely.** The O(n) nearest-neighbour in `registration.rs` is fine for tests; replace with KD-tree only when needed.

## File map

| File | Purpose | Migration target |
|---|---|---|
| `src/scalar_field.rs` | Statistics: `mean`, `std`, `min_max`, `rms`, `compare` | `libs/qCC_db/src/ScalarField.cpp` |
| `src/registration.rs` | ICP: `icp_iterate` + SVD rotation | `libs/qCC_db/src/RegistrationTools.cpp` |
| `src/octree.rs` | Octree + KD-tree stubs | `libs/qCC_db/src/DgmOctree.cpp` |
| `src/io.rs` | CSV loading | (Phase 4) |
| `src/lib.rs` | Module exports + Sentry | (cross-cutting) |
| `src/main.rs` | CLI | (cross-cutting) |
| `cpp/las_loader.cc` | CXX bridge stub — disabled | re-enable when CCCoreLib is built standalone |

## When to update this file

- When the session graduates (delete this file, create `cc-rust/`).
- When a module's scope changes.
- When a new characterisable behaviour is added.

## Related

- **Workflow:** `experimental/AGENTS.md`
- **Templates:** `experimental/templates/rust_lib/`
- **PRD:** `PRD/rust/05-roadmap.md`
- **Scenario comparisons:** `experimental/scenarios/2026-08-19-icp-variants/`
