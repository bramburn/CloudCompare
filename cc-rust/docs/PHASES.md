# cc_rust — Phase Status

This document tracks the migration status per `PRD/rust/05-roadmap.md`.

## Phase 0: Infrastructure ✅ scaffold complete

- ✅ Cargo workspace at `cc-rust/` (this directory)
- ✅ Build script: pure-Rust default, CXX FFI gated behind feature
- ✅ All four module folders scaffolded (`scalar_field`, `registration`, `octree`, `io`)
- ✅ CLI: `cc_rust_cli` with `scalar-stats`, `icp`, `status` subcommands
- ✅ `lib_name = "cc_rust"` (crate-type `staticlib` + `rlib`) — ready for C++ linkage
- ⏸ **CXX FFI live test** — pending CCCoreLib standalone build (see `CONFIGURE_CCCORELIB.md` in experimental/)

## Phase 1: ScalarField ✅ done (pure-Rust)

- ✅ `mean`, `std_dev`, `min_max`, `rms`, `valid_count`, `compute_all`
- ✅ All characterisation tests pass against CCCoreLib formulas
- ✅ Population std (no Bessel correction) — matches `ScalarField::computeMeanAndVariance`
- ✅ NaN handling: `ValidValue(v) = std::isfinite(v)` — IEEE NaN, NOT a sentinel
- ✅ 28/28 tests pass in `experimental/sessions/2026-08-19-rust-migration-icp-scalarfield/`
- ✅ **Hybrid parallel strategy selected**: sequential below 50K, rayon above
  (15x speedup on 1M elements — see `experimental/scenarios/2026-08-19-scalarfield-strategies/decisions.md`)

## Phase 2: Registration (ICP/Horn) ⏳ in progress

- ✅ Horn 1987 SVD-based ICP — done
- ✅ Tests: identity transform, small translation
- ⏸ Characterisation vs. CCCoreLib's `RegistrationTools::ICP` — pending
- ⏸ KD-tree acceleration — 3-variant A/B/C in `experimental/scenarios/2026-08-19-icp-variants/`
  (hand-rolled octree is provisional winner)

## Phase 3: DgmOctree / KdTree ⏳ skeleton

- ✅ Type definitions, KD-tree stub, octree stub
- ⏸ Full implementation pending Phase 2 characterisation results

## Phase 4: File parsers (LAS, PLY) ⏳ not started

- ⏸ Pure-Rust `las` crate (or `las-rs`) for `.las`/`.laz`
- ⏸ `ply-rs` for `.ply`
- ⏸ CXX bridge: `Vec<u8>` → `ccPointCloud`

## How to use

```powershell
# Pure-Rust build (works on any toolchain)
cd C:\dev\CloudCompare\cc-rust
cargo test              # 28/28 unit tests pass
cargo bench             # criterion benchmarks
cargo run -- status     # show phase status

# With CXX FFI (requires MSVC)
. C:\dev\CloudCompare\experimental\shared\scripts\get-vcvars.ps1
$env:CC = 'cl.exe'; $env:CXX = 'cl.exe'
cargo build --features cxx-ffi
```

## Rollback

This directory ships behind a CMake feature flag (`ENABLE_RUST_*`).
If Rust is broken, flip the flag and the original C++ is unchanged.
The C++ implementations are **never deleted** — they remain the
fallback as long as CloudCompare ships.

## Related

- Roadmap: [`../PRD/rust/05-roadmap.md`](../PRD/rust/05-roadmap.md)
- Decisions: [`../experimental/docs/decisions.md`](../experimental/docs/decisions.md)
- Active session: [`../experimental/sessions/2026-08-19-rust-migration-icp-scalarfield/`](../experimental/sessions/2026-08-19-rust-migration-icp-scalarfield/)
