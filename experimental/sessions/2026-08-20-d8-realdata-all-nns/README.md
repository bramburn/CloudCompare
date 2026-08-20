# D8 end-to-end on real data

End-to-end test of the `NearestNeighbour` trait + `icp_with_nn`
on the brook-avenue `.las` scan. Same data, same algorithm,
different NN — compare wall time and correctness.

## Run

```powershell
Push-Location C:\dev\CloudCompare\experimental\sessions\2026-08-20-d8-realdata-all-nns
cargo build --release
& '.\target\release\d8_realdata_all_nns.exe' `
    -x 0.5 `
    'D:\82 BROOK AVENUE\output\2026-08-13-09-46-35_82 brook avenue.splice.las'
Pop-Location
```

The output includes a `results.json` file with the per-variant
wall time, RMS, and recovered translation, plus a pass/fail
verdict on correctness and translation recovery.

## What it proves

- The D8 trait dispatch is real (not just a signature change).
- Each variant's NN produces the same correspondences (RMS
  matches within 1e-3) on real survey data.
- Each variant's NN recovers the known translation.
- The kiddo NN is the fastest, confirming the
  `2026-08-20-icp-nn-comparison` finding on real data.

## See also

- `AGENTS.md` for the full experiment contract.
- `../../scenarios/2026-08-20-icp-nn-comparison/` for the
  synthetic-Gaussian end-to-end bench.
- `../../docs/decisions.md` D8 and `../../docs/patterns.md`
  P16 for the architectural context.
