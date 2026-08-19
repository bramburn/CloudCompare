# Session: 01-pure-rust-las (Phase 4 strategy 1)

**Scenario:** [`../`](../) — LAS file parser strategies
**Approach:** Pure-Rust `las` crate
**Status:** Code + CLI done, NOT YET tested on real .las data (requires real data path + las crate version compatibility check)

## What this is

Uses the [`las`](https://crates.io/crates/las) crate (pure-Rust, no FFI)
to read `.las` files. Replaces CCCoreLib's reliance on `LASzip` (C++).

## Build & run

```powershell
cd experimental/scenarios/2026-08-19-las-parsers/01-pure-rust-las
cargo build
.\target\debug\las_inspect.exe "D:\path\to\file.las"
.\target\debug\las_inspect.exe "D:\path\to\file.las" --load
```

## Limitations

- `.las` only (no `.laz` compression). The `las` crate doesn't read LAZ
  out of the box; need to add `laz` crate on top.
- All-into-memory: a 250MB `.las` → ~1-2 GB RAM.
- API is `Iterator`-based; no random access.

## When this wins

- Pure-Rust means no FFI bridge in the CXX call chain. The Rust crate
  is loaded once, C++ calls `read_all_xyz` via CXX, gets `Vec<f32>`
  back, populates `ccPointCloud`.
- No `LASzip` dependency. The build is smaller and easier to vendor.

## When this loses

- LAZ files (compressed): need the `laz` crate too. Combined
  `las + laz` is a known combination, but adds a dep.
- Streaming for very large files (>1 GB): would need a custom
  iterator that yields chunks.

## See also

- Scenario: [`../`](../)
- Sibling: [`../02-pure-rust-laz/`](../02-pure-rust-laz/) (TODO — same idea but with `laz` for compression)
- Phase 4 roadmap: [`../../../PRD/rust/05-roadmap.md` Phase 4](../../../PRD/rust/05-roadmap.md)
