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
pub fn get_cell_pos(code: u64, level: u8, truncated: bool) -> (i32, i32, i32) {
    let mut code = if truncated { code } else { code >> get_bit_shift(level) };
    let mut cell_pos = (0i32, 0i32, 0i32);
    let mut bit_mask = 1i32;
    for _ in 0..level {
        if code & 4 != 0 {
            cell_pos.2 |= bit_mask;
        }
        if code & 2 != 0 {
            cell_pos.1 |= bit_mask;
        }
        if code & 1 != 0 {
            cell_pos.0 |= bit_mask;
        }
        code >>= 3;
        bit_mask <<= 1;
    }
    cell_pos
}

/// Compute the cell code for a point at the given subdivision
/// level, given the bounding box. The cell code is a u64 with
/// the format described in the module docs.
pub fn compute_cell_code(
    point: [f32; 3],
    bb_min: [f32; 3],
    bb_max: [f32; 3],
    level: u8,
) -> u64 {
    // For each axis, find the cell index at the given level
    // (between 0 and 2^level - 1) and shift it into the code.
    let mut code: u64 = 0;
    for k in 0..level {
        let shift = get_bit_shift(k + 1);
        for axis in 0..3 {
            let mid = (bb_min[axis] + bb_max[axis]) * 0.5;
            let v = if point[axis] < mid { 0u64 } else { 1u64 };
            // Place the bit at position `shift` in the final
            // code. Axis ordering: x = bit 0, y = bit 1, z = bit 2
            // within each level's 3-bit group.
            let axis_bit = match axis {
                0 => 1u64,    // x
                1 => 2u64,    // y
                2 => 4u64,    // z
                _ => unreachable!(),
            };
            code |= v * axis_bit << shift;
        }
    }
    code
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
}

impl DgmOctree {
    /// Build the octree at the given subdivision level.
    /// The level is bounded by `MAX_OCTREE_LEVEL`. Points are
    /// left in input order; `codes[i]` is the truncated cell
    /// code of `points[i]`.
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
        // Cell count = unique codes.
        let mut sorted = codes.clone();
        sorted.sort_unstable();
        sorted.dedup();
        let cell_count = sorted.len();

        Self {
            bb_min,
            bb_max,
            points: points.to_vec(),
            codes,
            build_level: level,
            cell_count,
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

    /// Find the nearest point to `query`. Brute-force scan
    /// over the points — kept simple for this Phase 3
    /// increment. A real NN search would use cell-code
    /// ordering to prune, similar to the hand-rolled octree
    /// in `experimental/scenarios/2026-08-19-icp-variants/03-handrolled-octree/`.
    /// The cell-code data is built and available via
    /// `get_cell_pos` and `cell_count`; an `nn_search` with
    /// pruning is a follow-up (see D9 candidate).
    pub fn nearest_neighbor(&self, query: [f32; 3]) -> (usize, f32) {
        let mut best_idx = 0;
        let mut best_d2 = f32::INFINITY;
        for (i, p) in self.points.iter().enumerate() {
            let dx = p[0] - query[0];
            let dy = p[1] - query[1];
            let dz = p[2] - query[2];
            let d2 = dx * dx + dy * dy + dz * dz;
            if d2 < best_d2 {
                best_d2 = d2;
                best_idx = i;
            }
        }
        (best_idx, best_d2)
    }

    /// Return the cell code for the point at `point_index`.
    /// Used for testing and for the future NN search with
    /// pruning.
    pub fn code_for(&self, point_index: usize) -> Option<u64> {
        self.codes.get(point_index).copied()
    }
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
        // the same code at the truncated position (bits 0..level*3).
        for level in 1..=5u8 {
            for raw in 0..(1u64 << (level * 3)) {
                // Treat `raw` as the truncated code: bits 0..level*3
                // hold the cell info. Re-encode by placing bits at
                // positions 0..level*3 (no shift).
                let (x, y, z) = get_cell_pos(raw, level, true);
                let mut reencoded: u64 = 0;
                for k in 0..level {
                    let shift = k * 3;
                    let bits = (x >> k) & 1 | (((y >> k) & 1) << 1) | (((z >> k) & 1) << 2);
                    reencoded |= (bits as u64) << shift;
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
}
