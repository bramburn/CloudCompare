//! `DgmOctree` — the core spatial-index structure from
//! CCCoreLib, ported to pure Rust.
//!
//! This is **Phase 3 (incremental)** of the migration: a working
//! octree with multi-level cell addressing and NN search, built
//! on the same **cell-code (Morton-like) addressing** as
//! `DgmOctree.cpp` in CCCoreLib. The full class has 50+ public
//! methods (cell statistics, CC extraction, ray-casting, sphere
//! queries, etc.); this port covers the **core surface** used by
//! ICP and other iterative closest-point workflows:
//!
//! - `build(points, max_level)` — partition a point cloud.
//! - `nearest_neighbor(query)` — find the closest point by cell-
//!   code address. Returns `(point_index, squared_distance)`.
//! - `get_cell_pos(code, level)` — extract the integer cell
//!   position (x, y, z) for a given code and subdivision level.
//!
//! ## Cell code
//!
//! A cell code is a 64-bit integer. At each subdivision level, 3
//! bits encode the (x, y, z) child-cell offset relative to the
//! parent's centre:
//!
//! - bit 0 → x offset (0 = lower, 1 = upper)
//! - bit 1 → y offset
//! - bit 2 → z offset
//!
//! So a code of `0b00_001_010` at level 2 means: the lower-half
//! child in x of the upper-half child in y of the lower-half
//! child in z, at level 2. The cell position is `(0, 1, 0)` at
//! level 2.
//!
//! The maximum level is 21 (21 × 3 = 63 bits + 1 sign bit).
//!
//! ## Why cell codes
//!
//! Cell codes give a **total ordering** on cells that matches
//! the spatial layout: codes that share a high prefix are
//! spatially close. NN search uses this to prune: if the
//! current best distance is d, then any cell whose minimum
//! possible distance to the query point exceeds d² can be
//! skipped. The cell-code distance gives a cheap lower bound.

/// Maximum subdivision level (21 × 3 bits = 63 bits + sign).
pub const MAX_OCTREE_LEVEL: u8 = 21;

use std::collections::HashMap;

/// Bit shift to truncate a cell code to a given level.
/// (MAX_OCTREE_LEVEL - level) * 3.
#[inline]
pub fn get_bit_shift(level: u8) -> u32 {
    (MAX_OCTREE_LEVEL as u32 - level as u32) * 3
}

/// Extract the integer cell position (x, y, z) at `level` from
/// `code`. If `truncated` is false, the code is shifted right
/// by `GET_BIT_SHIFT(level)` first (the code is a *full* code,
/// not yet truncated to the requested level).
///
/// The convention is **MSB-first** at every level: the
/// level-1 (topmost, largest) bits are at the highest
/// bit-position of the (truncated) code, the level-2 bits
/// are at the next-lower bit-position, and so on. The
/// cell position is constructed with the level-1 bit as the
/// MSB of the per-axis cell index, so the spatial cell
/// index matches the bit position (cell_pos.x = level-1_x*4 +
/// level-2_x*2 + level-3_x*1 for level 3). This is the
/// standard convention for binary subdivision.
pub fn get_cell_pos(code: u64, level: u8, truncated: bool) -> (i32, i32, i32) {
    let code = if truncated { code } else { code >> get_bit_shift(level) };
    let mut cell_pos = (0i32, 0i32, 0i32);
    for k in 0..level {
        // k=0 reads the level-1 bits (at the highest
        // position in the truncated code), k=level-1
        // reads the level bits (at the lowest position).
        let code_shift = (level - 1 - k) * 3;
        let cell_shift = level - 1 - k;
        let bits = (code >> code_shift) & 0b111;
        if bits & 0b001 != 0 { cell_pos.0 |= 1 << cell_shift; }
        if bits & 0b010 != 0 { cell_pos.1 |= 1 << cell_shift; }
        if bits & 0b100 != 0 { cell_pos.2 |= 1 << cell_shift; }
    }
    cell_pos
}

/// Compute the cell code for a point at the given subdivision
/// level, given the bounding box. The cell code is a u64 with
/// the format described in the module docs.
///
/// The subdivision is **recursive**: at each level, the current
/// cell's midpoint (not the whole-bbox midpoint) is used to
/// decide which sub-cell the point is in. Bits are packed MSB-
/// first across the truncated code: the level-1 (topmost) bits
/// go in the highest bit-position, level-2 bits in the next,
/// and the level (bottommost) bits in the lowest. The cell
/// position from `get_cell_pos` uses the same convention, so
/// cell_pos.x matches the standard MSB-first spatial cell
/// index (e.g. for level 3, cell_pos.x = level-1_x*4 +
/// level-2_x*2 + level-3_x*1).
pub fn compute_cell_code(
    point: [f32; 3],
    bb_min: [f32; 3],
    bb_max: [f32; 3],
    level: u8,
) -> u64 {
    compute_cell_code_recursive(point, bb_min, bb_max, level, 1)
}

fn compute_cell_code_recursive(
    point: [f32; 3],
    cell_min: [f32; 3],
    cell_max: [f32; 3],
    level: u8,
    depth: u8,  // 1 = topmost (largest, = level 1), level = bottommost (smallest, = level `level`)
) -> u64 {
    if depth > level {
        return 0;
    }
    let mid = [
        (cell_min[0] + cell_max[0]) * 0.5,
        (cell_min[1] + cell_max[1]) * 0.5,
        (cell_min[2] + cell_max[2]) * 0.5,
    ];
    // Bit position in the truncated code: level-1 (topmost) at
    // the highest position, level (bottommost) at the lowest.
    // depth=1 → bit position (level-1)*3; depth=level → bit position 0.
    let code_shift = (level - depth) * 3;
    let mut code: u64 = 0;
    let mut next_min = [0.0_f32; 3];
    let mut next_max = [0.0_f32; 3];
    for axis in 0..3 {
        if point[axis] < mid[axis] {
            // Lower half: bit 0.
            next_min[axis] = cell_min[axis];
            next_max[axis] = mid[axis];
        } else {
            // Upper half: bit set.
            let axis_bit = match axis {
                0 => 1u64,    // x
                1 => 2u64,    // y
                2 => 4u64,    // z
                _ => unreachable!(),
            };
            code |= axis_bit << code_shift;
            next_min[axis] = mid[axis];
            next_max[axis] = cell_max[axis];
        }
    }
    // Recurse into the chosen sub-cell at the next deeper level.
    let next_code = compute_cell_code_recursive(point, next_min, next_max, level, depth + 1);
    code | next_code
}

/// A point cloud plus its multi-level cell addressing. Built
/// once; queries are O(log n) average.
pub struct DgmOctree {
    /// Bounding box of the input cloud.
    pub bb_min: [f32; 3],
    pub bb_max: [f32; 3],
    /// Points in input order. The cell addressing is built
    /// separately, sorted by cell code.
    pub points: Vec<[f32; 3]>,
    /// Truncated cell code for each point at `build_level`.
    pub codes: Vec<u64>,
    /// `build_level` used for `codes`.
    pub build_level: u8,
    /// Number of unique cells at `build_level`. (For future
    /// expansion to multi-level queries.)
    pub cell_count: usize,
    /// Pre-sorted point indices by cell code. Cached at
    /// `build()` time so `nearest_neighbor` doesn't have to
    /// sort on every call. The slice `[s..e]` in
    /// `sorted_indices` corresponds to all points with the
    /// same cell code (see `cell_ranges`).
    pub sorted_indices: Vec<usize>,
    /// Cell code → (start, end) in `sorted_indices` for that
    /// cell. Cached at `build()` time for O(1) cell lookup
    /// during the D9 shell expansion. HashMap (not dense
    /// Vec) so memory scales with the populated cell count,
    /// not the maximum possible at `build_level`.
    pub cell_ranges: HashMap<u64, (usize, usize)>,
}

impl DgmOctree {
    /// Build the octree at the given subdivision level.
    /// The level is bounded by `MAX_OCTREE_LEVEL`. Points are
    /// left in input order; `codes[i]` is the truncated cell
    /// code of `points[i]`. The cell-code → (start, end)
    /// range map and the pre-sorted index array are also
    /// built once here so `nearest_neighbor` doesn't pay
    /// the O(n log n) sort cost on every query.
    pub fn build(points: &[[f32; 3]], level: u8) -> Self {
        assert!(level <= MAX_OCTREE_LEVEL, "level exceeds MAX_OCTREE_LEVEL");
        if points.is_empty() {
            return Self {
                bb_min: [0.0; 3],
                bb_max: [0.0; 3],
                points: Vec::new(),
                codes: Vec::new(),
                build_level: level,
                cell_count: 0,
                sorted_indices: Vec::new(),
                cell_ranges: HashMap::new(),
            };
        }
        // Compute bounding box.
        let mut bb_min = [f32::MAX; 3];
        let mut bb_max = [f32::MIN; 3];
        for p in points {
            for i in 0..3 {
                if p[i] < bb_min[i] {
                    bb_min[i] = p[i];
                }
                if p[i] > bb_max[i] {
                    bb_max[i] = p[i];
                }
            }
        }
        // Pad slightly so points on the boundary don't get
        // classified into the next cell.
        for i in 0..3 {
            let pad = (bb_max[i] - bb_min[i]).max(1e-6) * 0.01;
            bb_min[i] -= pad;
            bb_max[i] += pad;
        }
        // Compute each point's truncated cell code.
        let mut codes = Vec::with_capacity(points.len());
        for p in points {
            codes.push(compute_cell_code(*p, bb_min, bb_max, level));
        }
        // Build the pre-sorted index array and the cell
        // ranges. Walk through the sorted order, recording
        // each cell's [s..e) range. This is O(n log n) once
        // at build time, not per query.
        let n = points.len();
        let mut sorted_indices: Vec<usize> = (0..n).collect();
        sorted_indices.sort_by_key(|&i| codes[i]);
        let mut cell_ranges: HashMap<u64, (usize, usize)> = HashMap::new();
        let mut s = 0;
        while s < n {
            let code = codes[sorted_indices[s]];
            let mut e = s + 1;
            while e < n && codes[sorted_indices[e]] == code {
                e += 1;
            }
            cell_ranges.insert(code, (s, e));
            s = e;
        }
        let cell_count = cell_ranges.len();

        Self {
            bb_min,
            bb_max,
            points: points.to_vec(),
            codes,
            build_level: level,
            cell_count,
            sorted_indices,
            cell_ranges,
        }
    }

    /// Cell size at the given level. The bounding box is
    /// divided into `2^level` cells per axis.
    pub fn cell_size(&self, level: u8) -> [f32; 3] {
        let cells = 1u32 << level;
        [
            (self.bb_max[0] - self.bb_min[0]) / cells as f32,
            (self.bb_max[1] - self.bb_min[1]) / cells as f32,
            (self.bb_max[2] - self.bb_min[2]) / cells as f32,
        ]
    }

    /// Cell centre at the given level and cell position
    /// (x, y, z) — each in [0, 2^level).
    pub fn cell_center(&self, level: u8, cell_pos: (i32, i32, i32)) -> [f32; 3] {
        let cell = self.cell_size(level);
        [
            self.bb_min[0] + (cell_pos.0 as f32 + 0.5) * cell[0],
            self.bb_min[1] + (cell_pos.1 as f32 + 0.5) * cell[1],
            self.bb_min[2] + (cell_pos.2 as f32 + 0.5) * cell[2],
        ]
    }

    /// Find the nearest point to `query`. **D9 (2026-08-20):**
    /// cell-code-ordered search with **Chebyshev-distance shell
    /// expansion** and spatial AABB pruning. Returns
    /// `(point_index, squared_distance)`.
    ///
    /// Algorithm (see D9 in `experimental/docs/decisions.md`):
    ///
    /// 1. For a query, compute the query's cell position
    ///    `(qx, qy, qz)` at `build_level`.
    /// 2. **Jump optimisation** (mirrors the C++ algorithm
    ///    in `DgmOctree.cpp` lines 1212-1235): if the query
    ///    is outside the cloud's bbox, compute the per-axis
    ///    `dist_to_border` (in cell units) and start the
    ///    shell expansion at `max(dist_to_border)`, skipping
    ///    the empty shells between the query and the cloud.
    /// 3. Expand Chebyshev-distance shells from there
    ///    (`d = start_d, start_d+1, ...`). For each cell in
    ///    the shell, compute the spatial minimum distance
    ///    from the query to the cell's AABB. If that exceeds
    ///    the current best squared distance, skip the cell.
    ///    Otherwise look up the cell's range in the cached
    ///    `cell_ranges` HashMap (O(1)) and scan the points.
    /// 4. Stop when the lower bound on the AABB min
    ///    distance to the most-aligned cell in shell d
    ///    exceeds `sqrt(best_d2)`. The lower bound depends
    ///    on whether the query is inside or outside the
    ///    bbox — see the comment at the termination check.
    ///
    /// The sorted-by-cell-code order and the cell → range
    /// map are pre-computed in `build()` so this method is
    /// pay-per-query only: no per-query sort, no per-query
    /// cell-map construction. For typical ICP use (query
    /// near a known model point, NN in a nearby cell), the
    /// expansion visits O(1) to O(27) cells and scans ~4
    /// points per cell (the target build-level occupancy).
    pub fn nearest_neighbor(&self, query: [f32; 3]) -> (usize, f32) {
        if self.points.is_empty() {
            return (0, f32::INFINITY);
        }
        let level = self.build_level;
        let cell = self.cell_size(level);
        let max_dim = 1i32 << level;
        let max_cell = max_dim - 1;
        let query_code = compute_cell_code(query, self.bb_min, self.bb_max, level);
        let (qx, qy, qz) = get_cell_pos(query_code, level, true);
        let cell_max_dim = cell[0].max(cell[1]).max(cell[2]) as f64;

        // Jump optimisation: if the query's cell is outside
        // the valid cell range (e.g. the query is past the
        // cloud's bbox), start the shell expansion at the
        // nearest in-bounds cell. The C++ version measures
        // this in cell indices (cellPos is already in cell
        // units), so dist_to_border here is in cells, not
        // in physical units.
        let mut dist_to_border: [i32; 3] = [0; 3];
        let mut query_outside = false;
        for axis in 0..3 {
            let q = [qx, qy, qz][axis];
            if q < 0 {
                dist_to_border[axis] = -q;
                query_outside = true;
            } else if q > max_cell {
                dist_to_border[axis] = q - max_cell;
                query_outside = true;
            }
        }
        let start_d: i32 = if query_outside {
            dist_to_border[0]
                .max(dist_to_border[1])
                .max(dist_to_border[2])
        } else {
            0
        };

        // min_dist_to_border: distance from the query to
        // the closest face of the query's (in-bounds) cell.
        // This is the "free distance" we already have — the
        // query is partway into the cell, and any d=1 cell
        // shares a face with it. Critical for the
        // early-termination check below: without it, the
        // check uses the Chebyshev distance from the cell
        // *index* and incorrectly terminates before
        // visiting cells adjacent to the query's cell face.
        let cell_aabb_min = [
            self.bb_min[0] + qx as f32 * cell[0],
            self.bb_min[1] + qy as f32 * cell[1],
            self.bb_min[2] + qz as f32 * cell[2],
        ];
        let cell_aabb_max = [
            cell_aabb_min[0] + cell[0],
            cell_aabb_min[1] + cell[1],
            cell_aabb_min[2] + cell[2],
        ];
        let mut min_dist_to_border = f32::INFINITY;
        for i in 0..3 {
            if query[i] < cell_aabb_min[i] {
                min_dist_to_border =
                    min_dist_to_border.min(cell_aabb_min[i] - query[i]);
            } else if query[i] > cell_aabb_max[i] {
                min_dist_to_border =
                    min_dist_to_border.min(query[i] - cell_aabb_max[i]);
            } else {
                let d_min = query[i] - cell_aabb_min[i];
                let d_max = cell_aabb_max[i] - query[i];
                min_dist_to_border = min_dist_to_border.min(d_min.min(d_max));
            }
        }

        let mut best_idx: usize = 0;
        let mut best_d2 = f64::INFINITY;

        // Chebyshev-distance shell expansion. At distance
        // d, the shell is the set of cells with
        // max(|dx|, |dy|, |dz|) == d.
        let mut d: i32 = start_d;
        loop {
            // Prune: if the lower bound on the AABB min
            // distance to the most-aligned cell in shell d
            // exceeds sqrt(best_d2), every cell in this
            // shell (and beyond) is at least that far from
            // the query, so no further cell can contain a
            // closer point.
            //
            // The lower bound is:
            //   inside the bbox:  min_dist_to_border +
            //                      (d - 1) * cell_max_dim
            //   outside the bbox: min_dist_to_border +
            //                      d * cell_max_dim
            // The (d - 1) for inside queries captures the
            // fact that the d=1 cells share a face with the
            // query's cell — the per-axis AABB min distance
            // is just `min_dist_to_border` (the query is
            // already partway into the cell). For outside
            // queries, the d=start_d cells are at least one
            // cell further out.
            //
            // Skipped on the first iteration (`d == start_d`)
            // because we haven't set best_d2 yet (it's still
            // infinity and the comparison would always
            // trigger).
            if d > start_d {
                let extra: f64 = if query_outside {
                    d as f64
                } else {
                    (d - 1) as f64
                };
                let lower_bound = min_dist_to_border as f64 + extra * cell_max_dim;
                if lower_bound * lower_bound > best_d2 {
                    break;
                }
            }

            // Visit all cells in this shell.
            for dx in -d..=d {
                for dy in -d..=d {
                    for dz in -d..=d {
                        // Shell condition: at least one of
                        // |dx|, |dy|, |dz| equals d (and the
                        // others are ≤ d).
                        if dx.abs().max(dy.abs()).max(dz.abs()) != d {
                            continue;
                        }
                        let cx = qx + dx;
                        let cy = qy + dy;
                        let cz = qz + dz;
                        if cx < 0 || cy < 0 || cz < 0
                            || cx >= max_dim || cy >= max_dim || cz >= max_dim
                        {
                            continue;
                        }
                        let cell_code = pack_cell_pos(cx, cy, cz, level);

                        // Compute the spatial min distance
                        // from the query to this cell's AABB.
                        let aabb_min = [
                            self.bb_min[0] + cx as f32 * cell[0],
                            self.bb_min[1] + cy as f32 * cell[1],
                            self.bb_min[2] + cz as f32 * cell[2],
                        ];
                        let aabb_max = [
                            aabb_min[0] + cell[0],
                            aabb_min[1] + cell[1],
                            aabb_min[2] + cell[2],
                        ];
                        let min_d2 = aabb_min_dist_sq(query, aabb_min, aabb_max);
                        if min_d2 > best_d2 {
                            continue;
                        }

                        // Look up this cell's range in the
                        // pre-computed HashMap (O(1)) and
                        // scan its points.
                        if let Some(&(s, e)) = self.cell_ranges.get(&cell_code) {
                            for &i in &self.sorted_indices[s..e] {
                                let d2 = point_dist_sq(query, self.points[i]);
                                if d2 < best_d2 {
                                    best_d2 = d2;
                                    best_idx = i;
                                }
                            }
                        }
                    }
                }
            }

            d += 1;
            if d > max_dim {
                break;
            }
        }

        (best_idx, best_d2 as f32)
    }

    /// Return the cell code for the point at `point_index`.
    /// Used for testing and for the future NN search with
    /// pruning.
    pub fn code_for(&self, point_index: usize) -> Option<u64> {
        self.codes.get(point_index).copied()
    }
}

/// `NearestNeighbour` adapter for `DgmOctree` (D9, 2026-08-20).
///
/// This is what plugs the cell-code-ordered octree into the
/// D8 `icp_with_nn` entry point. The adapter holds the model
/// in `f32` (the D8 trait contract) and constructs a
/// `DgmOctree` on first query, then reuses it.
pub struct DgmOctreeNN {
    tree: DgmOctree,
}

impl DgmOctreeNN {
    /// Build the cell-code octree from a flat `f32` point
    /// cloud (the layout cc-rust uses for `model_points`).
    /// The build level is the smallest `L` such that the
    /// resulting average cell occupancy is ~4 (so the
    /// per-cell AABB pruning is effective without making
    /// the build too coarse). For n points: L = clamp(4,
    /// 0, MAX_OCTREE_LEVEL - 1) where the upper bound is set
    /// so the cell count is at most ~n/4.
    pub fn build(model_points: &[f32]) -> Self {
        let n = model_points.len() / 3;
        if n == 0 {
            return Self { tree: DgmOctree::build(&[], 0) };
        }
        // Aim for ~4 points per cell at the build level.
        // n / 8^L ≈ 4  →  L ≈ log_8(n / 4)
        let target_cells = (n / 4).max(1) as f64;
        let l_f = (target_cells.log2() / 3.0).ceil().max(0.0) as i32;
        let l = (l_f as u8).clamp(1, MAX_OCTREE_LEVEL);
        let points: Vec<[f32; 3]> = (0..n)
            .map(|i| [model_points[i * 3], model_points[i * 3 + 1], model_points[i * 3 + 2]])
            .collect();
        Self { tree: DgmOctree::build(&points, l) }
    }
}

impl crate::registration::NearestNeighbour for DgmOctreeNN {
    fn nearest(&self, query: &[f32; 3]) -> (usize, f32) {
        self.tree.nearest_neighbor(*query)
    }
}

// ── NN-search helpers ───────────────────────────────────────────────

/// Squared distance from a point to a 3D AABB. Used by the
/// D9 cell-code-ordered NN search to prune cells whose
/// minimum possible distance exceeds the current best.
#[inline]
fn aabb_min_dist_sq(
    query: [f32; 3],
    aabb_min: [f32; 3],
    aabb_max: [f32; 3],
) -> f64 {
    let mut d2 = 0.0_f64;
    for i in 0..3 {
        if query[i] < aabb_min[i] {
            let dx = aabb_min[i] - query[i];
            d2 += (dx * dx) as f64;
        } else if query[i] > aabb_max[i] {
            let dx = query[i] - aabb_max[i];
            d2 += (dx * dx) as f64;
        }
    }
    d2
}

/// Pack a cell position (x, y, z) at the truncated level into
/// a cell code. This is the inverse of `get_cell_pos` (with
/// the MSB-first convention) and is used by the D9 NN search
/// to look up cell codes from the shell-expansion coordinates.
#[inline]
fn pack_cell_pos(x: i32, y: i32, z: i32, level: u8) -> u64 {
    let mut code: u64 = 0;
    for k in 0..level {
        // k=0 → level-1 bit (MSB of cell_pos) at the highest
        // code position. k=level-1 → level bit (LSB of cell_pos)
        // at the lowest code position.
        let code_shift = (level - 1 - k) * 3;
        let cell_shift = level - 1 - k;
        let bits = ((x >> cell_shift) & 1)
            | (((y >> cell_shift) & 1) << 1)
            | (((z >> cell_shift) & 1) << 2);
        code |= (bits as u64) << code_shift;
    }
    code
}

/// Squared distance from `query` to a single point.
#[inline]
fn point_dist_sq(query: [f32; 3], p: [f32; 3]) -> f64 {
    let dx = (p[0] - query[0]) as f64;
    let dy = (p[1] - query[1]) as f64;
    let dz = (p[2] - query[2]) as f64;
    dx * dx + dy * dy + dz * dz
}

#[cfg(test)]
mod tests {
    use super::*;

    fn approx_eq(a: f32, b: f32) -> bool {
        (a - b).abs() < 1e-5
    }

    #[test]
    fn get_cell_pos_round_trip() {
        // For any truncated cell code at any level, the cell
        // position should round-trip: extract, then re-encode
        // the same code at the truncated position (bits
        // 0..level*3, with level-1 at the HIGHEST position).
        for level in 1..=5u8 {
            for raw in 0..(1u64 << (level * 3)) {
                // Treat `raw` as the truncated code. Extract
                // the position, then re-encode. The convention is
                // MSB-first: level-1 bits go at the highest
                // position of the truncated code, level bits at
                // the lowest.
                let (x, y, z) = get_cell_pos(raw, level, true);
                let mut reencoded: u64 = 0;
                for k in 0..level {
                    // k=0 → level-1 (MSB of cell_pos, highest code position)
                    // k=level-1 → level (LSB of cell_pos, lowest code position)
                    let code_shift = (level - 1 - k) * 3;
                    let cell_shift = level - 1 - k;
                    let bits = (x >> cell_shift) & 1
                        | (((y >> cell_shift) & 1) << 1)
                        | (((z >> cell_shift) & 1) << 2);
                    reencoded |= (bits as u64) << code_shift;
                }
                assert_eq!(reencoded, raw,
                    "level={} raw={} (x={},y={},z={})", level, raw, x, y, z);
                let _ = approx_eq; // suppress unused warning
            }
        }
    }

    #[test]
    fn get_cell_pos_known_values() {
        // bit 0 = x, bit 1 = y, bit 2 = z (per level)
        // At level 1, code 0b000 = (0, 0, 0)
        // At level 1, code 0b001 = (1, 0, 0)
        // At level 1, code 0b010 = (0, 1, 0)
        // At level 1, code 0b100 = (0, 0, 1)
        let codes: [(u64, i32, i32, i32); 8] = [
            (0b000, 0, 0, 0),
            (0b001, 1, 0, 0),
            (0b010, 0, 1, 0),
            (0b011, 1, 1, 0),
            (0b100, 0, 0, 1),
            (0b101, 1, 0, 1),
            (0b110, 0, 1, 1),
            (0b111, 1, 1, 1),
        ];
        for (code, x, y, z) in codes {
            let (px, py, pz) = get_cell_pos(code, 1, true);
            assert_eq!((px, py, pz), (x, y, z), "code 0b{:03b}", code);
        }
    }

    #[test]
    fn compute_cell_code_assigns_unique_codes() {
        // Different points in different sub-cells should get
        // different codes (at level 1, the 8 octants of the unit
        // cube get codes 0..7).
        let points = vec![
            [0.1, 0.1, 0.1],  // (0, 0, 0)
            [0.9, 0.1, 0.1],  // (1, 0, 0)
            [0.1, 0.9, 0.1],  // (0, 1, 0)
            [0.9, 0.9, 0.1],  // (1, 1, 0)
            [0.1, 0.1, 0.9],  // (0, 0, 1)
            [0.9, 0.1, 0.9],  // (1, 0, 1)
            [0.1, 0.9, 0.9],  // (0, 1, 1)
            [0.9, 0.9, 0.9],  // (1, 1, 1)
        ];
        let codes: Vec<u64> = points
            .iter()
            .map(|p| compute_cell_code(*p, [0.0, 0.0, 0.0], [1.0, 1.0, 1.0], 1))
            .collect();
        // All 8 codes should be distinct.
        let unique: std::collections::HashSet<u64> = codes.iter().copied().collect();
        assert_eq!(unique.len(), 8, "expected 8 distinct codes, got {:?}", codes);
    }

    #[test]
    fn build_assigns_codes_and_finds_nearest() {
        let points = vec![
            [0.0, 0.0, 0.0],
            [1.0, 1.0, 1.0],
            [0.5, 0.5, 0.5],
            [2.0, 3.0, 4.0],
        ];
        let tree = DgmOctree::build(&points, 2);
        assert_eq!(tree.points.len(), 4);
        assert_eq!(tree.codes.len(), 4);
        assert!(tree.cell_count > 0);
        // Query near (0.1, 0, 0) should find points[0].
        let (idx, d2) = tree.nearest_neighbor([0.1, 0.0, 0.0]);
        assert_eq!(idx, 0);
        assert!(d2 < 0.02);
        // Query near (2.0, 3.0, 4.05) should find points[3].
        let (idx, d2) = tree.nearest_neighbor([2.0, 3.0, 4.05]);
        assert_eq!(idx, 3);
        assert!(d2 < 0.02);
    }

    #[test]
    fn cell_center_lies_within_bbox() {
        // The cell centre at any level/position should lie
        // within the bounding box.
        let points = vec![[0.0, 0.0, 0.0], [1.0, 1.0, 1.0]];
        let tree = DgmOctree::build(&points, 3);
        for level in 1..=3u8 {
            for xi in 0..(1u32 << level) {
                for yi in 0..(1u32 << level) {
                    for zi in 0..(1u32 << level) {
                        let c = tree.cell_center(level, (xi as i32, yi as i32, zi as i32));
                        assert!(c[0] >= tree.bb_min[0] - 1e-3
                                && c[0] <= tree.bb_max[0] + 1e-3);
                        assert!(c[1] >= tree.bb_min[1] - 1e-3
                                && c[1] <= tree.bb_max[1] + 1e-3);
                        assert!(c[2] >= tree.bb_min[2] - 1e-3
                                && c[2] <= tree.bb_max[2] + 1e-3);
                    }
                }
            }
        }
    }

    /// D9 (2026-08-20): the cell-code-ordered NN must agree
    /// with the brute-force NN on every query, on a small
    /// fixture. This is the correctness test for the
    /// pruning — if the AABB min-distance check is wrong,
    /// a valid cell will be skipped and a non-NN returned.
    #[test]
    fn d9_nearest_matches_brute_force_on_small_fixture() {
        // 27 points in a 3x3x3 grid (one point per cell at
        // level 1 — 8 cells, but the points are slightly
        // perturbed to make NN non-trivial).
        let mut points: Vec<[f32; 3]> = Vec::new();
        for i in 0..3 {
            for j in 0..3 {
                for k in 0..3 {
                    points.push([
                        i as f32 * 1.0 + (k as f32 * 0.05),
                        j as f32 * 1.0 + (i as f32 * 0.03),
                        k as f32 * 1.0 + (j as f32 * 0.07),
                    ]);
                }
            }
        }
        let tree = DgmOctree::build(&points, 3);

        // For each query (every point in the cloud, perturbed
        // by a small random offset), the cell-code-ordered
        // NN must agree with brute force.
        for (i, q) in points.iter().enumerate() {
            let q_perturbed = [
                q[0] + 0.01 * (i as f32 * 0.7).sin(),
                q[1] + 0.01 * (i as f32 * 1.3).cos(),
                q[2] + 0.01 * (i as f32 * 0.5).sin(),
            ];
            let (idx_d9, d2_d9) = tree.nearest_neighbor(q_perturbed);

            // Brute force: scan all points.
            let mut best_idx = 0;
            let mut best_d2 = f32::INFINITY;
            for (j, p) in points.iter().enumerate() {
                let dx = p[0] - q_perturbed[0];
                let dy = p[1] - q_perturbed[1];
                let dz = p[2] - q_perturbed[2];
                let d2 = dx * dx + dy * dy + dz * dz;
                if d2 < best_d2 {
                    best_d2 = d2;
                    best_idx = j;
                }
            }
            // Both NNs must point to the same model point and
            // report the same squared distance.
            assert_eq!(idx_d9, best_idx,
                       "D9 NN disagrees with brute force at query {}: d9={} bf={}",
                       i, idx_d9, best_idx);
            assert!((d2_d9 - best_d2).abs() < 1e-5,
                    "D9 distance disagrees with brute force at query {}: d9={} bf={}",
                    i, d2_d9, best_d2);
        }
    }

    /// D9: same test as above but on a Gaussian blob, which
    /// has a much larger point count and no grid structure.
    /// This is the realistic ICP fixture.
    #[test]
    fn d9_nearest_matches_brute_force_on_gaussian() {
        // 500 Gaussian points is small enough to brute force
        // in the test but large enough to exercise the
        // pruning meaningfully.
        let points = gaussian_cloud(500, 0.5, 42);
        let tree = DgmOctree::build(&points, 5);

        // 50 random queries.
        let mut state: u64 = 12345;
        for q in 0..50 {
            // Pseudo-random query near the cloud.
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            let dx = (state as f64 / u64::MAX as f64) as f32;
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            let dy = (state as f64 / u64::MAX as f64) as f32;
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            let dz = (state as f64 / u64::MAX as f64) as f32;
            let query = [dx * 2.0, dy * 2.0, dz * 2.0];

            let (idx_d9, d2_d9) = tree.nearest_neighbor(query);

            // Brute force.
            let mut best_idx = 0;
            let mut best_d2 = f32::INFINITY;
            for (j, p) in points.iter().enumerate() {
                let ddx = p[0] - query[0];
                let ddy = p[1] - query[1];
                let ddz = p[2] - query[2];
                let d2 = ddx * ddx + ddy * ddy + ddz * ddz;
                if d2 < best_d2 {
                    best_d2 = d2;
                    best_idx = j;
                }
            }
            assert_eq!(idx_d9, best_idx,
                       "D9 NN disagrees at query {}: d9={} bf={}", q, idx_d9, best_idx);
            assert!((d2_d9 - best_d2).abs() < 1e-4,
                    "D9 distance disagrees at query {}: d9={} bf={}", q, d2_d9, best_d2);
        }
    }

    /// D9: the cell-code-ordered NN must be FASTER than
    /// brute force on a Gaussian of 5k points, on average,
    /// AND must return the same NN as brute force on the
    /// same fixture. This is the headline perf test — if
    /// the pruning is broken, the per-query cost stays
    /// O(n) and the timing assertion fails. If the cell
    /// code or the AABB check is wrong, the correctness
    /// assertions fail.
    ///
    /// **Query scope:** queries are drawn from the same
    /// `[-1, 1]^3` cube as the Gaussian's main mass. This
    /// matches the ICP use case (each data point queries a
    /// nearby model point after a few alignment iterations).
    /// Queries drawn from a wider cube (e.g. `[0, 3]^3`) are
    /// far from the cloud, the shell expansion visits many
    /// empty cells, and even the C++ version is slower than
    /// brute force in that regime — see the discussion in
    /// `experimental/docs/decisions.md` D9.
    #[test]
    fn d9_nearest_is_faster_than_brute_force_on_gaussian_5k() {
        use std::time::Instant;
        let points = gaussian_cloud(5000, 0.5, 99);
        let tree = DgmOctree::build(&points, 6);

        // 200 queries, deterministic, drawn from the
        // Gaussian's main mass [-1, 1]^3.
        let mut state: u64 = 7777;
        let mut queries: Vec<[f32; 3]> = Vec::with_capacity(200);
        for _ in 0..200 {
            state ^= state << 13; state ^= state >> 7; state ^= state << 17;
            let x = (state as f64 / u64::MAX as f64) as f32;
            state ^= state << 13; state ^= state >> 7; state ^= state << 17;
            let y = (state as f64 / u64::MAX as f64) as f32;
            state ^= state << 13; state ^= state >> 7; state ^= state << 17;
            let z = (state as f64 / u64::MAX as f64) as f32;
            queries.push([x * 2.0 - 1.0, y * 2.0 - 1.0, z * 2.0 - 1.0]);
        }

        // Brute force: compute the correct NN for each
        // query, and check the D9 result matches. Use a
        // real index (`j`) so the compiler can't elide the
        // inner loop — the previous `best_idx = 0`
        // hardcoded the index, which let LLVM prove the
        // whole loop was a no-op and report bf_elapsed = 0us.
        for (qi, q) in queries.iter().enumerate() {
            let mut best_idx: usize = 0;
            let mut best_d2 = f32::INFINITY;
            for (j, p) in points.iter().enumerate() {
                let dx = p[0] - q[0];
                let dy = p[1] - q[1];
                let dz = p[2] - q[2];
                let d2 = dx * dx + dy * dy + dz * dz;
                if d2 < best_d2 {
                    best_d2 = d2;
                    best_idx = j;
                }
            }
            let (idx_d9, d2_d9) = tree.nearest_neighbor(*q);
            assert_eq!(idx_d9, best_idx,
                       "D9 NN disagrees at query {}: d9={} bf={}", qi, idx_d9, best_idx);
            assert!((d2_d9 - best_d2).abs() < 1e-4,
                    "D9 distance disagrees at query {}: d9={} bf={}", qi, d2_d9, best_d2);
        }

        // Brute force timing (now on the fully-validated
        // fixture so the timing comparison is meaningful).
        let t0 = Instant::now();
        let mut bf_total: u64 = 0;
        for q in &queries {
            let mut best_idx: usize = 0;
            let mut best_d2 = f32::INFINITY;
            for (j, p) in points.iter().enumerate() {
                let dx = p[0] - q[0];
                let dy = p[1] - q[1];
                let dz = p[2] - q[2];
                let d2 = dx * dx + dy * dy + dz * dz;
                if d2 < best_d2 {
                    best_d2 = d2;
                    best_idx = j;
                }
            }
            bf_total = bf_total.wrapping_add(best_idx as u64);
        }
        let bf_elapsed = t0.elapsed();

        // D9 timing.
        let t0 = Instant::now();
        let mut d9_total: u64 = 0;
        for q in &queries {
            let (idx, _d2) = tree.nearest_neighbor(*q);
            d9_total = d9_total.wrapping_add(idx as u64);
        }
        let d9_elapsed = t0.elapsed();

        // Touch both totals to keep the compiler from
        // eliding the loops. A non-zero total confirms the
        // brute-force loop actually ran.
        assert!(bf_total != 0 || d9_total != 0,
                "both totals are 0 — compiler elided the loops");

        let bf_us = bf_elapsed.as_micros();
        let d9_us = d9_elapsed.as_micros();
        // D9 should be at least 2x faster than brute force
        // on 5k Gaussian points with realistic queries.
        assert!(d9_us * 2 < bf_us.max(1),
                "D9 not fast enough: brute={}us, d9={}us", bf_us, d9_us);
    }

    /// D9 + D8: the DgmOctreeNN adapter must plug into
    /// `icp_with_nn` and produce the same RMS as brute
    /// force on a small fixture.
    #[test]
    fn dgm_octree_nn_plugs_into_icp_with_nn() {
        use crate::registration::{icp_with_nn, BruteForceNN, IcprParamsRust};
        // Use the asymmetric-9 fixture: 8 cube corners + 1
        // off-axis point. The off-axis point makes the
        // cross-covariance H full-rank, so ICP can recover
        // the translation.
        let model: Vec<f32> = vec![
            0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
            1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0,
            1.5, 0.3, 0.7,
        ];
        let data_offset = [0.4, -0.1, 0.2];
        let mut data: Vec<f32> = (0..model.len() / 3)
            .flat_map(|i| {
                let i3 = i * 3;
                vec![model[i3] + data_offset[0],
                     model[i3 + 1] + data_offset[1],
                     model[i3 + 2] + data_offset[2]]
            })
            .collect();

        let params = IcprParamsRust {
            max_iterations: 50,
            min_rms_decrease: 1e-6,
            ..Default::default()
        };

        // Run ICP with the DgmOctreeNN adapter.
        let octree_nn = DgmOctreeNN::build(&model);
        let r_d9 = icp_with_nn(&mut data, &model, &octree_nn, &params).expect("D9 ICP");

        // Run ICP with brute force (ground truth) on a
        // fresh data copy.
        let mut data_bf = data.clone();
        let bf_nn = BruteForceNN::new(&model);
        let r_bf = icp_with_nn(&mut data_bf, &model, &bf_nn, &params).expect("BF ICP");

        // Same RMS (within fp tolerance) and same iteration count.
        assert!((r_d9.rms - r_bf.rms).abs() < 1e-5,
                "D9 ICP RMS differs from BF: d9={} bf={}", r_d9.rms, r_bf.rms);
        assert_eq!(r_d9.iterations, r_bf.iterations);
    }

    // ── test helpers ──────────────────────────────────────────────

    /// Generate `n` Gaussian-distributed 3D points with
    /// standard deviation `sigma`, seeded by `seed`. Used by
    /// the D9 correctness and perf tests. Same xorshift
    /// PRNG pattern as the existing registration.rs tests.
    fn gaussian_cloud(n: usize, sigma: f32, seed: u64) -> Vec<[f32; 3]> {
        let mut state = seed;
        let mut next_u = || -> f32 {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            (state as f64 / u64::MAX as f64) as f32
        };
        let mut next_normal = || -> f32 {
            let u1 = next_u().max(1e-6);
            let u2 = next_u();
            let r = (-2.0 * u1.ln()).sqrt();
            let theta = 2.0 * std::f32::consts::PI * u2;
            r * theta.cos() * sigma
        };
        let mut out = Vec::with_capacity(n);
        for _ in 0..n {
            out.push([next_normal(), next_normal(), next_normal()]);
        }
        out
    }
}
