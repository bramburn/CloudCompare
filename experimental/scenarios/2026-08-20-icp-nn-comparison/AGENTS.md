# ICP NN comparison scenario (D8, 2026-08-20)

> Cross-variant comparison of the three ICP NN structures after
> the `NearestNeighbour` trait refactor. This is the D8 deliverable
> for the Rust migration roadmap.

## What this scenario is

The three sibling scenarios under [`2026-08-19-icp-variants/`](../2026-08-19-icp-variants/)
each implement one NN structure:

- `01-naive-on2` — brute force
- `02-kiddo-kdtree` — `kiddo` 6.0 KD-tree
- `03-handrolled-octree` — hand-rolled octree with AABB pruning

Pre-D8, the three implementations were correct but could not be
compared end-to-end: cc-rust's `icp_iterate` signature did not
accept a custom NN, so each variant's wrapper built its tree for
timing and then fell back to cc-rust's brute force for the actual
ICP iteration. The "ICP wall time" column in the old
`decisions.md` was therefore measuring cc-rust's brute force, not
the variant's NN.

**D8 changes this:** cc-rust now exposes `icp_with_nn` and a
`NearestNeighbour` trait. Each of the three variants implements
that trait and runs the full ICP loop against its own NN. This
scenario is the cross-variant end-to-end comparison.

## How to run

```powershell
# From the scenario folder
.\run.ps1
```

The runner builds each variant, runs their `icp_bench` binary at
2k / 5k / 10k Gaussian points, parses the output, and writes:

- `results.json` — machine-readable, one row per (variant, size)
- `RESULTS.md` — human-readable summary with the winner called out

## What to expect

**Correctness:** all three NNs return identical ICP results on the
Gaussian fixture (same RMS, same iteration count, same converged
flag) at every size. This is the D8 correctness proof — the trait
dispatch is correct because all three adapters agree.

**Performance — ICP wall time (end-to-end, NN-driven):** this is
the headline D8 number. Pre-D8 the three variants produced
similar wall times because they fell back to cc-rust's
hard-coded brute force. Post-D8 the trait dispatch is real and
the per-variant NN is actually used inside the ICP loop.

| Variant | N=2k (s) | N=5k (s) | N=10k (s) | Speedup @ 10k |
|---|---|---|---|---|
| `01-naive-on2` | 0.247 | 2.03 | 7.54 | 1.0× |
| `02-kiddo-kdtree` | **0.021** | **0.080** | **0.175** | **43×** |
| `03-handrolled-octree` | 0.359 | 3.34 | 18.5 | 0.41× (slower than naive) |

**The kiddo advantage is now visible end-to-end** at every size
tested. The hand-rolled octree is *slower* than brute force at
10k because its `search()` falls back to depth-first traversal
without AABB pruning (the per-child AABB isn't preserved through
the recursion, so `min_dist_sq` can't fire at the internal-node
level). See `decisions.md` for the breakdown.

**Performance — NN query time:**

| Variant | N=2k (µs/q) | N=5k (µs/q) | N=10k (µs/q) | Scaling |
|---|---|---|---|---|
| `02-kiddo-kdtree` | **0.28** | **0.39** | **0.40** | O(log n), flat |
| `03-handrolled-octree` | 6.51 | 16.8 | 35.9 | O(n) (broken pruning) |

(Brute force has no separate query step — the scan is
interleaved with the rest of the ICP loop.)

## How the winner is picked

The runner picks the winner based on **end-to-end ICP wall time**
(the actual ICP loop, now NN-driven). At N=10k:

- kiddo: 0.175 s → winner
- naive: 7.54 s
- octree: 18.5 s

The end-to-end wall time is the metric the user actually
cares about; the per-query cost is the underlying explanation
for why the wall times differ.

**Winner: `02-kiddo-kdtree`.** The bench numbers are the
canonical evidence; see `decisions.md` for the breakdown.

## See also

- `experiment.toml` — scenario manifest (id, status, metrics, promotion gates)
- `decisions.md` — winner pick + evidence + non-pick rationale
- `results.json` — machine-readable numbers
- `RESULTS.md` — human-readable summary written by `run.ps1`
- [`../2026-08-19-icp-variants/`](../2026-08-19-icp-variants/) — the three unit-tested implementations
- [`../../docs/decisions.md`](../../docs/decisions.md) D8 — the trait + dispatch decision
- [`../../docs/patterns.md`](../../docs/patterns.md) P16 — the trait + adapter pattern
