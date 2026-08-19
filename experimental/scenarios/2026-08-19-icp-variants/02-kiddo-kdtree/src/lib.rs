//! ICP variant 2: `kiddo` KD-tree nearest-neighbour.
//!
//! Uses the kiddo 6.0 API. The KD-tree is built once from the model
//! and reused for every NN query in the ICP loop. Per-query cost
//! is O(log n) average.
//!
//! The surrounding ICP algorithm (Horn SVD, trimmed ICP, etc.)
//! is delegated to `cc-rust` so the only thing this variant
//! changes is the NN data structure. As of 2026-08-19, the cc-rust
//! ICP signature doesn't yet accept a custom NN, so the wrapper
//! builds the KD-tree for timing and exercises the algorithm via
//! cc-rust's brute force. See D8 candidate in
//! `scenarios/2026-08-19-icp-variants/decisions.md` for the
//! refactor that would unlock end-to-end kiddo-driven ICP.

use cc_rust::registration as icp;
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
pub use icp::IcprParamsRust as IcpParams;

/// ICP result — re-export the corrected cc-rust type.
pub use icp::IcprResultRust as IcpResult;

/// Build a KD-tree from a point cloud. The item stored for each
/// point is its index in the input slice.
pub fn build_kdtree(points: &[[f64; 3]]) -> Tree {
    Tree::new_from_slice(points).expect("kd-tree construction")
}

/// Run an NN query and return (input_index, squared_distance).
pub fn nearest(tree: &Tree, query: &[f64; 3]) -> (usize, f64) {
    let result = tree
        .query(query)
        .nearest_one::<SquaredEuclidean<f64>>()
        .execute();
    (result.item as usize, result.distance)
}

/// ICP wrapper. Builds the kiddo KD-tree (for timing) and runs
/// the corrected ICP via cc-rust. To do an end-to-end kiddo-driven
/// ICP, cc-rust needs a `NearestNeighbour` trait — see D8.
pub fn icp_iterate(
    data_points: &mut [f32],
    model_points: &[f32],
    params: &IcpParams,
) -> Result<IcpResult, icp::IcprErrorRust> {
    let n_model = model_points.len() / 3;
    let model_arr: Vec<[f64; 3]> = (0..n_model)
        .map(|i| {
            [
                model_points[i * 3] as f64,
                model_points[i * 3 + 1] as f64,
                model_points[i * 3 + 2] as f64,
            ]
        })
        .collect();
    let _tree = build_kdtree(&model_arr);

    icp::icp_iterate(data_points, model_points, params)
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
        let tree = build_kdtree(&points);
        // Query closest to points[0].
        let (idx, d) = nearest(&tree, &[0.1, 0.0, 0.0]);
        assert_eq!(idx, 0, "nearest to (0.1,0,0) should be points[0]");
        assert!(d < 0.02, "got d² = {}", d);
        // Query closest to points[3].
        let (idx, d) = nearest(&tree, &[2.0, 3.0, 4.05]);
        assert_eq!(idx, 3);
        assert!(d < 0.02, "got d² = {}", d);
    }

    #[test]
    fn icp_via_cc_rust() {
        // The corrected ICP via cc-rust should converge on the
        // asymmetric-9 fixture (the canonical non-degenerate test
        // cloud; the pure 8-cube is degenerate per P14).
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
}
