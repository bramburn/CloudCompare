# Fixtures — real-data and synthetic reference data

> **Real datasets are referenced by manifest + hash, never committed.**
> The fixture folder contains TOML manifests that point to where
> the actual data lives on the local machine (or a network share).

## Format

Each fixture is a TOML file with:

- `id` — unique name (lowercase, kebab-case)
- `kind` — `synthetic` or `real`
- `path` — absolute or `~`-relative path to the data file
- `sha256` — hex hash of the file
- `size_bytes` — file size in bytes
- `format` — `las` / `laz` / `ply` / `bin` / `csv` / `json` / ...
- `expected` — what the fixture should produce when parsed correctly
  (e.g. point count, schema fields)
- `provenance` — where the file came from (free-form)
- `local_only` — `true` if the file is proprietary or too large for git

## Example: synthetic

```toml
id = "asymmetric-9"
kind = "synthetic"
generator = "cc-rust/tests/fixtures/asymmetric_9"   # where the code lives
format = "json"
point_count = 9
expected_centroid = [0.611, 0.478, 0.522]
provenance = "test fixture for ICP; non-symmetric on purpose"
```

## Example: real (locally-stored scan)

```toml
id = "brook-avenue-splice"
kind = "real"
path = "D:/82 BROOK AVENUE/output/2026-08-13-09-46-35_82 brook avenue.splice.las"
sha256 = "REPLACE_ME"
size_bytes = 253_000_000
format = "las"
expected = { point_count = 7_459_206, version = "1.4", pdrf = 3 }
provenance = "Leica RTC360 scan, captured 2026-08-13"
local_only = true
notes = "Do not commit. Path includes a space — quote it in shell."
```

## How sessions reference fixtures

A scenario's `experiment.toml` lists fixture manifests:

```toml
fixtures = [
    "synthetic/asymmetric-9.toml",
    "synthetic/cube-8.toml",
    "real/brook-avenue-splice.toml",   # missing → skipped, not failed
]
```

The `run.ps1` script resolves each manifest, hashes the file at run
time, and refuses to run if the SHA-256 in the manifest doesn't match
the file on disk (catches silent corruption / wrong file).

## Synthetic fixture generators

When a "synthetic" fixture is a generated point cloud, the generator
itself is small enough to commit. Store it under
`experimental/fixtures/synthetic/_generators/<id>.rs` or similar. The
manifest points at it.

## Adding a new fixture

1. Place the file outside the repo (e.g. `D:/scans/...`).
2. Compute its SHA-256: `Get-FileHash -Algorithm SHA256 <path>`.
3. Create the manifest at `experimental/fixtures/<kind>/<id>.toml`.
4. Reference it from your scenario's `experiment.toml`.
5. Run `run.ps1` and confirm the manifest is resolved.

## Real datasets the project has used

- `real/brook-avenue-splice.toml` — 253 MB `.las`, 7.5M points,
  Leica RTC360 indoor scan, captured 2026-08-13.
- `real/brook-avenue-optimized1.toml` — 166 MB `.las`, follow-up scan.
- `real/...` (more to come as the migration progresses)

The `.rcp` and `.rcs` files in the same directory are proprietary
Autodesk ReCap SDK formats requiring a licensed SDK not present in
this repo. They are referenced here for context only; use the
converted `.las` files for experiments.
