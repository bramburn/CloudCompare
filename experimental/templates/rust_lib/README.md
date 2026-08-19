# rust_lib template

Pure-Rust library. Replace the placeholder code with your experiment.

## Quick start

```powershell
cargo test
cargo run --bin demo_cli -- 1 2 3 4 5
```

## What you get

- `src/lib.rs` — library entry point with example `mean()` and `variance()` functions
- `examples/demo_cli.rs` — CLI that takes numbers on argv
- `tests/basic.rs` — integration test
- `log` + `env_logger` for stdout logging
- Optional `sentry` feature for crash reporting (gated — see Cargo.toml)

## Environment variables

| Var | Default | Used for |
|---|---|---|
| `RUST_LOG` | `info` | log filter (`debug`, `info`, `warn`, `error`) |
| `SENTRY_DSN` | empty | Sentry DSN; if empty and `sentry` feature is on, init is a no-op |

## Sentry

```powershell
# Build with Sentry
cargo build --features sentry
$env:SENTRY_DSN = "https://<key>@<org>.ingest.sentry.io/<project>"
cargo run --features sentry --bin demo_cli -- 1 2 3
```

## What to edit

1. `Cargo.toml` — rename `rust_lib_template` to your crate name
2. `src/lib.rs` — replace `mean()` and `variance()` with your logic
3. `examples/demo_cli.rs` — replace CLI args
4. `tests/basic.rs` — add real integration tests

## See also

- `AGENTS.md` — when to use this template
- `../../AGENTS.md` — workflow contract
