# Scenario Template — start here for A/B/C comparisons

> A "scenario" is a multi-variant comparison. You have 2–3 candidate
> implementations of the same algorithm and want to pick a winner based on
> correctness + speed + memory.

## How to scaffold a new scenario

```powershell
cp -r experimental/templates/scenario experimental/scenarios/<YYYY-MM-DD>-<topic>
# Edit the files below. Then:
mkdir experimental/scenarios/<YYYY-MM-DD>-<topic>/01-naive
cp -r experimental/templates/rust_lib experimental/scenarios/<YYYY-MM-DD>-<topic>/01-naive
mkdir experimental/scenarios/<YYYY-MM-DD>-<topic>/02-kdtree
cp -r experimental/templates/rust_lib experimental/scenarios/<YYYY-MM-DD>-<topic>/02-kdtree
# ... as many variants as you need
```

## Files in this template

- `AGENTS.md` — what we're comparing, scope, what we are NOT comparing
- `experiment.toml` — the **machine-readable contract** for the scenario
- `README.md` — human-readable overview
- `decisions.md` — winner + rationale (filled in at the end)
- `RESULTS.md` — actual measurements (filled in after running all variants)
- `results.json` — same data in JSON for downstream tooling

## `experiment.toml` schema

```toml
# The unique ID for this scenario (folder name without scenarios/).
id = "2026-08-19-icp-variants"

# The hypothesis we're testing. Phrase as a falsifiable statement.
question = "Which nearest-neighbour structure should support Rust ICP?"

# Lifecycle state (see experimental/AGENTS.md).
# One of: scaffolded | buildable | unit-tested | reference-validated |
#         benchmarked | selected | graduated | abandoned
status = "benchmarked"

# Owning area (for cross-referencing in root AGENTS.md / PRD).
owner = "rust-migration"

# Reference to the C++ algorithm we're trying to match (or None if
# pure-Rust exploration).
reference = { cpp = "CCCoreLib/src/RegistrationTools.cpp" }

# Inputs that are valid for this scenario. Each entry is a fixture
# declared in experimental/fixtures/. The manifest path is relative.
# The runner will look these up and refuse to benchmark against
# unknown fixtures.
fixtures = [
    "synthetic/cube-8.toml",
    "synthetic/gaussian-2k.toml",
    "real/brook-avenue-splice.toml",   # optional: missing → skipped
]

# Variants: the implementations we're comparing. Each variant must
# be a sibling session folder with its own experiment.toml.
variants = [
    "01-naive-on2",
    "02-kiddo-kdtree",
    "03-handrolled-octree",
]

# Metrics that matter. The runner collects these for each variant
# on each fixture and writes them to results.json.
metrics = [
    "build_success",         # bool — did cargo build exit 0?
    "test_success",          # bool — did cargo test pass?
    "correctness_rmse",      # float — RMS distance to brute-force reference
    "runtime_ms",            # float — wall time on the chosen fixture
    "peak_memory_mb",        # float — peak RSS
    "point_count",           # int   — number of input points
    "failure_mode",          # string — "ok" | "diverged" | "crashed" | "timeout"
]

# Promotion rules: what the winner must demonstrate before it can
# be marked "selected". This is the gate.
promotion = { required = [
    "all_correctness_tests_pass",
    "benchmark_results_recorded_in_results_json",
    "decisions_md_explains_why",
] }

# Tags for cross-searching. Don't be stingy.
tags = ["icp", "nearest-neighbour", "rust", "phase-2"]
```

## Per-variant `experiment.toml`

Each variant folder has its own `experiment.toml` declaring:

- The variant's ID (its folder name)
- Its current `status` (independent of the scenario)
- The fixtures it actually ran against
- Any variant-specific notes (e.g. "uses kiddo 6.0 API")

This is what allows the runner to skip a not-yet-built variant without
marking the whole scenario as failed.

See `01-naive-on2/experiment.toml` for an example.

## The runner

Use the single command:

```powershell
& 'C:\dev\CloudCompare\experimental\run.ps1' `
    -Scenario 'C:\dev\CloudCompare\experimental\scenarios\2026-08-19-icp-variants' `
    -Profile release
```

It will:

1. Validate the `experiment.toml` schema.
2. For each variant, in order:
   - `cargo build --release` (or whatever the variant's build is)
   - `cargo test` (or the variant's test command)
   - Run the benchmark binary against each fixture
   - Capture wall time, peak memory, RMS vs brute-force reference
3. Write `results.json` (machine-readable) and `RESULTS.md` (human-readable).
4. Print a summary table.

It does NOT make a "selected" decision. That's a human review step.

## What "selected" means in the manifest

Once a winner is picked:

1. Update the scenario's `experiment.toml` to `status = "selected"`.
2. Update each non-winning variant to `status = "abandoned"` with a reason.
3. Move/keep the winner's code, but **also** write a `promotion.md` in
   the scenario folder (or in `cc-rust/docs/`) describing what to
   carry into production and what to leave behind.
4. The agent should not silently copy code — it should write the
   promotion proposal and wait for explicit approval.
