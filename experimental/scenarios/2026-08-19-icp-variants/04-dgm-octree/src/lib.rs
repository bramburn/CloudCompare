//! ICP variant 4: `DgmOctree` cell-code-ordered nearest-neighbour
//! (D9 in `experimental/docs/decisions.md`).
//!
//! The octree is built once from the model and reused for every NN
//! query in the ICP loop. Per-query cost is `O(d^3)` cells visited
//! in the worst case (where d is the Chebyshev distance to the
//! NN's cell), times the AABB check. For typical ICP use (query at
//! a known model point, NN in a nearby cell), d ≤ 2 and the cost
//! is O(1) to O(27) cells.
//!
//! **D9 (2026-08-20):** the cell-code-ordered search has:
//! - Pre-sorted indices and cell→range map cached at `build()` time
//!   (no per-query sort).
//! - O(1) cell lookup via `HashMap<u64, (start, end)>`.
//! - Chebyshev-distance shell expansion with AABB pruning.
//! - C++-style jump optimisation for queries outside the bbox.
//! - Correct `minDistToBorder` accounting so the early-termination
//!   check doesn't miss cells adjacent to the query's cell face.

use cc_rust::dgm_octree::DgmOctree;
use cc_rust::registration::{icp_with_nn, IcprErrorRust, NearestNeighbour};

/// ICP parameters — re-export the corrected cc-rust type.
pub use cc_rust::registration::IcprParamsRust as IcpParams;

/// ICP result — re-export the corrected cc-rust type.
pub use cc_rust::registration::IcprResultRust as IcpResult;

/// Re-export so the comparison scenario and main.rs can use the
/// same `NearestNeighbour` type name as the other variants.
pub use cc_rust::registration::NearestNeighbour as NnTrait;

/// Re-export the D9 octree type so main.rs can call into it
/// directly for the standalone NN-query benchmark.
pub use cc_rust::dgm_octree::DgmOctreeNN;

/// End-to-end ICP driven by the DgmOctree cell-code NN.
pub fn icp_iterate(
    data_points: &mut [f32],
    model_points: &[f32],
    params: &IcpParams,
) -> Result<IcpResult, IcprErrorRust> {
    let nn = DgmOctreeNN::build(model_points);
    icp_with_nn(data_points, model_points, &nn, params)
}

pub fn default_params() -> IcpParams {
    IcpParams::default()
}

/// Build a `DgmOctreeNN` from an f32 point cloud (in the
/// `[x, y, z, x, y, z, ...]` flat layout cc-rust uses for
/// `model_points`). Returns a `DgmOctreeNN` ready to plug into
/// `icp_with_nn`.
pub fn build_nn(model_points: &[f32]) -> DgmOctreeNN {
    DgmOctreeNN::build(model_points)
}

/// Direct NN query on a `DgmOctree`. Kept for main.rs's standalone
/// benchmark; the ICP loop uses `icp_with_nn` via the trait
/// instead, so the per-iteration NN search is genuinely
/// cell-code-driven.
pub fn nearest(tree: &DgmOctree, query: [f32; 3]) -> (usize, f32) {
    tree.nearest_neighbor(query)
}

#[cfg(test)]
mod tests {
    use super::*;

    /// D9: end-to-end DgmOctree-driven ICP converges on the
    /// asymmetric-9 fixture.
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

    /// D9: build the DgmOctreeNN from an f32 cloud and confirm
    /// the trait dispatch works end-to-end (compile-time check).
    #[test]
    fn dgm_octree_nn_built_from_f32() {
        let model: Vec<f32> = vec![
            0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
            1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        ];
        let nn = build_nn(&model);
        let (idx, d) = nn.nearest(&[0.1, 0.0, 0.0]);
        assert_eq!(idx, 0, "expected points[0], got {}", idx);
        assert!(d < 0.02, "d^2 too high: {}", d);
    }
}
