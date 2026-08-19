# Lifecycle — status state machine

> Defines the **only** valid transitions for a session or scenario.

## States

```
scaffolded
   ↓
buildable              (cargo build / cmake --build succeeds)
   ↓
unit-tested            (cargo test / ctest / Qt Test pass)
   ↓
reference-validated    (matches a reference impl within tolerance)
   ↓
benchmarked            (timing/memory numbers recorded with toolchain
                       + commit SHA + fixture identity)
   ↓
selected               (named winner of a scenario)
   ↓
graduated              (code moved to cc-rust/ or production)
   ↓
abandoned              (terminal — replaced by a successor or killed)
```

## What each state allows you to claim

| State | Allowed claims | Disallowed |
|---|---|---|
| scaffolded | "the project structure exists" | anything about correctness, performance, or completeness |
| buildable | "the code compiles" | "it works" / "it's fast" |
| unit-tested | "the unit tests pass" | "it matches the C++ reference" / "production-ready" |
| reference-validated | "matches reference within ε" | any performance number without benchmarked |
| benchmarked | "X ms at N points, profile release, on fixture F, commit SHA" | "always faster" / "in production" |
| selected | "we picked this for the project" | "we promoted it" (see graduated) |
| graduated | "this code is now in cc-rust/" | new claims about it — those belong in a decision record |
| abandoned | "we tried this and stopped" | resurrection without re-entering at scaffolded |

## Promotion gates

The transitions `benchmarked → selected` and `selected → graduated`
have **mandatory gates**:

### benchmarked → selected

- [ ] All correctness tests pass (`test_success = true` in `results.json`)
- [ ] At least one fixture has a measured reference (brute-force or
  CCCoreLib) for the same input
- [ ] `decisions.md` at the scenario level explains the pick with
  links to the rows in `results.json`
- [ ] No unverified "expected O(n log n)" or "should be faster"
  language in the decision record

### selected → graduated

- [ ] `promotion.md` describes what code moves to `cc-rust/` and what
  is left behind
- [ ] Human review of the promotion request
- [ ] Destination target in `cc-rust/` is updated to consume the new
  module (not a side-channel)
- [ ] The scenario's `status` is updated to `graduated`

## Status recovery

If a graduated implementation regresses (tests fail, benchmark
regresses by > 2×), the **session** in `experimental/` returns to
`benchmarked` for re-investigation. The **code in `cc-rust/`** stays
until the new winner is selected. The two-track accounting is what
makes the experiment history auditable.

## The "scope drift" failure mode

A common LLM failure: start a session as "test kiddo vs naive for
ICP", and end up adding CXX FFI, Qt visualisation, and a CLI parser
because "while we're at it". That's three different goals mixed into
one session. If you find yourself going beyond the original hypothesis:

- **Stop** and write a new session/scenario for the new goal.
- Link it from the current session's README as "related work".
- Do not promote the new goal to `selected` under the original
  scenario's name.

The runner uses the `question` field in `experiment.toml` to detect
this — if the variant's tests don't reference the original question,
that's a sign of drift.
