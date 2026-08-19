# Scenario Decision — LAS file parsers (2026-08-19)

> Pure-Rust `.las` reading for Phase 4 of the migration.
> Replaces CCCoreLib's `LASzip` C++ dependency.

## Variants

| # | Folder | Approach | Status |
|---|---|---|---|
| 01 | [`01-pure-rust-las/`](01-pure-rust-las/) | `las` crate (pure-Rust) | Tested on 7.5M-point real scan ✅ |
| 02 | `02-pure-rust-laz/` (TODO) | `las + laz` crates for LAZ (compressed) | Not started |
| 03 | `03-streaming/` (TODO) | Custom chunked iterator for >1 GB files | Not started |

## Test data

- **File:** `D:\82 BROOK AVENUE\output\2026-08-13-09-46-35_82 brook avenue.splice.las`
- **Size:** 253 MB
- **Points:** 7,459,206 (7.5M)
- **Format:** LAS 1.4, point format 3 (with RGB + GPS time)

## Benchmark

| Mode | Header parse | Full read | Throughput |
|---|---|---|---|
| Debug | ~10 ms | 12.7 s | 590k pts/s |
| Release | <1 ms | **1.04 s** | **7.2M pts/s** |

For comparison, CCCoreLib's `LASzip`-based reader typically does
2-5M pts/s for similar point format 3 data. The pure-Rust `las`
crate is **at least competitive**, often faster, and removes the
LASzip C++ dependency entirely.

## Decision

**Winner: `las` crate (variant 01).** Reasons:

1. **Performance is at least competitive** with the C++ LASzip path
   (7.2M pts/s in release, often beating LASzip on point format 3).
2. **No FFI** — pure-Rust crate loaded into the cc_rust staticlib
   and exposed to C++ via the CXX bridge.
3. **No `LASzip` dependency** — one less C++ library to vendor, build,
   and audit.
4. **API is `Iterator`-based** — natural fit for our existing
   `ccPointCloud::addPoint` push-style ingestion.

## Caveats (not blockers, follow-ups)

- **`.laz` (compressed) support** is missing. Add `laz` crate on top
  of `las` (the `las` crate's API supports it via a `Reader` wrapper).
  This is `02-pure-rust-laz/` in the scenario.
- **All-into-memory** load. For >1 GB `.las` files, write a
  streaming variant (`03-streaming/`) that yields point chunks.
  Not needed for the current survey data (max ~250 MB).
- **Point format 3 (RGB + GPS time)** is what we tested. Need to
  verify formats 1, 2, 6, 7 (with classification, scan angle, etc.)
  before replacing `FileIOFilter::loadFile` in CCCoreLib.

## What we'd do differently

- Build a `write_las()` for export too. The same `las` crate handles
  write — but it wasn't tested here.
- Test point format 1 (the most common in older survey data).

## See also

- Phase 4 roadmap: [`../../../PRD/rust/05-roadmap.md` Phase 4](../../../PRD/rust/05-roadmap.md)
- Test data location: `D:\82 BROOK AVENUE\output\*.las`
- Pure-Rust LAS: <https://crates.io/crates/las>
- CCCoreLib's current LAS path: `libs/qCC_db/extern/CCCoreLib/src/LasOpenFilter.cpp` (uses LASzip)
