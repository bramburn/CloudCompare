# Templates

> "Start from a working thing, not a blank folder."

Each template is a **pre-scaffolded, minimal, buildable** starter. Copy it
(`cp -r templates/<name> sessions/<date>-<topic>`) and modify. Don't invent your
own project structure from scratch — that's how you end up with five different
Cargo.toml conventions and a build that only works on your machine.

## Available templates

| Template | Stack | When to use | Build time |
|---|---|---|---|
| [`cpp_qt_console/`](cpp_qt_console/) | C++ + Qt 6 + CMake | CLI test that needs Qt types (paths, strings) but no GUI. Pure logic + I/O. | ~30 s |
| [`cpp_qt_gui/`](cpp_qt_gui/) | C++ + Qt 6 + OpenGL + CXX → Rust | Desktop view: 3D viewport, real-time visualisation, links to Rust crate. | ~60 s |
| [`rust_lib/`](rust_lib/) | Rust + Cargo | Pure-Rust library — no C++, no FFI, headless tests. | ~10 s |
| [`rust_cxx_app/`](rust_cxx_app/) | Rust + CXX + Cargo + C++ | Rust calls C++ or vice versa. Tests FFI bindings. | ~20 s |
| [`scenario/`](scenario/) | A/B/C variant comparison | Multiple variants of the same algorithm. Includes `experiment.toml` schema. | n/a (folder template) |

## How to pick

1. **Pure Rust, no GUI?** → `rust_lib/`
2. **Need to call C++ from Rust (or vice versa)?** → `rust_cxx_app/`
3. **Need a CLI that uses Qt types?** → `cpp_qt_console/`
4. **Need a 3D viewport, file dialog, or visualisation?** → `cpp_qt_gui/`
5. **Comparing 2–3 implementations of the same algorithm?** →
   [`scenario/`](scenario/) (folder template; copy the whole folder
   to `scenarios/<date>-<topic>/`, then add per-variant subfolders
   with `experiment.toml`).

If you need a combination (e.g. Qt GUI that links to Rust crate), copy
`cpp_qt_gui/` and edit the `CMakeLists.txt` to add `cargo build` as a custom
target — see that template's `AGENTS.md` for the recipe.

## Each template has

- `AGENTS.md` — what it is, when to use, extension patterns
- `README.md` — build/run commands
- The minimum code to build and run
- Wired logging (spdlog for C++, `log` for Rust)
- Wired Sentry **disabled by default** — set `SENTRY_DSN` in the
  environment to enable. (Per the evidence rules in `../AGENTS.md`.)

## Sentry policy

The Sentry SDK is wired into the templates but **disabled by default**.
A template MUST NOT send data to Sentry without an explicit environment
variable:

```powershell
$env:SENTRY_DSN = "https://...@...ingest.sentry.io/..."   # enables
$env:SENTRY_ENVIRONMENT = "experimental"                  # default
$env:SENTRY_RELEASE = "<session-folder-name>"             # required
cargo run --release
```

If `SENTRY_DSN` is empty or unset, `sentry_init` is skipped and the
program runs with console + file logging only. This is the recommended
mode for all local experiments.

## Adding a new template

If you find yourself creating a similar scaffold for the third time:

1. **Promote it to a template.** Move it under `templates/`.
2. **Write the AGENTS.md** for the template: what, when, structure, extension patterns.
3. **Cross-link from this file.**
4. **Document the pattern in `../docs/patterns.md`** — explain why the template
   looks the way it does.

Templates should stay **small and obvious**. If your template grows past 200 lines
of source, it's probably not a template anymore — it's a session.

## What templates MUST do

- [ ] Build on this machine (Windows Server 2019, MSVC 14.44, Qt 6.8.3, Rust 1.89)
- [ ] Pass a no-op test ("hello world" passes)
- [ ] Have build/run commands that take < 60 seconds for "hello world"
- [ ] Wire `log` (Rust) or `spdlog` (C++) in their main
- [ ] Wire Sentry **gated behind `SENTRY_DSN` env var** (no hard-coded DSN)
- [ ] Have an `AGENTS.md` and a `README.md`

## What templates MUST NOT do

- [ ] Pull in the full CloudCompare build (no `ccPluginInterface`, no `ccPointCloud` types)
- [ ] Use vendored forks of Qt or other libs — always use the system Qt 6.8.3
- [ ] Commit `target/`, `build/`, or `Cargo.lock`
- [ ] Be a "blank" template (must have hello-world code that builds and runs)
- [ ] Hard-code a Sentry DSN — must be environment-driven

## Related

- Parent: `../AGENTS.md`
- How to copy a template into a session: `../AGENTS.md` § "Working rules"
- Status state machine: `../docs/lifecycle.md`
- Why templates exist: see `../docs/patterns.md` (P8)
