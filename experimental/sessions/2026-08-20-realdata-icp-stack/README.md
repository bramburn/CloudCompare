# Real-data ICP with the recommended stack

End-to-end real-data test of the new ICP features
(`coarse_align`, `icp_multi_resolution`, trimmed ICP) on the
brook-avenue `.las` scan. See `AGENTS.md` for the experiment
contract and the expected outcome.

## Build

```powershell
$env:Path = "C:\dev\tools\cmake-4.3.0\bin;C:\Users\bramburn\.cargo\bin;$env:Path"
Push-Location C:\dev\CloudCompare\experimental\sessions\2026-08-20-realdata-icp-stack
cargo build --release
Pop-Location
```

## Run

```powershell
& 'C:\dev\CloudCompare\experimental\sessions\2026-08-20-realdata-icp-stack\target\release\realdata_icp_stack.exe' `
    -x 0.5 `
    'D:\82 BROOK AVENUE\output\2026-08-13-09-46-35_82 brook avenue.splice.las'
```
