# Decisions — ICP variants scenario

> This file supersedes the original "naive wins" entry below.
> The original numbers were measured with a buggy ICP. The
> corrected ICP is in `cc-rust/src/registration.rs` and
> re-benchmarked here against the same fixtures.

## Current state (2026-08-19, post-fix)

| Variant | Status | Last benchmark (corrected ICP) | Notes |
|---|---|---|---|
| `01-naive-on2` | `unit-tested` | 0.37 s / 2.4 s / 10.0 s @ 2k/5k/10k | RMS 0.0002 at 2k (converged); 0.06 at 5k/10k (max-iter hit, not diverged) |
| `02-kiddo-kdtree` | `buildable` (skeleton) | n/a | kiddo 6.0 API migration deferred — bounded work, see `02-kiddo-kdtree/experiment.toml` |
| `03-handrolled-octree` | `buildable` (skeleton) | n/a | `nearest()` still returns 0 as placeholder; needs real point indices and an early-out |

## Cannot pick a winner yet

The scenario status is **`benchmarked`** but **NOT** `selected`,
because:

- `02-kiddo-kdtree` is a skeleton (no KD-tree plugged in).
- `03-handrolled-octree` is a skeleton (`nearest()` returns 0).
- The previous "naive wins" claim was based on a benchmark of the
  octree variant whose `nearest()` was returning 0 — i.e. the
  octree was finding wrong correspondences, ICP was diverging
  silently, and the wall time was a meaningless comparison.

To promote this scenario to `selected`, the runner needs:

1. `02-kiddo-kdtree` to be `reference-validated` (KD-tree plugged in,
   tests pass on `asymmetric-9`).
2. `03-handrolled-octree` to be `reference-validated` (real
   `nearest()` returning point indices, with an early-out).
3. All three variants benchmarked on the same Gaussian fixture
   at 2k / 5k / 10k / 50k.
4. A `decisions.md` entry that picks a winner based on those
   numbers.

## Original (SUPERSEDED) decision

> Retained for the historical record. Do not use these numbers
> to make a decision.

The original claim was:

- **Naive wins for now.** On random Gaussian point clouds of
  2k–10k points, the naive implementation was **2–3× faster**
  than the hand-rolled octree (measured 2026-08-19 18:00 UTC:
  0.36s vs 0.60s on 2k, 2.4s vs 4.1s on 5k, 9.2s vs 23.3s on 10k).
- **Reason:** the hand-rolled octree was naive (no early-out,
  no per-leaf bounds, all 8 children recursed).

That was true, but both variants were running the buggy ICP, so
the *correctness* of the comparison was not load-bearing for any
ICP feature work. Re-bench with the corrected algorithm before
reinstating or reversing the claim.

## Next concrete steps

1. **Fix `03-handrolled-octree::Octree::nearest()`** to return the
   real point index (not 0). Add an early-out for subtrees whose
   bounding-box distance exceeds the current best. Tests on
   `asymmetric-9`.
2. **Implement `02-kiddo-kdtree`** with the kiddo 6.0 API. Tests
   on `asymmetric-9`.
3. **Re-bench all three** with the corrected ICP, on the same
   Gaussian fixture at 2k / 5k / 10k / 50k.
4. **Update this file** with the new numbers and a `selected`
   pick.
5. **Update `../../docs/decisions.md` D4** to either reinstate or
   reverse the "naive wins" claim based on the new numbers.

## Source

- `cc-rust/src/registration.rs` (corrected ICP)
- `scenarios/2026-08-19-icp-variants/01-naive-on2/` (re-benched 2026-08-19 23:00 UTC)
- `experimental/docs/decisions.md` D4 (history)
- `experimental/docs/patterns.md` P12, P13, P14 (the three fixes)
