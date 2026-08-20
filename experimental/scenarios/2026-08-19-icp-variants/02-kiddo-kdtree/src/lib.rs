//! ICP variant 2: `kiddo` KD-tree nearest-neighbour.
//!
//! Uses the kiddo 6.0 API. The KD-tree is built once from the model
//! and reused for every NN query in the ICP loop. Per-query cost
//! is O(log n) average.
//!
//! **D8 (2026-08-20):** the kiddo `KdTree` is now wrapped in a
//! `KiddoNN` adapter that implements the `NearestNeighbour` trait
//! from cc-rust, and the ICP loop runs via `icp_with_nn`. Before
//! D8, the variant's wrapper built the tree for timing and then
//! fell back to cc-rust's brute force for the actual ICP
//! iteration. Now the ICP iteration is genuinely tree-driven.

use cc_rust::registration::{icp_with_nn, IcprErrorRust, NearestNeighbour};
use kiddo::KdTree;
use kiddo::SquaredEuclidean;
use kiddo::leaf_strategies::VecOfArenas;
use kiddo::stem_strategies::EytzingerFlexPf;

/// KD-tree type alias. kiddo 6.0 has 6 generic parameters; the
/// alias keeps them out of the way of the rest of the code.
type Tree = KdTree<
    f64,
    u32,
    EytzingerFlexPf,
    VecOfArenas<f64, u32, 3, 32>,
    3,
    32,
>;

/// ICP parameters — re-export the corrected cc-rust type.
pub use cc_rust::registration::IcprParamsRust as IcpParams;

/// ICP result — re-export the corrected cc-rust type.
pub use cc_rust::registration::IcprResultRust as IcpResult;

/// Re-export so the comparison scenario and main.rs can use the
/// same `NearestNeighbour` type name as the other variants.
pub use cc_rust::registration::NearestNeighbour as NnTrait;

/// Adapter that implements the cc-rust `NearestNeighbour` trait
/// over a kiddo `KdTree`.
///
/// `kiddo` works in `f64` internally and stores `u32` item ids.
/// The trait expects `f32` queries and `usize` indices. This
/// adapter does the cast on every call; the cast is essentially
/// free relative to the cost of the tree descent.
pub struct KiddoNN {
    tree: Tree,
}

impl KiddoNN {
    /// Build a kiddo KD-tree over a list of model points and
    /// return a struct that implements the `NearestNeighbour`
    /// trait for use with `icp_with_nn`.
    ///
    /// The caller is responsible for keeping the original model
    /// slice alive if they need the point coordinates later —
    /// the tree only stores indices.
    pub fn build(points: &[[f64; 3]]) -> Self {
        let tree = Tree::new_from_slice(points).expect("kd-tree construction");
        Self { tree }
    }
}

impl NearestNeighbour for KiddoNN {
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

/// Build a kiddo KD-tree from an f32 point cloud (in the
/// `[x, y, z, x, y, z, ...]` flat layout cc-rust uses for
/// `model_points`). Returns a `KiddoNN` ready to plug into
/// `icp_with_nn`.
pub fn build_nn(model_points: &[f32]) -> KiddoNN {
    let n_model = model_points.len() / 3;
    let points: Vec<[f64; 3]> = (0..n_model)
        .map(|i| {
            [
                model_points[i * 3] as f64,
                model_points[i * 3 + 1] as f64,
                model_points[i * 3 + 2] as f64,
            ]
        })
        .collect();
    KiddoNN::build(&points)
}

/// Backwards-compatible alias: the previous name used by main.rs
/// and the bench binary. Builds a `Tree` directly, which is what
/// the standalone NN-query benchmark needs (it does not go
/// through the trait).
pub fn build_kdtree(points: &[[f64; 3]]) -> Tree {
    Tree::new_from_slice(points).expect("kd-tree construction")
}

/// End-to-end ICP driven by the kiddo KD-tree NN.
pub fn icp_iterate(
    data_points: &mut [f32],
    model_points: &[f32],
    params: &IcpParams,
) -> Result<IcpResult, IcprErrorRust> {
    let nn = build_nn(model_points);
    icp_with_nn(data_points, model_points, &nn, params)
}

/// Direct NN query (kept for the main.rs benchmark; not used
/// inside `icp_with_nn`, which dispatches via the trait).
pub fn nearest(tree: &Tree, query: &[f64; 3]) -> (usize, f64) {
    let result = tree
        .query(query)
        .nearest_one::<SquaredEuclidean<f64>>()
        .execute();
    (result.item as usize, result.distance)
}

pub fn default_params() -> IcpParams {
    IcpParams::default()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn kdtree_nearest_returns_correct_index() {
        let points: Vec<[f64; 3]> = vec![
            [0.0, 0.0, 0.0],
            [1.0, 1.0, 1.0],
            [0.5, 0.5, 0.5],
            [2.0, 3.0, 4.0],
        ];
        let nn = KiddoNN::build(&points);
        // Query closest to points[0].
        let (idx, d) = nn.nearest(&[0.1, 0.0, 0.0]);
        assert_eq!(idx, 0, "nearest to (0.1,0,0) should be points[0]");
        assert!(d < 0.02, "got d² = {}", d);
        // Query closest to points[3].
        let (idx, d) = nn.nearest(&[2.0, 3.0, 4.05]);
        assert_eq!(idx, 3);
        assert!(d < 0.02, "got d² = {}", d);
    }

    /// D8 (2026-08-20): end-to-end kiddo-driven ICP converges
    /// on the asymmetric-9 fixture. Before D8 this was a
    /// brute-force ICP in disguise; now the per-iteration NN
    /// search really is the kiddo KD-tree.
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

    /// D8: confirm that the trait adapter and the kiddo direct
    /// query agree on the same point cloud. This is the round-trip
    /// test that proves the adapter is not silently corrupting
    /// the cast.
    #[test]
    fn kiddo_nn_agrees_with_direct_query() {
        let points: Vec<[f64; 3]> = vec![
            [0.0, 0.0, 0.0],
            [1.0, 2.0, 3.0],
            [-1.0, -2.0, -3.0],
        ];
        let tree = KdTree::new_from_slice(&points).expect("kd-tree construction");
        let nn = KiddoNN::build(&points);
        for q_f64 in [[0.1, 0.0, 0.0], [0.9, 1.9, 2.95], [-0.5, -1.0, -1.5]] {
            let q_f32 = [q_f64[0] as f32, q_f64[1] as f32, q_f64[2] as f32];
            let (idx_trait, _d_trait) = nn.nearest(&q_f32);
            let (idx_direct, _d_direct) = nearest(&tree, &q_f64);
            assert_eq!(idx_trait, idx_direct,
                       "trait and direct disagree on {:?}: trait={} direct={}",
                       q_f64, idx_trait, idx_direct);
        }
    }
}
