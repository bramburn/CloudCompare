//! ICP variant 1: naive O(n²) nearest-neighbour.
//!
//! This variant plugs the **brute-force** nearest-neighbour into
//! the canonical ICP algorithm from `cc-rust`. The point of the
//! scenario is to A/B the *NN data structure*, not the surrounding
//! algorithm. As of 2026-08-19, the algorithm in cc-rust has been
//! fixed (see D4 in experimental/docs/decisions.md and patterns
//! P12, P13, P14).
//!
//! Complexity: O(n × m) per ICP iteration, where n = data size,
//! m = model size. For 100k data points and 100k model points,
//! this is 10^10 ops per iteration — completely infeasible for
//! real data. The whole reason this scenario exists is to see
//! whether the KD-tree / octree variants can do better in practice.

use cc_rust::registration as icp;

/// ICP parameters — re-export the corrected cc-rust type.
pub use icp::IcprParamsRust as IcpParams;

/// ICP result — re-export the corrected cc-rust type.
pub use icp::IcprResultRust as IcpResult;

/// Run ICP with the brute-force NN plugged in.
///
/// Thin wrapper around `cc_rust::registration::icp_iterate` that
/// exists for two reasons:
/// 1. The scenario's main.rs can import `icp_v1_naive::icp_iterate`
///    without depending on `cc_rust` directly.
/// 2. Future variants in the same scenario (e.g. 02-kiddo-kdtree)
///    can use the same wrapper to swap in their own NN impl.
pub fn icp_iterate(
    data_points: &mut [f32],
    model_points: &[f32],
    params: &IcpParams,
) -> Result<IcpResult, icp::IcprErrorRust> {
    icp::icp_iterate(data_points, model_points, params)
}

/// Default parameters for ICP iteration.
///
/// Mirrors `IcpParams::default()` from cc-rust.
pub fn default_params() -> IcpParams {
    IcpParams::default()
}

#[cfg(test)]
mod tests {
    use super::*;

    /// ICP on the asymmetric-9 fixture (the canonical non-degenerate
    /// test cloud) recovers a known translation.
    #[test]
    fn translation_only_asymmetric_9() {
        // 8 cube corners + 1 off-axis point at (1.5, 0.3, 0.7).
        let mut model: Vec<f32> = vec![
            0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
            1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0,
            1.5, 0.3, 0.7,
        ];
        let data_offset = [0.5, -0.2, 0.1];
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

        let result = icp_iterate(&mut data, &model, &default_params()).expect("ICP failed");
        assert!(result.converged, "ICP should converge on the asymmetric-9 cloud");
        assert!(result.rms < 0.01, "rms too high: {}", result.rms);
    }
}
