# 2026-08-19-rust-realdata-icp

End-to-end Rust ICP test on a real `.las` scan. Ties together:
- Phase 1 (ScalarField statistics)
- Phase 2 (ICP registration)
- Phase 4 (LAS file I/O)

## Status

Stub. See `AGENTS.md` for the plan.

## Quick start

```powershell
cargo run --release -- "D:\82 BROOK AVENUE\output\2026-08-13-09-46-35_82 brook avenue.splice.las"
```

## See also

- `AGENTS.md`
- [`../scenarios/2026-08-19-las-parsers/`](../scenarios/2026-08-19-las-parsers/) — Phase 4 strategy
- [`../scenarios/2026-08-19-icp-variants/`](../scenarios/2026-08-19-icp-variants/) — Phase 2 NN strategies
- [`../scenarios/2026-08-19-scalarfield-strategies/`](../scenarios/2026-08-19-scalarfield-strategies/) — Phase 1 strategies
- [`../../cc-rust/`](../../cc-rust/) — the Cargo workspace
