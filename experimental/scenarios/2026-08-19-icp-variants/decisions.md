# Decisions — ICP variants scenario

> This file supersedes the original "naive wins" entry below.
> The original numbers were measured with a buggy ICP. The
> corrected ICP is in `cc-rust/src/registration.rs` and
> re-benchmarked here against the same fixtures.

## Current state (2026-08-19, post-fix)

| Variant | Status | NN query (2k, µs/q) | NN query (10k, µs/q) | Notes |
|---|---|---|---|---|
| `01-naive-on2` | `unit-tested` | ~50 µs | ~500 µs | Brute force. RMS 0.0002 at 2k (converged); 0.06 at 10k (max-iter hit) |
| `02-kiddo-kdtree` | `unit-tested` | **0.36** | **0.39** | kiddo 6.0 KD-tree. New in this round. |
| `03-handrolled-octree` | `unit-tested` (NN) | 4.6 | 40.0 | Hand-rolled octree + AABB pruning. `nearest()` returns real index. |

## NN timings (release, Gaussian, 1000 queries against N-point tree)

| Variant | N=2k | N=10k | Notes |
|---|---|---|---|
| `01-naive-on2` | ~50 µs/q | ~500 µs/q | O(n) per query |
| `02-kiddo-kdtree` | **0.36 µs/q** | **0.39 µs/q** | O(log n), barely scales with N |
| `03-handrolled-octree` | 4.6 µs/q | 40.0 µs/q | O(log n) average with AABB pruning |

The NN query times tell the real story. For typical ICP use,
**kiddo is ~10× faster than the octree and ~100× faster than
brute force** on the Gaussian fixtures. Build times are all
sub-millisecond at these sizes.

## ICP wall times (release, Gaussian, with cc-rust brute force)

| Variant | N=2k | N=5k | N=10k |
|---|---|---|---|
| `01-naive-on2` | 0.37 s | 2.4 s | 10.0 s |
| `02-kiddo-kdtree` | 0.27 s | (skipped) | 10.1 s |
| `03-handrolled-octree` | 0.26 s | 2.45 s | 9.89 s |

The full-ICP wall times are similar across all three variants
because **cc-rust's `icp_iterate` signature does not accept a
custom NN** — each variant's wrapper builds its tree (to time
build + query) and then falls back to cc-rust's brute force for
the actual iteration. Without plugging the variant's NN into
cc-rust, the ICP wall time is dominated by brute force, not the
NN.

## Cannot pick a winner yet

The scenario status is **`benchmarked`** but **NOT** `selected`,
because:

- All three variants have working NNs.
- None of them can plug their NN into cc-rust's ICP without a
  signature change. So the "ICP wall time" column above is
  measuring cc-rust's brute force, not the variant's NN.
- A meaningful winner requires an end-to-end ICP loop using each
  NN. That's a cc-rust API change (D8 candidate — see below).

To promote this scenario to `selected`, the runner needs:

1. cc-rust to expose an `icp_with_nn(&dyn NearestNeighbour, ...)` entry point.
2. All three variants implement `NearestNeighbour` and re-bench
   at 2k / 5k / 10k / 50k on the same Gaussian fixture.
3. A `decisions.md` entry that picks a winner based on those
   numbers.

**Until that refactor, the data we have says: kiddo's NN is the
fastest, but the choice between variants doesn't matter for
end-to-end ICP wall time.** This is a D8 (cc-rust refactor)
problem, not an NN problem.

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
