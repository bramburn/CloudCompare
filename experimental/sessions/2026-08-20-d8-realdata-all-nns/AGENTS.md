# D8 end-to-end on real data (2026-08-20)

> Run `icp_with_nn` with each of the 3 NNs (naive, kiddo,
> octree) on a subsample of the brook-avenue `.las` scan.
> Same data, same algorithm, different NN — compare wall time
> and correctness.

## What this session is

The D8 refactor (`NearestNeighbour` trait + `icp_with_nn`) is
already unit-tested in `cc-rust/src/registration.rs` and in
each of the 3 variant crates under
`scenarios/2026-08-19-icp-variants/`. This session is the
**end-to-end real-data test**:

1. Read the brook-avenue `.las` (7.5M points) via the
   pure-Rust `las` crate.
2. Subsample by a configurable stride (default 150 → ~50k pts).
3. Apply a known translation (default 0.5m on X).
4. For each of the 3 NNs:
   - Call the variant's `icp_iterate` (which goes through
     `icp_with_nn` internally with the variant's NN).
   - Capture wall time, iterations, RMS, recovered translation.
5. Verify all 3 NNs produce the same RMS (trait dispatch is
   correct) and recover the translation within 0.05m.
6. Write `results.json` with the full comparison.

## Why this is "fully test it out"

The 3 NNs produce the same RMS only if the D8 trait dispatch
is real — i.e. `icp_with_nn` actually calls each variant's
NN, not a hard-coded brute force. On real data, the
correspondence problem is much harder than on synthetic
Gaussian: the scan has noise, partial overlap, and a real
building. If the trait dispatch were broken, the kiddo
variant would either crash (tree built from full-resolution
data is huge) or diverge (correspondences wrong). It doesn't,
because the dispatch is real.

## How to run

```powershell
$env:Path = "C:\dev\tools\cmake-4.3.0\bin;C:\Users\bramburn\.cargo\bin;$env:Path"
Push-Location C:\dev\CloudCompare\experimental\sessions\2026-08-20-d8-realdata-all-nns
cargo build --release
& '.\target\release\d8_realdata_all_nns.exe' `
    -x 0.5 `
    'D:\82 BROOK AVENUE\output\2026-08-13-09-46-35_82 brook avenue.splice.las'
Pop-Location
```


## Gotcha: clone the data per variant (P17)

**The first run of this session had a subtle bug.** The session
calls each variant's icp_iterate in sequence. icp_iterate
mutates the data array in place — it applies the recovered
transform to the data before returning. So when the second
variant ran, the data had already been moved to the model by
the first variant, and the second variant saw data == model.

**Symptom:** every variant reported converged=true after only
2 iterations with RMS = 0.000001 (the "perfect" alignment of
data to model). The recovered translations were wildly
different: naive recovered -0.5 (correct), kiddo and octree
recovered 0 (wrong, because the trivial "data == model" is
the identity transform).

**Why the RMS check didn't catch it:** the test asserts "all
3 NNs produce the same RMS" — which is true, they all
produce 0.000001. But the assertion doesn't check that the
*recovered translation* is meaningful. A data == model
trivial alignment is a 0.000001 RMS that says nothing.

**Fix:** clone the shifted-data template before each variant
call (see src/main.rs, lines 70-90). This is captured as
**pattern P17** in ../../docs/patterns.md — the lesson is
generic: any time you benchmark N variants of an in-place
algorithm, snapshot the input per variant.
## Expected outcome

- All 3 NNs produce the same RMS (within 1e-3) — trait
  dispatch is correct.
- All 3 NNs recover the 0.5m translation within 0.05m.
- kiddo is the fastest of the 3 (the headline D8 finding
  from the `2026-08-20-icp-nn-comparison` scenario).

## Build dependencies

This session depends on 3 sibling variant crates (which in
turn depend on `cc-rust` and `kiddo`):

- `cc-rust` — the trait, `icp_with_nn`, `BruteForceNN`.
- `icp_v1_naive` — `BruteForceNN` adapter (alias).
- `icp_v2_kiddo` — `KiddoNN` adapter (f32→f64 wrap).
- `icp_v3_octree` — `OctreeNN` adapter.
- `las_v1_pure_rust` — the pure-Rust `.las` reader.

## See also

- `../../scenarios/2026-08-20-icp-nn-comparison/` — the
  cross-variant end-to-end bench (D8 deliverable on synthetic
  Gaussian).
- `../../docs/decisions.md` D8 — the trait + dispatch decision.
- `../../docs/patterns.md` P16 — the trait + adapter pattern.
- `../2026-08-20-realdata-icp-stack/` — the recommended-stack
  (coarse_align + multi-res + trimmed) on the same `.las`.
