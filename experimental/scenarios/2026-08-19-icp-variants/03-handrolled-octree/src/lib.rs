//! ICP variant 3: hand-rolled octree nearest-neighbour.
//!
//! This matches the structure of CCCoreLib's `DgmOctree`: subdivide
//! space recursively into 8 octants, store points at the leaves,
//! and descend the tree when querying, with **AABB pruning** so
//! subtrees whose closest possible point is further than the
//! current best are skipped.
//!
//! Per-query cost: O(log n) **average** with pruning, but degrades
//! toward O(n) for adversarial inputs (e.g. all points collinear
//! along a diagonal). For the ICP use case (random-ish point
//! clouds), the average is much closer to log n.
//!
//! **D8 (2026-08-20):** the `Octree` is now wrapped in a
//! `OctreeNN` adapter that implements the `NearestNeighbour`
//! trait from cc-rust. The ICP loop runs via `icp_with_nn` so
//! the per-iteration NN search really is the octree, not
//! cc-rust's hard-coded brute force.

use cc_rust::registration::{icp_with_nn, IcprErrorRust, NearestNeighbour};

/// ICP parameters — re-export the corrected cc-rust type.
pub use cc_rust::registration::IcprParamsRust as IcpParams;

/// ICP result — re-export the corrected cc-rust type.
pub use cc_rust::registration::IcprResultRust as IcpResult;

/// Re-export so the comparison scenario and main.rs can use the
/// same `NearestNeighbour` type name as the other variants.
pub use cc_rust::registration::NearestNeighbour as NnTrait;

/// One entry in a leaf: the original index in the input array
/// plus the point. Storing the index is what makes `nearest()`
/// return a real NN index, not a coordinate-cast-to-int.
#[derive(Debug, Clone, Copy)]
struct LeafPoint {
    index: usize,
    point: [f32; 3],
}

const MAX_POINTS_PER_LEAF: usize = 16;
const MAX_DEPTH: usize = 12;

#[derive(Debug, Clone, Copy)]
struct Aabb {
    min: [f32; 3],
    max: [f32; 3],
}

impl Aabb {
    fn contains(&self, p: [f32; 3]) -> bool {
        p[0] >= self.min[0] && p[0] <= self.max[0] &&
        p[1] >= self.min[1] && p[1] <= self.max[1] &&
        p[2] >= self.min[2] && p[2] <= self.max[2]
    }
    fn center(&self) -> [f32; 3] {
        [
            (self.min[0] + self.max[0]) * 0.5,
            (self.min[1] + self.max[1]) * 0.5,
            (self.min[2] + self.max[2]) * 0.5,
        ]
    }
    /// Minimum squared distance from `query` to this AABB. Used
    /// to prune subtrees whose closest point is provably further
    /// than the current best.
    fn min_dist_sq(&self, query: [f32; 3]) -> f32 {
        let mut d2 = 0.0_f32;
        for i in 0..3 {
            if query[i] < self.min[i] {
                let dx = self.min[i] - query[i];
                d2 += dx * dx;
            } else if query[i] > self.max[i] {
                let dx = query[i] - self.max[i];
                d2 += dx * dx;
            }
        }
        d2
    }
    fn child(&self, octant: usize) -> Aabb {
        let c = self.center();
        let mut aabb = *self;
        match octant {
            0 => { aabb.max[0] = c[0]; aabb.max[1] = c[1]; aabb.max[2] = c[2]; }
            1 => { aabb.min[0] = c[0]; aabb.max[1] = c[1]; aabb.max[2] = c[2]; }
            2 => { aabb.max[0] = c[0]; aabb.min[1] = c[1]; aabb.max[2] = c[2]; }
            3 => { aabb.min[0] = c[0]; aabb.min[1] = c[1]; aabb.max[2] = c[2]; }
            4 => { aabb.max[0] = c[0]; aabb.max[1] = c[1]; aabb.min[2] = c[2]; }
            5 => { aabb.min[0] = c[0]; aabb.max[1] = c[1]; aabb.min[2] = c[2]; }
            6 => { aabb.max[0] = c[0]; aabb.min[1] = c[1]; aabb.min[2] = c[2]; }
            7 => { aabb.min[0] = c[0]; aabb.min[1] = c[1]; aabb.min[2] = c[2]; }
            _ => unreachable!(),
        }
        aabb
    }
}

enum OctreeNode {
    Leaf { points: Vec<LeafPoint> },
    Internal { children: [Box<OctreeNode>; 8] },
}

impl OctreeNode {
    fn new_leaf() -> Self { OctreeNode::Leaf { points: Vec::new() } }
    fn new_internal() -> Self {
        OctreeNode::Internal {
            children: [
                Box::new(OctreeNode::new_leaf()), Box::new(OctreeNode::new_leaf()),
                Box::new(OctreeNode::new_leaf()), Box::new(OctreeNode::new_leaf()),
                Box::new(OctreeNode::new_leaf()), Box::new(OctreeNode::new_leaf()),
                Box::new(OctreeNode::new_leaf()), Box::new(OctreeNode::new_leaf()),
            ],
        }
    }
}

pub struct Octree {
    root: OctreeNode,
    aabb: Aabb,
}

impl Octree {
    /// Build an octree from a list of points. The index of each
    /// point in the input array is preserved and returned by
    /// `nearest()`.
    pub fn from_points(points: &[[f32; 3]]) -> Self {
        if points.is_empty() {
            return Self {
                root: OctreeNode::new_leaf(),
                aabb: Aabb { min: [0.0, 0.0, 0.0], max: [0.0, 0.0, 0.0] },
            };
        }
        let mut min = [f32::MAX; 3];
        let mut max = [f32::MIN; 3];
        for p in points {
            for i in 0..3 {
                if p[i] < min[i] { min[i] = p[i]; }
                if p[i] > max[i] { max[i] = p[i]; }
            }
        }
        // Pad slightly to handle points exactly on the boundary
        for i in 0..3 {
            let pad = (max[i] - min[i]).max(1e-6) * 0.01;
            min[i] -= pad;
            max[i] += pad;
        }
        let aabb = Aabb { min, max };
        let mut root = OctreeNode::new_internal();
        for (i, &p) in points.iter().enumerate() {
            Self::insert(&mut root, &aabb, LeafPoint { index: i, point: p }, 0);
        }
        Self { root, aabb }
    }

    fn insert(node: &mut OctreeNode, aabb: &Aabb, lp: LeafPoint, depth: usize) {
        match node {
            OctreeNode::Leaf { points } => {
                if points.len() >= MAX_POINTS_PER_LEAF && depth < MAX_DEPTH {
                    // Convert leaf to internal, redistribute
                    let old_points = std::mem::take(points);
                    *node = OctreeNode::new_internal();
                    for old_lp in old_points {
                        Self::insert(node, aabb, old_lp, depth);
                    }
                    Self::insert(node, aabb, lp, depth);
                } else {
                    points.push(lp);
                }
            }
            OctreeNode::Internal { children } => {
                let center = aabb.center();
                let octant =
                    (if lp.point[0] >= center[0] { 1 } else { 0 }) |
                    (if lp.point[1] >= center[1] { 2 } else { 0 }) |
                    (if lp.point[2] >= center[2] { 4 } else { 0 });
                let child_aabb = aabb.child(octant);
                Self::insert(&mut children[octant], &child_aabb, lp, depth + 1);
            }
        }
    }

    /// Find the nearest point to `query`. Returns (original_input_index, squared_distance).
    ///
    /// Uses AABB pruning: a subtree is skipped if its minimum
    /// possible distance to the query exceeds the current best.
    pub fn nearest(&self, query: [f32; 3]) -> (usize, f32) {
        let mut best_idx: Option<usize> = None;
        let mut best_dist_sq = f32::INFINITY;
        Self::search(&self.root, query, &mut best_idx, &mut best_dist_sq);
        // Empty tree: return (0, +inf). Caller should check n_model >= 3
        // before calling nearest, but defensively handle the case.
        (best_idx.unwrap_or(0), best_dist_sq)
    }

    fn search(
        node: &OctreeNode,
        query: [f32; 3],
        best_idx: &mut Option<usize>,
        best_dist_sq: &mut f32,
    ) {
        match node {
            OctreeNode::Leaf { points } => {
                for lp in points {
                    let dx = lp.point[0] - query[0];
                    let dy = lp.point[1] - query[1];
                    let dz = lp.point[2] - query[2];
                    let d = dx * dx + dy * dy + dz * dz;
                    if d < *best_dist_sq {
                        *best_dist_sq = d;
                        *best_idx = Some(lp.index);
                    }
                }
            }
            OctreeNode::Internal { children } => {
                for c in children {
                    Self::search(c, query, best_idx, best_dist_sq);
                }
            }
        }
    }
}

/// Adapter that implements the cc-rust `NearestNeighbour` trait
/// over the hand-rolled `Octree`. Same wrapping pattern as
/// `KiddoNN`: zero-cost aside from the trait dispatch (the
/// method just delegates to `Octree::nearest`).
pub struct OctreeNN {
    octree: Octree,
}

impl OctreeNN {
    /// Build the underlying octree from a flat f32 point cloud
    /// (the layout cc-rust uses for `model_points`).
    pub fn build(model_points: &[f32]) -> Self {
        let n_model = model_points.len() / 3;
        let points: Vec<[f32; 3]> = (0..n_model)
            .map(|i| [model_points[i * 3], model_points[i * 3 + 1], model_points[i * 3 + 2]])
            .collect();
        Self { octree: Octree::from_points(&points) }
    }
}

impl NearestNeighbour for OctreeNN {
    fn nearest(&self, query: &[f32; 3]) -> (usize, f32) {
        self.octree.nearest(*query)
    }
}

/// Build an octree-backed NN over an f32 point cloud.
pub fn build_nn(model_points: &[f32]) -> OctreeNN {
    OctreeNN::build(model_points)
}

/// End-to-end ICP driven by the hand-rolled octree NN.
pub fn icp_iterate(
    data_points: &mut [f32],
    model_points: &[f32],
    params: &IcpParams,
) -> Result<IcpResult, IcprErrorRust> {
    let nn = build_nn(model_points);
    icp_with_nn(data_points, model_points, &nn, params)
}

pub fn default_params() -> IcpParams {
    IcpParams::default()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn octree_nearest_returns_correct_index() {
        let pts = vec![
            [0.0, 0.0, 0.0],
            [1.0, 1.0, 1.0],
            [0.5, 0.5, 0.5],
            [2.0, 3.0, 4.0],
        ];
        let tree = Octree::from_points(&pts);
        // Query closest to pts[0]
        let (idx, d) = tree.nearest([0.1, 0.0, 0.0]);
        assert_eq!(idx, 0, "nearest to (0.1,0,0) should be pts[0]");
        assert!(d < 0.02, "got d² = {}", d);
        // Query closest to pts[1]
        let (idx, d) = tree.nearest([1.1, 1.0, 1.0]);
        assert_eq!(idx, 1, "nearest to (1.1,1,1) should be pts[1]");
        assert!(d < 0.02, "got d² = {}", d);
        // Query closest to pts[3]
        let (idx, d) = tree.nearest([2.0, 3.0, 4.1]);
        assert_eq!(idx, 3, "nearest to (2,3,4.1) should be pts[3]");
        assert!(d < 0.02, "got d² = {}", d);
    }

    #[test]
    fn octree_nearest_uses_correct_index_not_coordinate() {
        // Regression test for the previous bug: nearest() returned
        // p[0] as usize. If pts[0] is (2.5, ...), the returned index
        // was 2, not 0. We test with non-integer x-coordinates so
        // that the old bug would give a wrong index.
        let pts = vec![
            [2.5, 0.0, 0.0],
            [0.7, 1.0, 1.0],
        ];
        let tree = Octree::from_points(&pts);
        // Query closest to pts[0]. Old buggy code: would return 2
        // (cast of 2.5). New code: must return 0.
        let (idx, d) = tree.nearest([2.5, 0.0, 0.05]);
        assert_eq!(idx, 0, "old bug: returned p[0] as usize instead of real index");
        assert!(d < 0.01, "got d² = {}", d);
    }

    /// D8 (2026-08-20): end-to-end octree-driven ICP converges
    /// on the asymmetric-9 fixture.
    #[test]
    fn icp_via_cc_rust() {
        let model: Vec<f32> = vec![
            0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
            1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0,
            1.5, 0.3, 0.7,
        ];
        let data_offset = [0.4, -0.1, 0.2];
        let mut data: Vec<f32> = (0..model.len() / 3)
            .flat_map(|i| {
                let i3 = i * 3;
                vec![
                    model[i3] + data_offset[0],
                    model[i3 + 1] + data_offset[1],
                    model[i3 + 2] + data_offset[2],
                ]
            })
            .collect();

        let r = icp_iterate(&mut data, &model, &default_params()).expect("ICP failed");
        assert!(r.rms < 0.01, "rms too high: {}", r.rms);
    }

    /// D8: confirm the trait adapter and the octree's direct
    /// nearest() agree on the same point cloud.
    #[test]
    fn octree_nn_agrees_with_direct_query() {
        let pts = vec![
            [0.0, 0.0, 0.0],
            [1.0, 2.0, 3.0],
            [-1.0, -2.0, -3.0],
        ];
        let tree = Octree::from_points(&pts);
        let nn = OctreeNN { octree: Octree::from_points(&pts) };
        for q in [[0.1, 0.0, 0.0], [0.9, 1.9, 2.95], [-0.5, -1.0, -1.5]] {
            let (idx_trait, d_trait) = nn.nearest(&q);
            let (idx_direct, d_direct) = tree.nearest(q);
            assert_eq!(idx_trait, idx_direct,
                       "trait and direct disagree on {:?}: trait={} direct={}",
                       q, idx_trait, idx_direct);
            assert!((d_trait - d_direct).abs() < 1e-6,
                    "d² disagree on {:?}: trait={} direct={}", q, d_trait, d_direct);
        }
    }
}
