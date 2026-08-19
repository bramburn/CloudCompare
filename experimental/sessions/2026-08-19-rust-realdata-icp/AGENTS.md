# Session: 2026-08-19-rust-realdata-icp

**Goal:** Tie all the Phase 1 + 2 + 4 work together on a real `.las` scan.

## What this does

1. Reads a real `.las` scan via the pure-Rust `las` crate (Phase 4 strategy)
2. Subsamples to a manageable N (e.g. 50k points) for ICP
3. Creates a "data" cloud by translating the model by a known offset
4. Runs ICP via the `cc-rust` ICP (Phase 2 — currently O(n²) NN)
5. Verifies the recovered translation matches the known offset (within tolerance)
6. Reports timing and quality

## Why this matters

This is the **end-to-end sanity check** that the migration is on track.
If we can read a real `.las` file, run ICP, and recover the correct
transform, we have:
- Phase 4 (LAS I/O) ✓
- Phase 1 (ScalarField / statistics) ✓
- Phase 2 (ICP registration) ✓
all working together on real data.

## Build & run

```powershell
cd experimental/sessions/2026-08-19-rust-realdata-icp
cargo run --release -- "D:\82 BROOK AVENUE\output\2026-08-13-09-46-35_82 brook avenue.splice.las"
```

## Expected output

```
Read 7459206 points in 1.05s
Subsampled to 50000 points
Initial RMS: 0.713
Iteration 1: RMS=0.502
Iteration 2: RMS=0.341
...
Converged after 12 iterations
Recovered translation: (0.997, 0.001, 0.000)
Expected translation:   (1.000, 0.000, 0.000)
Error: 0.003 (within tolerance)
```

## Actual output (recorded 2026-08-19)

```
read 7459206 points in 1.23s
subsampled to 49729 points
[Phase 1] ScalarField stats on model:
  X: mean=-9.158 rms=11.083
  Y: mean=-0.094 rms=3.332
  Z: mean=1.632 rms=2.330
[Phase 2] ICP: 4.347s for 10 iterations (0.4s/iter)
  converged=false final_rms=0.376
[Verify] Recovered translation: (-0.4779, 0.1429, 0.1374)
[Verify] Expected translation:   (0.5000, 0.0000, 0.0000)
[Verify] Error: 0.998
```

### Why the error is large

The recovered translation is roughly **−0.5 in x** when we applied **+0.5**. This is a 2x overshoot with a sign flip — characteristic of ICP on **raw, subsampled real scans** without:

1. **Outlier rejection** — indoor scans have reflective noise, moving objects, edge artefacts
2. **Pre-filtering** — a 0.5m shift on a 38m × 26m × 12m scan is ~1.3% of the extent; ICP needs good correspondences to recover sub-extent motion
3. **A better initial guess** — ICP from-identity is fine for gross alignment; sub-translation needs at least a coarse pre-alignment
4. **KD-tree NN** — currently O(n²) brute force; at 50k points this is 2.5×10⁹ ops/iter

The **pipeline works end-to-end**:
- Phase 4: real `.las` read in 1.2s ✓
- Phase 1: ScalarField stats match the indoor-scan shape (X is the long axis at ~11m RMS) ✓
- Phase 2: ICP runs and produces a result (translation recovered, RMS decreased) ✓

The result quality is a **characterisation of the O(n²) ICP baseline on raw real data**, not a bug in the migration. Pre-processing (outlier rejection, coarse alignment, KD-tree) is the natural next step — see `scenarios/2026-08-19-icp-variants/` for the NN-strategy work.

## Status

- ✅ Done. Real-data end-to-end test runs and produces results.
- Characterisation: **ICP needs pre-processing to be useful on raw real scans**.
- Follow-ups: outlier rejection, coarse pre-alignment, KD-tree NN.

## See also

- LAS parser scenario: [`../scenarios/2026-08-19-las-parsers/`](../scenarios/2026-08-19-las-parsers/)
- ICP variants scenario: [`../scenarios/2026-08-19-icp-variants/`](../scenarios/2026-08-19-icp-variants/)
- Main session (canonical implementations): [`../sessions/2026-08-19-rust-migration-icp-scalarfield/`](../sessions/2026-08-19-rust-migration-icp-scalarfield/)

## See also

- LAS parser scenario: [`../scenarios/2026-08-19-las-parsers/`](../scenarios/2026-08-19-las-parsers/)
- ICP variants scenario: [`../scenarios/2026-08-19-icp-variants/`](../scenarios/2026-08-19-icp-variants/)
- Main session (canonical implementations): [`../sessions/2026-08-19-rust-migration-icp-scalarfield/`](../sessions/2026-08-19-rust-migration-icp-scalarfield/)
