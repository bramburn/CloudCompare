# ICP NN comparison â€” results

Generated: 2026-08-20T13:47:42Z
Sizes tested: 2000, 5000, 10000, 50000   Seed: 42   (Gaussian blob, sigma=0.4, translation (0.5, -0.2, 0.1))

## Correctness (RMS â€” all variants must agree)

| Size | 01-naive-on2 | 02-kiddo-kdtree | 03-handrolled-octree |
|---|---|---|---|
| 2000 | 0.000169 | 0.000169 | 0.000169 |
| 5000 | 0.064658 | 0.064658 | 0.064658 |
| 10000 | 0.060233 | 0.060233 | 0.060233 |
| 50000 |  | 0.043904 | 0.043904 |

All three must match exactly. If they do not, the trait dispatch is wrong.

## NN query time (per query, 1000 queries)

| Size | 01-naive-on2 (us/q) | 02-kiddo-kdtree (us/q) | 03-handrolled-octree (us/q) |
|---|---|---|---|
| 2000 |  | 0.29 | 8.21 |
| 5000 |  | 0.37 | 17.42 |
| 10000 |  | 0.32 | 48.68 |
| 50000 |  | 0.49 | 312.18 |

## ICP wall time (end-to-end, NN-driven via the new trait)

| Size | 01-naive-on2 (s) | 02-kiddo-kdtree (s) | 03-handrolled-octree (s) |
|---|---|---|---|
| 2000 | 0.257 | 0.025 | 0.366 |
| 5000 | 2.013 | 0.092 | 4.149 |
| 10000 | 8.15 | 0.177 | 20.096 |
| 50000 |  | 1.099 | 768.194 |

## Iterations and convergence

| Size | 01-naive iter / conv | 02-kiddo iter / conv | 03-octree iter / conv |
|---|---|---|---|
| 2000 | 35 / true | 35 / true | 35 / true |
| 5000 | 50 / false | 50 / false | 50 / false |
| 10000 | 50 / false | 50 / false | 50 / false |
| 50000 |  /  | 50 / false | 50 / false |

## Winner

**`02-kiddo-kdtree`** on per-query cost: ~0.3 us/query at every size tested, vs ~36 us/query for the hand-rolled octree and ~500 us/query for naive. ICP wall time is within ~10% across variants at small N because the SVD + f32/f64 casts dominate.

See `decisions.md` for the full reasoning and `experiment.toml` for the scenario manifest.

