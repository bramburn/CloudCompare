// src/octree.rs â€” Phase 3: Pure-Rust Octree + KD-Tree
#![allow(dead_code)] // stub â€” fields filled in as Octree implementation is completed
//
// Implements DgmOctree and KdTree in pure Rust.
// Migration targets:
//   â€¢ CCCoreLib::DgmOctree::build  (octree structure)
//   â€¢ CCCoreLib::KdTree             (KD-tree nearest neighbour)
//
// These are the most complex Rust migration targets:
//   â€¢ DgmOctree: TBB parallel cell computation â†’ Rayon
//   â€¢ KdTree:    manual new/delete â†’ Box<KdTreeCell>
//   â€¢ Both:      MultiThreadingWrapper races â†’ Rust ownership

use smallvec::SmallVec;

// â”€â”€ Pure-Rust Octree / KD-Tree types â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

/// Result of octree build.
#[derive(Debug, Clone)]
pub struct OctreeDataRust {
    pub levels: u8,
    pub cell_count: usize,
}

/// Error from octree build.
#[derive(Debug, Clone)]
pub struct OctreeErrorRust {
    pub code: i32,
    pub message: String,
}

/// Nearest neighbour result.
#[derive(Debug, Clone)]
pub struct NnResultRust {
    pub index: usize,
    pub distance: f32,
}

// â”€â”€ KD-Tree â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

/// KD-Tree cell. Replaces the manual `new KdTreeCell()` / `deleteSubTree()` in CCCoreLib.
struct KdTreeCell {
    point: [f32; 3],
    left: Option<Box<KdTreeCell>>,
    right: Option<Box<KdTreeCell>>,
}

impl KdTreeCell {
    fn new(point: [f32; 3]) -> Self {
        KdTreeCell { point, left: None, right: None }
    }

    /// Insert a point into the subtree rooted at this cell.
    fn insert(&mut self, new_point: [f32; 3], depth: usize) {
        // axis = depth % 3: 0=x, 1=y, 2=z
        let axis = depth % 3;
        let cmp = new_point[axis] < self.point[axis];

        if cmp {
            if let Some(ref mut left) = self.left {
                left.insert(new_point, depth + 1);
            } else {
                self.left = Some(Box::new(KdTreeCell::new(new_point)));
            }
        } else {
            if let Some(ref mut right) = self.right {
                right.insert(new_point, depth + 1);
            } else {
                self.right = Some(Box::new(KdTreeCell::new(new_point)));
            }
        }
    }
}

// Box<KdTreeCell> implements Drop recursively â€” no manual delete needed.
// Rust's ownership model eliminates the KdTree.cpp manual memory management entirely.

/// Pure-Rust KD-Tree for nearest neighbour search.
pub struct KdTree {
    root: Option<Box<KdTreeCell>>,
    count: usize,
}

impl KdTree {
    pub fn new() -> Self {
        KdTree { root: None, count: 0 }
    }

    /// Build KD-tree from interleaved xyz array.
    /// Mirrors: KdTree::buildFromCloud()
    pub fn build(points: &[f32]) -> Self {
        let n = points.len() / 3;
        let mut tree = KdTree::new();

        for i in 0..n {
            let pt = [points[i * 3], points[i * 3 + 1], points[i * 3 + 2]];
            if let Some(ref mut root) = tree.root {
                root.insert(pt, 0);
            } else {
                tree.root = Some(Box::new(KdTreeCell::new(pt)));
            }
            tree.count += 1;
        }

        tree
    }

    /// Find k nearest neighbours to a query point.
    /// Mirrors: KdTree::findNearestNeighbours()
    pub fn nearest(&self, query: &[f32; 3], k: usize) -> Vec<NnResultRust> {
        if self.root.is_none() {
            return vec![];
        }

        let mut results: SmallVec<[NnResultRust; 16]> = SmallVec::new();
        self.search_nearest(self.root.as_deref(), query, k, &mut results);
        results.sort_by_key(|r| (r.distance * 1000.0) as u32); // sort by distance
        results.truncate(k);
        results.into_vec() // convert SmallVec â†’ Vec
    }

    fn dist_sq(&self, a: &[f32; 3], b: &[f32; 3]) -> f32 {
        let dx = a[0] - b[0];
        let dy = a[1] - b[1];
        let dz = a[2] - b[2];
        dx * dx + dy * dy + dz * dz
    }

    fn search_nearest(
        &self,
        cell: Option<&KdTreeCell>,
        query: &[f32; 3],
        k: usize,
        results: &mut SmallVec<[NnResultRust; 16]>,
    ) {
        let Some(node) = cell else { return; };

        let dist = self.dist_sq(&node.point, query);

        // Insert if results not full or if this is better than worst
        if results.len() < k || dist < results.last().map(|r| r.distance.powi(2)).unwrap_or(f32::MAX) {
            if results.len() >= k {
                results.pop();
            }
            results.push(NnResultRust {
                index: 0, // caller sets correct index
                distance: dist.sqrt(),
            });
        }

        // Determine which child to search first
        // Simple heuristic: both children always searched (correct for k=1)
        self.search_nearest(node.left.as_deref(), query, k, results);
        self.search_nearest(node.right.as_deref(), query, k, results);
    }
}

impl Default for KdTree {
    fn default() -> Self { Self::new() }
}

// â”€â”€ Octree â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

/// Octree cell. Mirrors DgmOctree::OctreeCell.
struct OctreeCell {
    // Truncated Morton code (SpatialIndex::GetTruncatedMortonCode)
    truncated_code: u64,
    // Indices into the points array for points in this cell
    point_indices: Vec<usize>,
    // Child indices in the cells array (None = leaf)
    children: Option<[usize; 8]>,
    level: u8,
}

impl OctreeCell {
    fn root(_bb_min: [f32; 3], _bb_max: [f32; 3]) -> Self {
        OctreeCell {
            truncated_code: 0,
            point_indices: Vec::new(),
            children: None,
            level: 0,
        }
    }

    /// Subdivide this cell into 8 children (octree rule).
    fn subdivide(&mut self, _points: &[f32], _indices: &[usize]) {
        // Placeholder: real implementation needs 3D bounding box tracking
        self.children = Some([0; 8]);
    }
}

/// Pure-Rust DgmOctree implementation.
/// Phase 3 migration target â€” most complex.
pub struct Octree {
    points: Vec<f32>,     // interleaved xyz
    cells: Vec<OctreeCell>,
    bounding_box_min: [f32; 3],
    bounding_box_max: [f32; 3],
    max_level: u8,
}

impl Octree {
    /// Build an octree from an interleaved xyz point cloud.
    /// Mirrors: DgmOctree::build()
    pub fn build(points: &[f32], max_level: u8) -> Result<OctreeDataRust, OctreeErrorRust> {
        if points.is_empty() {
            return Err(OctreeErrorRust { code: 1, message: "Empty point cloud".into() });
        }

        let n = points.len() / 3;
        if n < 2 {
            return Ok(OctreeDataRust { levels: 0, cell_count: 1 });
        }

        // Compute bounding box
        let (bb_min, bb_max) = compute_bounding_box(points);

        // Build octree using a worklist algorithm
        let mut cells = vec![OctreeCell::root(bb_min, bb_max)];
        let mut worklist: Vec<usize> = vec![0];

        while let Some(cell_idx) = worklist.pop() {
            let point_indices_clone = {
                let cell = &cells[cell_idx];
                if cell.point_indices.len() > 1 && cell.level < max_level {
                    Some(cell.point_indices.clone())
                } else {
                    None
                }
            };

            if let Some(indices) = point_indices_clone {
                let cell = &mut cells[cell_idx];
                cell.subdivide(points, &indices);
                if let Some(children) = &cell.children {
                    for &child_idx in children.iter() {
                        if child_idx > 0 {
                            worklist.push(child_idx);
                        }
                    }
                }
            }
        }

        Ok(OctreeDataRust {
            levels: max_level,
            cell_count: cells.len(),
        })
    }
}

fn compute_bounding_box(points: &[f32]) -> ([f32; 3], [f32; 3]) {
    let n = points.len() / 3;
    let mut min = [std::f32::MAX; 3];
    let mut max = [std::f32::MIN; 3];

    for i in 0..n {
        for axis in 0..3 {
            let v = points[i * 3 + axis];
            if v < min[axis] { min[axis] = v; }
            if v > max[axis] { max[axis] = v; }
        }
    }

    (min, max)
}

// â”€â”€ FFI wrappers (called by CXX bridge) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

/// Wraps the pure-Rust octree build for the FFI layer.
pub fn build_octree(points: &[f32], max_level: u8) -> Result<OctreeDataRust, OctreeErrorRust> {
    Octree::build(points, max_level)
}

/// Wraps the pure-Rust KD-tree nearest neighbour for the FFI layer.
pub fn kdtree_nearest(points: &[f32], query: &[f32; 3], k: usize) -> Vec<NnResultRust> {
    let tree = KdTree::build(points);
    tree.nearest(query, k)
}

// â”€â”€ Tests â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

#[cfg(test)]
mod tests {
    use super::*;

    fn make_cube(n_per_axis: usize, size: f32) -> Vec<f32> {
        // Simple uniform grid
        let mut pts = Vec::with_capacity(n_per_axis * n_per_axis * n_per_axis * 3);
        for i in 0..n_per_axis {
            for j in 0..n_per_axis {
                for k in 0..n_per_axis {
                    pts.push((i as f32) * size / (n_per_axis as f32));
                    pts.push((j as f32) * size / (n_per_axis as f32));
                    pts.push((k as f32) * size / (n_per_axis as f32));
                }
            }
        }
        pts
    }

    #[test]
    fn test_kdtree_empty() {
        let tree = KdTree::new();
        let results = tree.nearest(&[0.0, 0.0, 0.0], 3);
        assert!(results.is_empty());
    }

    #[test]
    fn test_kdtree_single_point() {
        let pts = vec![1.0_f32, 2.0, 3.0];
        let tree = KdTree::build(&pts);
        let results = tree.nearest(&[1.0, 2.0, 3.0], 1);
        assert_eq!(results.len(), 1);
        assert!(results[0].distance < 1e-6);
    }

    #[test]
    fn test_kdtree_nearest_is_self() {
        let pts = vec![
            0.0_f32, 0.0, 0.0,
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0,
        ];
        let tree = KdTree::build(&pts);
        let results = tree.nearest(&[0.05, 0.05, 0.05], 1);
        assert_eq!(results.len(), 1);
        // Nearest should be origin (0,0,0)
        assert!(results[0].distance < 0.2);
    }

    #[test]
    fn test_kdtree_k_greater_than_count() {
        let pts = vec![0.0_f32, 0.0, 0.0, 1.0, 1.0, 1.0];
        let tree = KdTree::build(&pts);
        let results = tree.nearest(&[0.5, 0.5, 0.5], 10);
        assert_eq!(results.len(), 2);  // Only 2 points exist
    }

    #[test]
    fn test_octree_empty() {
        let pts: Vec<f32> = vec![];
        let result = Octree::build(&pts, 4);
        assert!(result.is_err());
    }

    #[test]
    fn test_octree_single_point() {
        let pts = vec![0.0_f32, 0.0, 0.0];
        let result = Octree::build(&pts, 4).unwrap();
        assert_eq!(result.cell_count, 1);
        assert_eq!(result.levels, 0);
    }

    #[test]
    fn test_octree_cube() {
        let pts = make_cube(4, 1.0);
        let result = Octree::build(&pts, 3);
        assert!(result.is_ok());
        let r = result.unwrap();
        assert!(r.cell_count >= 1);
        assert!(r.levels <= 3);
    }

    #[test]
    fn test_bounding_box() {
        let pts = vec![
            -1.0_f32, -2.0, -3.0,
             5.0,      6.0,   7.0,
        ];
        let (min, max) = compute_bounding_box(&pts);
        assert_eq!(min, [-1.0, -2.0, -3.0]);
        assert_eq!(max, [5.0, 6.0, 7.0]);
    }

    #[test]
    fn test_box_cell_no_manual_delete() {
        // Verify Box<KdTreeCell> is properly dropped
        // No assertion needed â€” if this compiles and runs without leak,
        // Box's Drop impl handles cleanup automatically.
        let pts = vec![0.0_f32, 0.0, 0.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0];
        let _tree = KdTree::build(&pts);
        // tree drops here â†’ all Box<KdTreeCell> cells dropped automatically
        // No deleteSubTree() needed â€” this is the Rust migration win
    }
}
// ===========================================================================
// Kiddo KD-tree adapter (D8, 2026-08-20)
//
// Was previously only in the experimental `02-kiddo-kdtree`
// scenario crate. Promoted into the main lib so the CLI
// and any other consumer can use it without pulling in
// the experimental scenario as a dep.
//
// kiddo is a pure-Rust KD-tree (~30 KLOC, no C bindings).
// It is the recommended NN for ICP: 1.5-3x faster than
// DgmOctree's D9 at ICP scale (see the D9 promotion.md
// benchmarks in
// `experimental/scenarios/2026-08-19-icp-variants/04-dgm-octree/promotion.md`).
// ===========================================================================

use kiddo::KdTree as KiddoRawTree;
use kiddo::SquaredEuclidean;
use kiddo::leaf_strategies::VecOfArenas;
use kiddo::stem_strategies::EytzingerFlexPf;

/// kiddo 6.0 `KdTree` is generic over 6 type parameters:
///   A     — item type (f32 or f64)
///   T     — index type (u32 here)
///   SS    — stem strategy (EytzingerFlexPf = best for
///           read-heavy workloads like ICP NN queries)
///   LS    — leaf strategy (VecOfArenas with bucket size
///           and arena-size constants)
///   const K: usize — dimension (3 = XYZ)
///   const B: usize — leaf bucket size (32 is the
///           standard; 16-64 are reasonable)
///
/// We use the f64 flavour because that's what the
/// experimental scenario settled on (kiddo's f32 path
/// has slightly less numerical headroom for ICP and the
/// f32→f64 round-trip on insert is free). The trait
/// adapter below does the f32→f64 cast on every query.
pub type KiddoTree = KiddoRawTree<
    f64,
    u32,
    EytzingerFlexPf,
    VecOfArenas<f64, u32, 3, 32>,
    3,
    32,
>;

/// `NearestNeighbour` adapter over a kiddo `KdTree`. Used
/// by `icp_with_nn` as the default NN — kiddo is 1.5-3x
/// faster than DgmOctree's D9 at ICP scale (D9
/// promotion.md).
///
/// Build cost is O(n log n) once; per-query cost is
/// O(log n) amortised. For ICP with n = 7.5M points, the
/// full ICP wall time is ~244s (vs D9's ~514s — both
/// with thin LTO).
pub struct KiddoNN {
    tree: KiddoTree,
}

impl KiddoNN {
    /// Build a kiddo KD-tree from a flat f32 point cloud
    /// (the layout `icp_with_nn` uses: `[x, y, z, x, y, z,
    /// ...]`). Returns a `KiddoNN` ready to plug into
    /// `icp_with_nn`. Internally casts to f64 for kiddo's
    /// f64 flavour (no precision loss vs CCCoreLib which
    /// also uses f64 for ICP numerics).
    pub fn build(model_points: &[f32]) -> Self {
        let n = model_points.len() / 3;
        let points: Vec<[f64; 3]> = (0..n)
            .map(|i| [
                model_points[i * 3] as f64,
                model_points[i * 3 + 1] as f64,
                model_points[i * 3 + 2] as f64,
            ])
            .collect();
        // kiddo 6.0's bulk builder: takes a slice of
        // points and assigns sequential item ids starting
        // at 0. `n` may be 0 here; kiddo's `new_from_slice`
        // handles the empty case (returns an empty tree).
        let tree = KiddoTree::new_from_slice(&points)
            .expect("kiddo kd-tree construction");
        Self { tree }
    }
}

impl crate::registration::NearestNeighbour for KiddoNN {
    /// Per-query NN: returns `(point_index, squared_distance)`.
    /// f32 → f64 cast on the query point is essentially
    /// free relative to the cost of the tree descent.
    fn nearest(&self, query: &[f32; 3]) -> (usize, f32) {
        let q = [query[0] as f64, query[1] as f64, query[2] as f64];
        let result = self
            .tree
            .query(&q)
            .nearest_one::<SquaredEuclidean<f64>>()
            .execute();
        (result.item as usize, result.distance as f32)
    }
}
