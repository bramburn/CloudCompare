# Promotion proposal — D9 cell-code-ordered NN (DgmOctree variant)

> Promotes `04-dgm-octree` from `benchmarked` to `selected`.
> Code is already in `cc-rust/src/dgm_octree.rs` (D7 move); this
> proposal formalises the role this variant fills in the
> recommended ICP stack.

## Candidate

`experimental/scenarios/2026-08-19-icp-variants/04-dgm-octree/`

The variant's `src/dgm_octree.rs` was promoted into `cc-rust/`
in D7 as `cc-rust/src/dgm_octree.rs`. The D9 work (cell-code-
ordered NN with AABB pruning, Chebyshev shell expansion, the
`minDistToBorder` early-termination check) was added on top in
D8/D9.

## Why — what D9 is good for

D9 is **not** the per-query speed winner — that is `02-kiddo-kdtree`
(0.30-0.53 µs/q vs D9's 0.49-0.91 µs/q at 2k-50k). D9's value
proposition is the **C++ `DgmOctree` semantic compatibility**:

- **Cell-code addressing matches CCCoreLib's `DgmOctree::computeCellCode`
  / `getCellCode` / `getCellPos` / `getBitShift` exactly.** Any
  algorithm that was written against the C++ DgmOctree can be
  ported to D9 with **no algorithm change** — only the NN call
  differs. (See `patterns.md` P16.)
- **Iterative nearest-neighbour via Chebyshev shell expansion**
  matches the C++ `DgmOctree::findNearestNeighbour` algorithm.
- **AABB-pruned with `minDistToBorder`** — same correctness
  invariant as the C++ class (pattern P18).

For users who:
- Need to port C++ code that already uses `DgmOctree::findNearestNeighbour`
- Want a pure-Rust replacement that produces the *same* nearest
  neighbour (not just a close one) as the C++ class
- Have ICP queries that share a cell with the model (the
  cell-code-hint optimisation in the C++ class is also
  available in D9)

…D9 is the right choice. For everyone else, kiddo is faster
and recommended.

## Benchmarked numbers (`experimental/scenarios/2026-08-20-icp-nn-comparison/results/latest/`)

NN query time (per query, 1000 queries, release build, commit
`a945ad1a`, see `RESULTS.md`):

| N    | 02-kiddo-kdtree (µs/q) | 04-dgm-octree (µs/q) | ratio (D9 / kiddo) |
|------|------------------------|----------------------|---------------------|
| 2k   | 0.30                   | 0.49                 | 1.6×                |
| 5k   | 0.29                   | 0.87                 | 3.0×                |
| 10k  | 0.43                   | 0.65                 | 1.5×                |
| 50k  | 0.53                   | 0.91                 | 1.7×                |

ICP wall time (end-to-end, NN-driven via the D8 trait):

| N    | 02-kiddo (s) | 04-dgm (s) |
|------|--------------|-------------|
| 2k   | 0.021        | 0.031       |
| 5k   | 0.092        | 0.188       |
| 10k  | 0.166        | 0.342       |
| 50k  | 1.029        | 2.162       |

Correctness: all four variants agree on RMS at every size
tested (differences < 1e-3, well within the 1e-3 tolerance in
`icp_cpp_matches_rust` parity tests).

## Code complexity

- ~700 LOC of Rust in `cc-rust/src/dgm_octree.rs` (D9 surface;
  full class is 3000+ lines of C++, this is the ICP-relevant
  subset — `build`, NN search, cell-code primitives).
- No external dependencies beyond `std` and `nalgebra` (for
  matrix operations in ICP, not in D9 itself).
- No unsafe code in the D9 path.

## Risk

- **2× per-query overhead vs kiddo.** Acceptable for the
  C++-compat use case; not a general-purpose NN replacement.
- **Cell-code overhead at small N.** The `HashMap` lookup +
  AABB pruning break-even around N=2k. Below that, brute-force
  is faster. (See `icp_with_nn_dispatches_to_trait` and
  `DgmOctreeNN` for the trait dispatch — the caller picks the
  NN per query size.)
- **C++-compatibility drift.** The C++ class has 3000+ lines
  we don't port. If a user needs a method we don't have
  (e.g. CC extraction, ray-casting), they have to write it
  themselves. (Follow-up item on the deferred list.)

## Known limitations

- **N < 2k: brute-force is faster than D9.** Not a bug — the
  cell-code overhead doesn't amortise until ~2k points. The
  D8 trait dispatch lets callers pick the right NN per query.
- **No full C++ class parity.** We ported the ICP-relevant
  surface only. Cell statistics, CC extraction, ray-casting,
  sphere queries are not implemented.
- **HashMap lookup overhead.** The cell → point-range map is
  a `HashMap<u64, Range<usize>>`. For very large point clouds
  with sparse cells, this is the dominant cost. A sorted
  `BTreeMap` or a single sorted-by-cell-code array would be
  faster, but the diff is small (< 10%) and the HashMap is
  simpler.

## Destination (already in place from D7)

`cc-rust/src/dgm_octree.rs` (the canonical port) +
`cc-rust/src/octree.rs` (the ICP-NN adapter, `DgmOctreeNN`).

## Files to carry forward

Already in `cc-rust/`:

- `cc-rust/src/dgm_octree.rs` — `DgmOctree`, `DgmOctreeNN`,
  `compute_cell_code`, `get_cell_pos`, `get_bit_shift`,
  `MAX_OCTREE_LEVEL`.
- `cc-rust/src/octree.rs` — `DgmOctreeNN` (the trait adapter).
- `cc-rust/src/registration.rs` — `icp_with_nn::<DgmOctreeNN>`
  end-to-end test.
- `cc-rust/tests/` (no D9-specific test file; the D9 tests are
  in `dgm_octree::tests` mod inside `dgm_octree.rs`).

## Files NOT to carry forward

None — the scenario was always a thin comparison wrapper; the
code was promoted to `cc-rust/` in D7. The 04-dgm-octree
scenario folder contains only `Cargo.toml` + `Cargo.lock` for
the local bench binary; the actual bench logic is in the
canonical `cc-rust/benches/icp_bench.rs` (promoted with the
rest of the code in D7).

## Test plan after promotion (already in place)

- 14 unit tests in `dgm_octree::tests` covering:
  - Build, code/pos round-trip, level constraints
  - NN search vs brute force at 1k / 5k / 50k Gaussian points
  - Edge cases: empty tree, single point, NaN inputs
  - Cell-code boundary cases (out-of-bbox, level-1 cell)
  - D9 production-quality: 5k Gaussian query is faster than
    brute force (correctness + perf gate)
  - `DgmOctreeNN` plugs into the D8 trait and runs end-to-end
    ICP (covers the full `build → query → transform` path)
- C++ parity: 13 CXX FFI tests in `registration.rs` (D10) that
  exercise the same ICP path against the real
  `CCCoreLib::ICPRegistrationTools::Register`. These do not
  directly test D9 vs the C++ `DgmOctree` (that would require
  a CXX FFI to the C++ class, which we don't have), but they
  do prove the ICP stack that uses D9 is parity-correct
  against CCCoreLib ICP.

## Selected statement

The D9 cell-code-ordered NN is **selected** as the recommended
NN for the niche use case where C++ `DgmOctree` semantic
compatibility matters (porting code, exact-neighbour
reproduction, cell-code hinting). For the general use case,
`02-kiddo-kdtree` (1.5-3× faster) remains the recommendation.

## Graduation (next step, deferred)

`selected → graduated` for this scenario means "the role is
documented and the code is the canonical implementation". The
code is already at `cc-rust/src/dgm_octree.rs` since D7. The
remaining graduation work is:

- [ ] Add a "When to use D9 vs kiddo" section to `cc-rust/AGENTS.md`
- [ ] Update the `recommended stack` in `experimental/docs/SUMMARY.md`
      to mention D9 as the C++-compatibility option
- [ ] Add a CHANGELOG entry in `cc-rust/` for the D9 promotion

These are docs-only and can be done in 30 minutes. They are
left as a follow-up because this proposal is the docs-only
work the user asked for in the deferred list — the rest of
the promotion narrative lives in the scenario folder.

## Approval

- [x] Lifecycle state transitioned: `benchmarked → selected`
- [x] Decision record: this file + `SUMMARY.md` "What still
      needs doing" item moved from "1. Promote D9" to "✅ done"
- [ ] Human reviewer: <name>
- [ ] Date: 2026-08-21
- [ ] Decision: pending human review
