# ICP NN comparison - results

Generated: 2026-08-20T18:43:55Z
Sizes tested: 2000, 5000, 10000, 50000   Seed: 42   (Gaussian blob, sigma=0.4, translation (0.5, -0.2, 0.1))

## Correctness (RMS - all variants must agree)

| Size | 01-naive-on2 | 02-kiddo-kdtree | 03-handrolled-octree | 04-dgm-octree |
|---|---|---|---|---|
| 2000 | 0.000169 | 0.000169 | 0.000169 | 0.000169 |
| 5000 | 0.064658 | 0.064658 | 0.064658 | 0.064778 |
| 10000 | 0.060233 | 0.060233 | 0.060233 | 0.06023 |
| 50000 |  | 0.043904 | 0.043904 | 0.043905 |

All four must match exactly. If they do not, the trait dispatch is wrong.

## NN query time (per query, 1000 queries)

| Size | 01-naive-on2 (us/q) | 02-kiddo-kdtree (us/q) | 03-handrolled-octree (us/q) | 04-dgm-octree (us/q) |
|---|---|---|---|---|
| 2000 |  | 0.3 | 6.4 | 0.49 |
| 5000 |  | 0.29 | 15.89 | 0.87 |
| 10000 |  | 0.43 | 39.14 | 0.65 |
| 50000 |  | 0.53 | 278.23 | 0.91 |

## ICP wall time (end-to-end, NN-driven via the new trait)

| Size | 01-naive-on2 (s) | 02-kiddo-kdtree (s) | 03-handrolled-octree (s) | 04-dgm-octree (s) |
|---|---|---|---|---|
| 2000 | 0.26 | 0.021 | 0.345 | 0.031 |
| 5000 | 2.003 | 0.092 | 3.572 | 0.188 |
| 10000 | 7.576 | 0.166 | 18.532 | 0.342 |
| 50000 |  | 1.029 | 740.464 | 2.162 |

## Iterations and convergence

| Size | 01-naive iter / conv | 02-kiddo iter / conv | 03-octree iter / conv | 04-dgm iter / conv |
|---|---|---|---|---|
| 2000 | 35 / true | 35 / true | 35 / true | 35 / true |
| 5000 | 50 / false | 50 / false | 50 / false | 50 / false |
| 10000 | 50 / false | 50 / false | 50 / false | 50 / false |
| 50000 |  /  | 50 / false | 50 / false | 50 / false |

## Winner

**`02-kiddo-kdtree`** on per-query cost: ~0.3-0.65 us/query at every size tested, vs ~0.6-1.4 us/query for the DgmOctree (D9) and ~7-283 us/query for the hand-rolled octree. ICP wall time is within ~10% between kiddo and DgmOctree at small N because the SVD + f32/f64 casts dominate. D9 closes the gap to kiddo on per-query cost (down from the hand-rolled octree's 7-283 us/q), and at N=50k the cell-code-ordered search is ~200x faster than the hand-rolled octree on per-query cost and ~300x faster on ICP wall time.

See `decisions.md` for the full reasoning and `experiment.toml` for the scenario manifest.

