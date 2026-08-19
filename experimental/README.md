# Experimental Workspace

> "Test it three different ways, keep the best, write 1 page."

This folder is the **LLM scratchpad for full builds**. It contains small, disposable
applications and libraries the agent (and you) can use to test specific behaviours
end-to-end without modifying the main CloudCompare code.

## Quick start

```powershell
# See what's here
Get-ChildItem C:\dev\CloudCompare\experimental

# Read the workflow
code C:\dev\CloudCompare\experimental\AGENTS.md

# See the catalogue
code C:\dev\CloudCompare\experimental\docs\index.md
```

## What's in here

| Folder | Purpose |
|---|---|
| `templates/` | Pre-scaffolded, minimal, buildable starters. Pick one. |
| `sessions/` | One folder per experiment. Dated + named. |
| `scenarios/` | A/B/C variant comparisons. |
| `docs/` | `index.md` (catalogue), `patterns.md` (how we do X), `decisions.md` (why we chose Y). |
| `shared/` | Cross-cutting CMake helpers, build scripts. |

## Workflow at a glance

```
user: "test X"
agent:
  1. search docs/, sessions/, scenarios/ for similar work
  2. pick a template
  3. cp -r templates/<chosen> sessions/<YYYY-MM-DD>-<topic>
  4. implement minimally
  5. build + test
  6. document (README.md, index.md, decisions.md if needed)
```

## Templates

| Template | When to use |
|---|---|
| `cpp_qt_console` | CLI test that needs Qt (file paths, Qt types) but no GUI |
| `cpp_qt_gui` | Desktop view with 3D OpenGL viewport. Links to Rust via CXX. |
| `rust_lib` | Pure-Rust library — no C++, no FFI. |
| `rust_cxx_app` | Rust + CXX FFI — calls C++ from Rust or vice versa. |

See `templates/AGENTS.md` for details.

## Current active sessions

| Date | Name | What |
|---|---|---|
| 2026-08-19 | `2026-08-19-rust-migration-icp-scalarfield` | Pure-Rust port of CCCoreLib statistics + ICP |
| 2026-08-19 | `scenarios/2026-08-19-icp-variants/` | Comparing 3 ICP approaches: naive / kiddo / hand-rolled |

(See `docs/index.md` for the full catalogue.)

## Conventions

- **Session names:** `<YYYY-MM-DD>-<short-kebab-topic>`
- **Build:** cargo (Rust) or cmake (C++/Qt)
- **Logging:** `log` (Rust), `spdlog` (C++), wired in templates
- **Sentry:** wired in templates — one DSN, one release tag per session
- **Commits:** `feat(experimental): …` with session name in the body

## Why this exists

Main CloudCompare code is the production codebase. Modifying it to test an idea
is expensive — every change has to compile through CCCoreLib, qCC_db, qCC_io,
the plugin system, and the GUI. The cost of testing "what if we did this in Rust"
in production is so high that we don't test it, we just guess.

`experimental/` makes the cost of testing low:

- Pure-Rust crates compile in seconds.
- The Qt GUI template shows a 3D viewport of your data without dragging in the
  full CloudCompare build.
- Scenarios let us try 3 approaches, see which one works, and remember why.

## What this is NOT

- **Not a plugin sandbox.** Plugins live in `plugins/`.
- **Not a fork.** This is local-only; the `cc-rust/` directory and `plugins/`
  hold real production code.
- **Not a CI build.** Experimental builds are run by hand on the local machine.
- **Not gitignored wholesale.** The structure (AGENTS.md, templates/, docs/,
  source code) is tracked. Only build outputs (`target/`, `build/`, etc.) are ignored.
  See root `.gitignore` for the exact rules.

## Related

- Root instructions: `../AGENTS.md`
- Workflow contract: `./AGENTS.md`
- Templates: `./templates/AGENTS.md`
- Knowledge base: `./docs/`
- Rust migration PRD: `../PRD/rust/`
