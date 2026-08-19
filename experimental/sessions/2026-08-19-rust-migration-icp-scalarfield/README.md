# 2026-08-19-rust-migration-icp-scalarfield

**Status:** Active — Phase 1 (scalar field) + Phase 2 (ICP) passing
**Template:** `experimental/templates/rust_lib/` (then extended with a CLI)
**Goal:** Pure-Rust port of CCCoreLib statistics + ICP, characterising behaviour against the C++ source before integrating into `cc-rust/`.

## Why this session exists

We are migrating CloudCompare's hot path (ScalarField statistics, ICP registration, octree/KD-tree) to Rust. Before touching `libs/qCC_db` or `cc-rust/`, we need:

1. A pure-Rust implementation we can unit-test headlessly
2. Characterisation tests that match CCCoreLib exactly (population variance, IEEE NaN semantics, mean of squares for RMS)
3. A clear migration path for the CXX FFI layer once CCCoreLib is built standalone

## Scope

| Module | Status | Notes |
|---|---|---|
| `scalar_field.rs` | ✅ Done | 17 tests pass, matches CCCoreLib formulas (population variance, NaN=quiet_NaN) |
| `registration.rs` | ✅ Done | SVD-based Horn ICP; 3 tests pass; nearest-neighbour is O(n) — replace with KD-tree |
| `octree.rs` | 🚧 Skeleton | Types defined; KD-tree stub only — needs full impl for ICP acceleration |
| `io.rs` | 🚧 Skeleton | CSV-only; needs `las` crate for `.las`/`.laz` |
| `lib.rs` | ✅ Done | Module declarations + sentry init |
| `main.rs` | ✅ Done | CLI entry point |

## Build & test

```powershell
cd C:\dev\CloudCompare\experimental\sessions\2026-08-19-rust-migration-icp-scalarfield
cargo test         # 28/28 pass
cargo run          # CLI: reads from data/, writes stats to stdout
```

## Characterisation against C++

The C++ source formulas we mirror are documented in `docs/decisions/2026-08-19-rust-scalarfield-formulas.md`:

- `ScalarField::computeMeanAndVariance()` uses **population variance** (no Bessel correction): `var = Σx²/n − mean²`
- `ValidValue(v) = std::isfinite(v)` — IEEE NaN is the invalid marker, NOT a sentinel like `-1.0e-30`
- `computeMeanSquareScalarValue()` returns `Σx²/n`; the caller takes `sqrt` to get RMS

## Open questions

1. **KD-tree implementation** — `kiddo` crate vs. hand-rolled octree?
2. **LAS reading** — `las` crate (pure Rust) vs. LASzip (FFI) vs. las-rs?
3. **CXX FFI surface** — which functions cross the C↔Rust boundary? See `experimental/sessions/2026-08-19-icp-variants/` for variant exploration.

## Related

- **PRD:** `PRD/rust/05-roadmap.md`, `PRD/rust/06-sandbox-plan.md`
- **Other sessions exploring variants:** `experimental/scenarios/2026-08-19-icp-variants/`
- **Decision records:** `experimental/docs/decisions/`

## Graduation criteria

When ALL of these hold, the code is ready to move to `cc-rust/`:

1. ✅ All unit tests pass (28/28)
2. ⏸ Characterisation tests vs. CCCoreLib match within 1e-6
3. ⏸ Benchmark within ±10% of C++ on `.las` test data
4. ⏸ Sentry is clean (no panics, no unwrap crashes)
5. ⏸ KD-tree replaces O(n) nearest-neighbour
