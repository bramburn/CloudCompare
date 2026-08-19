# Scenario: LAS file parser strategies (2026-08-19)

> Test 2 pure-Rust strategies for reading `.las`/`.laz` files.
> Goal: replace CCCoreLib's reliance on `LASzip` (C++) for LAS/LAZ I/O.

## Why this matters

CCCoreLib's LAS reading goes through `LASzip` (a C++ library). Phase 4
of the migration is to replace that with a pure-Rust parser. Two
realistic options:

1. **`las` crate** — pure-Rust, no FFI, handles `.las` v1.0–1.4
2. **`laz` crate** — pure-Rust LAZ (compressed) support, often used with `las`

(There's also `pcd-rs`, `nom_las`, etc., but `las` is the most-used.)

## What we are NOT testing

- **LASzip C++ direct** — the whole point of the migration is to drop
  LASzip as a dependency. Not in scope.
- **`pointcloud-rs` (bindings to PDAL)** — out of scope; PDAL is huge.
- **Streaming parsers** — `.las` files for survey data are often 100MB+;
  for now we test the load-all-into-memory path. Streaming is a
  Phase 5+ concern.

## Test data

A `.las` file in `D:\82 BROOK AVENUE\output\` (or similar path on
this machine). The first variant just reads the header and counts
points; a second pass reads all points to verify the parser works.

## Workflow

1. **Build 01-pure-rust-las** with the `las` crate.
2. **Time the read** — header parse + point stream.
3. **Compare with CCCoreLib's load time** (when we have a working
   CXX bridge).
4. **Document winner** in `decisions.md`.

## Related

- Top-level: [`../../docs/decisions.md`](../../docs/decisions.md) (D6 TBD)
- Phase 4 roadmap: [`../../../PRD/rust/05-roadmap.md` Phase 4](../../../PRD/rust/05-roadmap.md)
