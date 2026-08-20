# Real-data ICP with the recommended stack

End-to-end test of the recommended ICP stack on the brook-avenue
`.las` scan. Compares vanilla ICP (the previous realdata-icp
session) against the new stack:

```text
1. coarse_align (PCA pre-alignment)  — matches principal axes
2. icp_multi_resolution(0.1, 1.0)   — coarse-to-fine
   with outlier_rejection_fraction=0.4
3. compare against vanilla ICP
```

## Status

- [x] scaffolded
- [x] buildable
- [ ] unit-tested (only has the integration test, no formal asserts)
- [ ] reference-validated
- [ ] benchmarked
- [ ] selected / graduated

## Run

```powershell
# Default: brook-avenue scan, 50k subsample, 0.5m translation,
# 30 ICP iterations per pass, 0.4 outlier rejection.
& 'C:\dev\CloudCompare\experimental\sessions\2026-08-20-realdata-icp-stack\target\release\realdata_icp_stack.exe' `
    -x 0.5 `
    'D:\82 BROOK AVENUE\output\2026-08-13-09-46-35_82 brook avenue.splice.las'

# Vanilla ICP only, for comparison.
... --vanilla-only ...
```

## Expected outcome

The previous realdata-icp session (vanilla ICP) recovered
`tx = -0.48` for an expected `+0.5` translation — a 4× overshoot
documented in earlier SUMMARYs. The recommended stack should
recover within `0.05` of the expected translation. If it
doesn't, the outlier-rejection fraction needs tuning for the
real-data distribution.

## Why this session exists

The new `cc-rust::coarse_align` and `icp_multi_resolution` /
trimmed-ICP features were unit-tested on synthetic fixtures.
This session is the first real-data anchor: did the new
algorithms actually move the needle on a real scan? If yes,
that's the green light to graduate them to "selected" status
in the lifecycle.

## Source

- `cc-rust/src/coarse_align.rs` (PCA pre-alignment)
- `cc-rust/src/registration.rs` (icp_multi_resolution, trimmed ICP)
- `experimental/sessions/2026-08-19-rust-realdata-icp/` (the
  vanilla-only predecessor this session supersedes)
