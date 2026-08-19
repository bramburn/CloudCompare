# Promotion — moving an experiment to production

> The only path from `experimental/` to `cc-rust/` (or a real plugin).

## Why a written proposal

The agent should not silently copy experiment code into the main
codebase. There are three reasons:

1. **Experiments are not production-ready.** They may have skipped
   edge cases, hard-coded test data, or relaxed error handling.
2. **Naming and module boundaries differ.** The experiment is
   optimised for fast iteration; `cc-rust/` is optimised for reuse.
3. **Auditable history.** A human must be able to see *what* moved
   and *why* — even months later.

## Promotion request template

Save this as `promotion.md` in the scenario folder:

```md
# Promotion proposal — <topic>

## Candidate

`scenarios/<date>-<topic>/<winner-variant>/`

## Why

- Correctness: <reference-validated, links to results.json>
- Speed: <benchmarked, links to results.json>
- Memory: <if measured>
- Code complexity: <LOC, key dependencies>
- Risk: <what could go wrong in production>

## Known limitations

- <unverified edge cases>
- <unmeasured performance regimes>
- <test coverage gaps>

## Destination

`<destination path in cc-rust/ or plugins/>`

## Files to carry forward

- src/<file>                (one bullet per file)
- tests/<file>
- benches/<file>

## Files NOT to carry forward

- <fixture generators>
- <experimental logging>
- <placeholder TODOs>
- <CLI-only code that has no place in the library API>

## Test plan after promotion

- <how do we know the moved code still works in cc-rust/>
- <new tests needed at the integration boundary>

## Approval

- [ ] Human reviewer: <name>
- [ ] Date: <date>
- [ ] Decision: <approved | approved-with-changes | rejected>
```

## The agent's job during promotion

1. Write the proposal above. Do not start copying code.
2. Wait for explicit approval.
3. Once approved, copy the agreed files into `cc-rust/` (or plugin).
4. Add the necessary `mod` declaration in `cc-rust/src/lib.rs` (or the
   plugin's `CMakeLists.txt`).
5. Add the integration test to `cc-rust/tests/` (or the plugin's tests).
6. Run `cargo test --all-targets --all-features` to confirm the main
   crate still builds.
7. Update the scenario's `experiment.toml` to `status = "graduated"`.
8. Update `docs/index.md` to link the graduated scenario.

## What "graduated" does NOT mean

- The scenario folder is not deleted. It stays as the historical
  record of the A/B/C decision.
- The variant's tests are not moved. `cc-rust/` has its own tests
  that match its module structure.
- The "selected" status is not undone. The agent should be able to
  grep for the candidate years later and see why it won.

## When NOT to promote

- The "winner" was selected by intuition, not measurement.
- The reference validation was against a degenerate fixture.
- The benchmarked numbers are from a different machine or toolchain.
- The code is still in `scaffolded` or `buildable` state.
- A regression was found after the original `selected` decision and
  the recovery hasn't finished.

The agent should refuse promotion in any of these cases and surface
the blocker to the human reviewer.
