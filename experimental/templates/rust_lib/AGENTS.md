# Template: rust_lib

## Purpose

A **pure-Rust library** crate. No C++. No FFI. No GUI.

Use this when you want to:

- Implement an algorithm and unit-test it headlessly
- Prototype a port of CCCoreLib logic
- Explore a numerical / data-structure problem
- Get a fast `cargo test` loop (no Qt, no linker)

## When NOT to use

- You need to call C++ from Rust (use `rust_cxx_app/`)
- You need a GUI (use `cpp_qt_gui/`)
- You need file I/O via Qt types (use `cpp_qt_console/`)

## Structure

```
rust_lib/
├── Cargo.toml          ← dependencies: log, env_logger, sentry (optional)
├── src/
│   └── lib.rs          ← the library
├── examples/
│   └── demo_cli.rs     ← a CLI that uses the library
├── tests/
│   └── basic.rs        ← integration tests
├── AGENTS.md           ← this file
└── README.md           ← build/run commands
```

## Build & run

```powershell
cd <session>
cargo test                       # all tests
cargo test --test basic          # integration tests only
cargo run --bin demo_cli -- 1 2 3 4 5
```

## Extension patterns

1. **Add a new module.** `touch src/foo.rs`; declare `pub mod foo;` in `src/lib.rs`.
2. **Add a CLI flag.** Use `clap` for argument parsing; see pattern in
   `experimental/sessions/2026-08-19-rust-migration-icp-scalarfield/src/main.rs`.
3. **Add a benchmark.** Add `criterion = "0.5"` to `[dev-dependencies]` and create
   `benches/foo.rs`. See `docs/patterns.md` for the full recipe.
4. **Need a GUI?** Switch to `cpp_qt_gui/` and link this crate as a `cdylib` /
   `staticlib` — see that template's AGENTS.md.

## When to upgrade

If you find yourself:

- Wanting to call C++ (especially `ccPointCloud`, `ccScalarField`) → `rust_cxx_app/`
- Wanting to visualise results → `cpp_qt_gui/`
- Wanting to ship the result to users → move to `cc-rust/` and add CMake

## What this template does NOT include

- No `unsafe` blocks (use `rust_cxx_app/` if you need them)
- No `tokio` / async (most point-cloud work is CPU-bound and synchronous)
- No `serde` (add only if your data is JSON/YAML — point clouds are binary)

## Related

- Sibling templates: `../cpp_qt_console/`, `../cpp_qt_gui/`, `../rust_cxx_app/`
- Workflow: `../../AGENTS.md`
- Example session: `../../sessions/2026-08-19-rust-migration-icp-scalarfield/`
