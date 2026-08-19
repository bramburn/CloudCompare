# 01-pure-rust-las — LAS file parser strategy 1

Pure-Rust `.las` reader using the `las` crate. Phase 4 of the migration.

## Quick start

```powershell
cargo build
.\target\debug\las_inspect.exe "D:\path\to\file.las"
```

## Status

Code written, but **not yet tested on a real .las file** in this session.
The `las` crate API may have changed; if `cargo build` fails, the AGENTS.md
notes the likely cause.

## See also

- `AGENTS.md`
- [`../`](../) — the scenario
