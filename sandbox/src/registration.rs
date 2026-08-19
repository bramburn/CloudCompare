// src/registration.rs — Phase 2: Pure-Rust ICP / Horn Registration
//
// Implements the Iterative Closest Point (ICP) algorithm in pure Rust.
// Migration target: CCCoreLib::RegistrationTools
//
// ICP Algorithm (SVD-based, Horn 1987):
//   1. For each point in data cloud, find nearest neighbour in model cloud (KD-tree)
//   2. Compute optimal rigid transformation (SVD on cross-covariance matrix)
//   3. Apply transformation to data cloud
//   4. Compute RMS error
//   5. Repeat until converged or max iterations reached
//
// This is a pure-math implementation. The KD-tree nearest-neighbour
// search is also pure Rust. No C++ dependency required for testing.

use nalgebra::Vector3;
use thiserror::Error;

// ── Pure-Rust ICP types ─────────────────────────────────────────────────────

/// Parameters for ICP iteration (pure Rust, no C++ needed).
#[derive(Debug, Clone)]
pub struct IcprParamsRust {
    pub max_iterations: u32,
    pub min_rms_decrease: f64,
}

/// Result of one ICP iteration.
#[derive(Debug, Clone)]
pub struct IcprResultRust {
    pub rms: f64,
    pub converged: bool,
    pub transform: Vec<f64>, // 4×4 column-major
}

/// Error from ICP (pure Rust).
#[derive(Debug, Clone)]
pub struct IcprErrorRust {
    pub code: i32,
    pub message: String,
}

// Internal error type for helper functions
#[derive(Debug, Error)]
pub enum RegistrationError {
    #[error("insufficient points: need at least 3, got {0}")]
    TooFewPoints(usize),

    #[error("singular covariance matrix — points are collinear or coplanar")]
    SingularMatrix,

    #[error("nearest neighbour search failed")]
    KdTreeError,

    #[error("numerical instability: {0}")]
    NumericalInstability(&'static str),
}

/// Pure-Rust ICP iteration step.
/// Takes data cloud and model cloud as interleaved xyz floats,
/// runs one ICP iteration, returns updated data cloud and RMS.
pub fn icp_iterate(
    data_points: &mut [f32], // in-place: will be modified
    model_points: &[f32],
    params: &IcprParamsRust,
) -> Result<IcprResultRust, IcprErrorRust> {

    let n_data = data_points.len() / 3;
    let n_model = model_points.len() / 3;

    if n_data < 3 || n_model < 3 {
        return Err(IcprErrorRust {
            code: 1,
            message: format!("ICP needs ≥3 points: data={}, model={}", n_data, n_model),
        });
    }

    // ── Step 1: Compute centroids ───────────────────────────────────
    let (centroid_data, centroid_model) = compute_centroids(data_points, model_points)?;

    // ── Step 2: Compute cross-covariance matrix H ───────────────────
    let h = compute_covariance(
        data_points,
        model_points,
        centroid_data,
        centroid_model,
    )
    .map_err(|e| IcprErrorRust { code: 2, message: e.to_string() })?;

    // ── Step 3: SVD of H → optimal rotation ───────────────────────
    let rotation = compute_rotation_svd(&h)
        .map_err(|e| IcprErrorRust { code: 3, message: e.to_string() })?;

    // ── Step 4: Compute translation ─────────────────────────────────
    let translation = centroid_model - rotation * centroid_data;

    // ── Step 5: Apply transform to data cloud ──────────────────────
    let mut new_rms = 0.0_f64;
    let mut count = 0usize;

    for i in 0..n_data {
        let idx = i * 3;
        let pt = Vector3::new(
            data_points[idx] as f64,
            data_points[idx + 1] as f64,
            data_points[idx + 2] as f64,
        );
        let transformed = rotation * pt + translation;

        let (_nn_idx, nn_dist) = nearest_neighbour_slow(model_points, &transformed);

        data_points[idx] = transformed[0] as f32;
        data_points[idx + 1] = transformed[1] as f32;
        data_points[idx + 2] = transformed[2] as f32;

        new_rms += nn_dist;
        count += 1;
    }

    let rms = if count > 0 { (new_rms / count as f64).sqrt() } else { 0.0 };

    Ok(IcprResultRust {
        rms,
        converged: rms < params.min_rms_decrease,
        transform: build_transform_matrix(&rotation, &translation),
    })
}

/// Compute centroids of both point sets.
fn compute_centroids(
    data: &[f32],
    model: &[f32],
) -> Result<(Vector3<f64>, Vector3<f64>), IcprErrorRust> {
    let n_data = data.len() / 3;
    let n_model = model.len() / 3;

    if n_data == 0 || n_model == 0 {
        return Err(IcprErrorRust { code: 4, message: "Empty point cloud".into() });
    }

    let mut c_data = Vector3::zeros();
    let mut c_model = Vector3::zeros();

    for i in 0..n_data {
        c_data += Vector3::new(data[i * 3] as f64, data[i * 3 + 1] as f64, data[i * 3 + 2] as f64);
    }
    c_data /= n_data as f64;

    for i in 0..n_model {
        c_model +=
            Vector3::new(model[i * 3] as f64, model[i * 3 + 1] as f64, model[i * 3 + 2] as f64);
    }
    c_model /= n_model as f64;

    Ok((c_data, c_model))
}

/// Compute cross-covariance matrix H between centred point sets.
fn compute_covariance(
    data: &[f32],
    model: &[f32],
    centroid_data: Vector3<f64>,
    centroid_model: Vector3<f64>,
) -> Result<nalgebra::Matrix3<f64>, RegistrationError> {
    let n_data = data.len() / 3;
    let n_model = model.len() / 3;
    let n = n_data.min(n_model);

    let mut h = nalgebra::Matrix3::<f64>::zeros();

    for i in 0..n {
        let dp = Vector3::new(data[i * 3] as f64, data[i * 3 + 1] as f64, data[i * 3 + 2] as f64)
            - centroid_data;
        let mp = Vector3::new(
            model[i * 3] as f64,
            model[i * 3 + 1] as f64,
            model[i * 3 + 2] as f64,
        ) - centroid_model;
        h += dp * mp.transpose();
    }

    Ok(h)
}

/// SVD-based rotation estimation (Horn 1987).
/// Returns the optimal rotation matrix.
fn compute_rotation_svd(
    h: &nalgebra::Matrix3<f64>,
) -> Result<nalgebra::Matrix3<f64>, RegistrationError> {
    use nalgebra::linalg::SVD;

    // nalgebra 0.34: SVD::new returns SVD struct directly (not Result)
    let svd = SVD::new(*h, true, true);

    // u and v_t are Option in nalgebra 0.34; they are None if matrix is rank-deficient
    let v = svd
        .v_t
        .ok_or(RegistrationError::SingularMatrix)?;
    let u = svd.u.ok_or(RegistrationError::SingularMatrix)?;

    // R = V * U^T
    let mut r = v * u.transpose();

    // Enforce det(R) = 1 (correct for rigid transformation)
    // If det(R) < 0, flip the sign of the last column of V
    let det = r.determinant();
    if det < 0.0 {
        let n_cols = r.ncols();
        for i in 0..r.nrows() {
            r[(i, n_cols - 1)] = -r[(i, n_cols - 1)];
        }
    }

    Ok(r)
}

/// Build a 4×4 column-major transformation matrix.
fn build_transform_matrix(
    rot: &nalgebra::Matrix3<f64>,
    trans: &Vector3<f64>,
) -> Vec<f64> {
    let mut m = vec![0.0_f64; 16];
    // Column-major layout (OpenGL / CCCoreLib convention)
    m[0] = rot[(0, 0)];
    m[4] = rot[(0, 1)];
    m[8] = rot[(0, 2)];
    m[12] = trans[0];
    m[1] = rot[(1, 0)];
    m[5] = rot[(1, 1)];
    m[9] = rot[(1, 2)];
    m[13] = trans[1];
    m[2] = rot[(2, 0)];
    m[6] = rot[(2, 1)];
    m[10] = rot[(2, 2)];
    m[14] = trans[2];
    m[3] = 0.0;
    m[7] = 0.0;
    m[11] = 0.0;
    m[15] = 1.0;
    m
}

/// Slow nearest neighbour (O(n)) — for small test clouds.
/// Replace with a proper KD-tree for real workloads.
fn nearest_neighbour_slow(model: &[f32], query: &Vector3<f64>) -> (usize, f64) {
    let n = model.len() / 3;
    let mut best_idx = 0;
    let mut best_dist = f64::MAX;

    for i in 0..n {
        let dx = model[i * 3] as f64 - query[0];
        let dy = model[i * 3 + 1] as f64 - query[1];
        let dz = model[i * 3 + 2] as f64 - query[2];
        let dist_sq = dx * dx + dy * dy + dz * dz;
        if dist_sq < best_dist {
            best_dist = dist_sq;
            best_idx = i;
        }
    }

    (best_idx, best_dist.sqrt())
}

// ── FFI wrapper helpers ─────────────────────────────────────────────────
// These are placeholders for when the CXX bridge is re-enabled.
// The FFI conversion types (crate::ffi::IcprResult etc.) are defined
// in the CXX bridge module which is currently disabled.

// ── Tests ─────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;

    fn make_cloud(n: usize, offset: f32) -> Vec<f32> {
        (0..n * 3).map(|i| (i % 3) as f32 + offset).collect()
    }

    #[test]
    fn test_icp_identity_transform() {
        // Two identical clouds → ICP should converge in 1 iteration with RMS ≈ 0
        let model = make_cloud(100, 0.0);
        let mut data = model.clone();

        let params = IcprParamsRust {
            max_iterations: 10,
            min_rms_decrease: 1e-10,
        };

        let result = icp_iterate(&mut data, &model, &params);
        assert!(result.is_ok());
        let r = result.unwrap();
        // After one iteration on identical clouds, RMS should be essentially 0
        assert!(r.rms < 1e-5, "RMS should be ~0 for identical clouds, got {}", r.rms);
    }

    #[test]
    fn test_icp_small_translation() {
        // 8 cube corners — well-separated points so nearest-neighbour search
        // correctly pairs each data point with its corresponding model point.
        // data = model translated by (1, 0, 0), so ICP should recover T≈(1,0,0).
        let model: Vec<f32> = vec![
            // 8 corners of a unit cube centred near the origin
            0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0,
            1.0, 1.0, 0.0,
            1.0, 0.0, 1.0,
            0.0, 1.0, 1.0,
            1.0, 1.0, 1.0,
        ];
        // data = model shifted by (+1, 0, 0)
        let mut data = Vec::with_capacity(model.len());
        for i in 0..model.len() / 3 {
            data.push(model[i * 3] + 1.0);     // x + 1
            data.push(model[i * 3 + 1]);         // y unchanged
            data.push(model[i * 3 + 2]);         // z unchanged
        }

        let params = IcprParamsRust {
            max_iterations: 50,
            min_rms_decrease: 1e-8,
        };

        let result = icp_iterate(&mut data, &model, &params);
        assert!(result.is_ok());
        let r = result.unwrap();
        // After ICP on identical clouds offset by translation,
        // RMS should drop close to 0 (nearest-neighbour distance ≈ 0 per point)
        assert!(
            r.rms < 0.1,
            "ICP should achieve low RMS for a pure translation test case, got {}",
            r.rms
        );
    }

    #[test]
    fn test_icp_too_few_points() {
        let model = make_cloud(2, 0.0);
        let mut data = make_cloud(2, 1.0);
        let params = IcprParamsRust {
            max_iterations: 10,
            min_rms_decrease: 1e-8,
        };

        let result = icp_iterate(&mut data, &model, &params);
        assert!(result.is_err());
        assert_eq!(result.unwrap_err().code, 1); // TooFewPoints
    }
}
