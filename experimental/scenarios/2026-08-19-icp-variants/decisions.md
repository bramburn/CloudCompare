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
| `03-handrolled-octree` | `unit-tested` (NN) + `buildable` (integration) | see below | `nearest()` now returns real index (regression test added); ICP wrapper still uses cc-rust brute force |

## Octree NN timings (2026-08-19)

`03-handrolled-octree::Octree::from_points` builds an octree and
exposes `nearest(query) -> (index, dist_sq)`. Timings (release,
Gaussian cloud, 1000 queries against a tree built from N points):

| N points | build (s) | 1000 queries (s) | per-query (µs) |
|---|---|---|---|
| 2 000 | 0.000 | 0.005 | 4.6 |
| 5 000 | 0.001 | 0.013 | 13.2 |
| 10 000 | 0.003 | 0.040 | 40.0 |

The build is essentially free for these sizes. The per-query
time is sub-O(n) thanks to AABB pruning (subtrees whose closest
possible distance exceeds the current best are skipped).

The full ICP iteration (using cc-rust's brute-force NN internally)
takes 0.26 s / 2.45 s / 9.89 s at 2k / 5k / 10k. The ICP loop is
**dominated by NN** but the wrapper currently builds the octree
without using it for ICP — see "Cannot pick a winner yet" below.

## Cannot pick a winner yet

The scenario status is **`benchmarked`** but **NOT** `selected`,
because:

- `02-kiddo-kdtree` is still a skeleton (no KD-tree plugged in).
- `03-handrolled-octree` has a real `nearest()` now, but the ICP
  wrapper does not use it for the actual iteration — cc-rust's
  ICP signature does not yet accept a custom NN. The wrapper
  builds the octree (to time build + query) but falls back to
  cc-rust's brute force for the ICP loop. Without plugging the
  octree into cc-rust, the ICP wall times are the same as
  `01-naive-on2` (~0.37 s / 2.4 s / 10.0 s).
- A meaningful "octree wins" or "naive wins" claim requires an
  end-to-end ICP loop using each NN. That's a cc-rust API change
  (D8 candidate — see below).

To promote this scenario to `selected`, the runner needs:

1. `02-kiddo-kdtree` to be `reference-validated` (KD-tree plugged in,
   tests pass on `asymmetric-9`).
2. `03-handrolled-octree` to be `reference-validated` via the new
   `cc_rust::icp_with_nn(...)` entry point (or equivalent).
3. All three variants benchmarked on the same Gaussian fixture
   at 2k / 5k / 10k / 50k with the **same** ICP loop body, only
   the NN data structure differing.
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

1. **Refactor cc-rust's ICP to accept an NN trait** (D8 candidate).
   The cleanest signature:
   ```rust
   pub trait NearestNeighbour {
       fn nearest(&self, query: &[f32; 3]) -> (usize, f32);
   }
   pub fn icp_with_nn<N: NearestNeighbour>(
       data: &mut [f32],
       model_nn: &N,
       params: &IcprParamsRust,
   ) -> Result<IcprResultRust, IcprErrorRust>;
   ```
   Then both `01-naive-on2` and `03-handrolled-octree` can
   implement `NearestNeighbour` and ICP runs once per NN.
2. **Implement `02-kiddo-kdtree`** with the kiddo 6.0 API and the
   same trait.
3. **Re-bench all three** with the corrected ICP + the new trait
   abstraction, on Gaussian at 2k / 5k / 10k / 50k.
4. **Update this file** with the new numbers and a `selected`
   pick.
5. **Update `../../docs/decisions.md` D4** with the actual
   winner or "all three are within X% of each other" finding.

## Source

- `cc-rust/src/registration.rs` (corrected ICP)
- `scenarios/2026-08-19-icp-variants/01-naive-on2/` (re-benched 2026-08-19 23:00 UTC)
- `scenarios/2026-08-19-icp-variants/03-handrolled-octree/` (real `nearest()` 2026-08-19 23:30 UTC)
- `experimental/docs/decisions.md` D4 (history)
- `experimental/docs/patterns.md` P12, P13, P14 (the three fixes)
- `experimental/docs/promotion.md` (process for `cc-rust` changes)
