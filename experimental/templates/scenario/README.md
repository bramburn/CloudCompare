# Scenario: <title>

> One-paragraph summary of what we're comparing and why.

## Status

- [ ] scaffolded
- [ ] buildable
- [ ] unit-tested
- [ ] reference-validated
- [ ] benchmarked
- [ ] selected
- [ ] graduated / abandoned

The current `status` in `experiment.toml` is the source of truth for
the agent. The checkbox list here is for human readers.

## Variants

| # | Folder | Approach | Status |
|---|---|---|---|
| 01 | `01-naive/` | Brute-force O(n²) | (filled in as variants land) |
| 02 | `02-kdtree/` | KD-tree | |
| 03 | `03-octree/` | Hand-rolled octree | |

## Build

```powershell
& 'C:\dev\CloudCompare\experimental\run.ps1' `
    -Scenario 'C:\dev\CloudCompare\experimental\scenarios\<this-folder>' `
    -Profile release
```

## What we are NOT comparing

- (algorithm variants — that's a different scenario)
- (convergence criteria — another scenario)
- (rotation estimation methods — another scenario)

## Related

- Top-level decisions: [`../../docs/decisions.md`](../../docs/decisions.md)
- Patterns: [`../../docs/patterns.md`](../../docs/patterns.md)
- Index: [`../../docs/index.md`](../../docs/index.md)
