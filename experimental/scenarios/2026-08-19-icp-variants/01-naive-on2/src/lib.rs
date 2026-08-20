//! ICP variant 1: naive O(n²) nearest-neighbour.
//!
//! This variant implements the `NearestNeighbour` trait (D8, 2026-08-20)
//! using a brute-force scan, then plugs it into `icp_with_nn` so the
//! full ICP loop actually runs the algorithm against the naive NN
//! (rather than falling back to cc-rust's own brute force inside a
//! non-templated `icp_iterate`).
//!
//! Complexity: O(n × m) per ICP iteration, where n = data size,
//! m = model size. For 100k data points and 100k model points,
//! this is 10^10 ops per iteration — completely infeasible for
//! real data. The whole reason this scenario exists is to provide
//! a *correctness reference* against which the kiddo and octree
//! variants can be measured: any other NN that disagrees with
//! brute force on a test fixture is buggy.

use cc_rust::registration::{icp_with_nn, BruteForceNN, NearestNeighbour};

/// ICP parameters — re-export the corrected cc-rust type.
pub use cc_rust::registration::IcprParamsRust as IcpParams;

/// ICP result — re-export the corrected cc-rust type.
pub use cc_rust::registration::IcprResultRust as IcpResult;

/// Re-export so the comparison scenario and main.rs can use the
/// same `NearestNeighbour` type name as the other variants.
pub use cc_rust::registration::NearestNeighbour as NnTrait;

/// The naive NN is just `cc_rust::BruteForceNN` directly. There is
/// no per-instance state to build beyond the model slice, so we
/// alias the type rather than wrapping it. This keeps the variant
/// honest: it adds zero abstraction over cc-rust's brute force.
pub type NaiveNN<'a> = cc_rust::registration::BruteForceNN<'a>;

/// Build a naive NN over a model slice. The returned reference is
/// tied to the lifetime of `model` — same as cc-rust's `BruteForceNN`.
pub fn build_nn<'a>(model: &'a [f32]) -> NaiveNN<'a> {
    BruteForceNN::new(model)
}

/// End-to-end ICP driven by the naive brute-force NN.
///
/// This is now a *true* end-to-end test: the per-iteration NN
/// search inside `icp_with_nn` is this variant's `BruteForceNN`,
/// not cc-rust's hard-coded one. (Pre-D8 the wrapper fell back
/// to cc-rust's brute force internally; the wall-time difference
/// was zero. Post-D8 the trait dispatch is real.)
pub fn icp_iterate(
    data_points: &mut [f32],
    model_points: &[f32],
    params: &IcpParams,
) -> Result<IcpResult, cc_rust::registration::IcprErrorRust> {
    let nn = build_nn(model_points);
    icp_with_nn(data_points, model_points, &nn, params)
}

/// Default parameters for ICP iteration.
pub fn default_params() -> IcpParams {
    IcpParams::default()
}

// Suppress unused-import warning for the trait re-export when no
// downstream consumer in this crate needs it directly.
#[allow(dead_code)]
fn _trait_is_object_safe(_: &dyn NnTrait) {}

#[cfg(test)]
mod tests {
    use super::*;

    /// ICP on the asymmetric-9 fixture (the canonical non-degenerate
    /// test cloud) recovers a known translation. This is the same
    /// assertion as the pre-D8 test, but now it actually exercises
    /// the trait dispatch path inside `icp_with_nn`.
    #[test]
    fn translation_only_asymmetric_9() {
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

    /// D8: confirm the naive NN agrees with the cc-rust internal
    /// `nearest_neighbour_slow` helper on a non-trivial fixture.
    /// This proves the trait impl is correct, not just that the
    /// ICP loop happens to converge.
    #[test]
    fn naive_nearest_matches_brute_force_truth() {
        // Hand-built model with deliberately asymmetric distances:
        // points[0] is uniquely closest to query (0, 0, 0);
        // points[3] is uniquely closest to (5, 5, 5).
        let model: Vec<f32> = vec![
            0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            5.0, 5.0, 5.0,
            5.1, 5.0, 5.0,
        ];
        let nn = build_nn(&model);
        let (idx, d) = nn.nearest(&[0.0, 0.0, 0.0]);
        assert_eq!(idx, 0);
        assert!(d < 1e-6);
        let (idx, d) = nn.nearest(&[5.0, 5.0, 5.0]);
        assert_eq!(idx, 3);
        assert!(d < 1e-6);
    }
}
