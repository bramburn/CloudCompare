// src/registration.rs — Phase 2: Pure-Rust ICP / Horn Registration
//
// Implements the Iterative Closest Point (ICP) algorithm in pure Rust.
// Migration target: CCCoreLib::RegistrationTools
//
// ICP Algorithm (corrected 2026-08-19):
//   rotation = I, translation = 0
//   repeat:
//     (1) find correspondences: for each data point, find nearest neighbour in model
//     (2) compute RMS = sqrt(mean of squared NN distances)
//     (3) convergence check: |prev_rms - rms| < min_rms_decrease
//     (4) compute optimal (R, t) from the matched pairs via Horn 1987 SVD
//     (5) apply cumulative (R, t) to data in place
//   until converged or max_iterations
//   return cumulative (R, t)
//
// This is a pure-math implementation. The nearest-neighbour search is
// also pure Rust. No C++ dependency required for testing.

use nalgebra::Vector3;
use thiserror::Error;

// ── Pure-Rust ICP types ─────────────────────────────────────────────────────

/// Parameters for ICP iteration (pure Rust, no C++ needed).
#[derive(Debug, Clone)]
pub struct IcprParamsRust {
    pub max_iterations: u32,
    /// Stop when |prev_rms − rms| < this. This is the "no more progress"
    /// criterion. See D4 in `experimental/docs/decisions.md` for history.
    pub min_rms_decrease: f64,
    /// Stop when RMS drops below this absolute value. Without this, ICP
    /// can keep iterating even after the data is at the model (RMS ≈ 0),
    /// because the H matrix is then the model's covariance, which is
    /// often rank-deficient and produces a reflection instead of the
    /// identity. The next "improvement" then moves the data *away* from
    /// the model. Default 1e-3.
    pub min_rms_absolute: f64,
}

impl Default for IcprParamsRust {
    fn default() -> Self {
        Self {
            max_iterations: 50,
            min_rms_decrease: 1e-6,
            min_rms_absolute: 1e-3,
        }
    }
}

/// Result of ICP.
#[derive(Debug, Clone)]
pub struct IcprResultRust {
    pub rms: f64,
    pub converged: bool,
    pub iterations: u32,
    /// Cumulative 4×4 column-major transformation matrix.
    pub transform: Vec<f64>,
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

/// Run ICP using O(n²) brute-force nearest-neighbour.
///
/// The standard ICP algorithm:
///   1. Find correspondences (data point ↔ nearest model point)
///   2. Compute optimal (R, t) from those correspondences (Horn 1987 SVD)
///   3. Apply (R, t) to data
///   4. Repeat from 1 until RMS change is small
///
/// **Corrected 2026-08-19**: the previous version computed (R, t) once
/// before the loop and only iterated the NN search. That is **not** ICP.
/// See `experimental/docs/decisions.md` D4 for the bug history.
///
/// `data_points` is mutated in place; on return, it is the registered cloud.
/// The returned `transform` is the cumulative 4×4 matrix that was applied
/// to map the input data cloud to its final pose.
pub fn icp_iterate(
    data_points: &mut [f32],
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

    // The running (R, t) starts at identity.
    let mut rotation = nalgebra::Matrix3::<f64>::identity();
    let mut translation = Vector3::zeros();

    // Scratch buffer for matched-pair indices (avoids allocating per iter).
    let mut nn_indices: Vec<usize> = vec![0; n_data];

    let mut prev_rms = f64::INFINITY;
    let mut last_rms = f64::INFINITY;
    let mut iter_used: u32 = 0;
    let mut converged = false;

    for iter in 0..params.max_iterations {
        iter_used = iter + 1;

        // ── (1) Find correspondences under current (R, t) ─────────
        // `data_points` is mutated in place, so the array IS the current
        // pose. We do NOT apply the cumulative (rotation, translation)
        // here — that would double-transform. (Bug fixed 2026-08-19:
        // an earlier version had `transformed = rotation * pt + translation`
        // here, which caused ICP to diverge after a successful first
        // iteration, because the data was applied to its already-moved
        // pose a second time, breaking the NN correspondences.)
        let mut sum_dist_sq = 0.0_f64;
        for i in 0..n_data {
            let idx = i * 3;
            let pt = Vector3::new(
                data_points[idx] as f64,
                data_points[idx + 1] as f64,
                data_points[idx + 2] as f64,
            );
            let (nn_idx, dist_sq) = nearest_neighbour_slow(model_points, &pt);
            nn_indices[i] = nn_idx;
            sum_dist_sq += dist_sq;
        }
        let rms = (sum_dist_sq / n_data as f64).sqrt();

        // ── (2) Convergence: change in RMS, not absolute RMS ───────
        // (Old check `rms < min_rms_decrease` was wrong: it triggered
        //  whenever RMS happened to be small, not when ICP stopped
        //  making progress.)
        let delta = (prev_rms - rms).abs();
        prev_rms = rms;
        last_rms = rms;
        log::debug!("iter {}: rms={:.6}, delta={:.6e}", iter, rms, delta);

        if iter > 0 && (delta < params.min_rms_decrease || rms < params.min_rms_absolute) {
            converged = true;
            break;
        }

        // ── (3) Compute optimal (R, t) from the matched pairs ───────
        // The matched pairs are: (data_i, model[nn_indices[i]]).
        // The Horn 1987 SVD gives an INCREMENTAL (ΔR, Δt) that aligns
        // the current data pose to the matched model pose.
        let (centroid_data, centroid_model) = compute_centroids_from_pairs(
            data_points, model_points, &nn_indices,
        );
        let h = compute_covariance_from_pairs(
            data_points, model_points, &nn_indices,
            centroid_data, centroid_model,
        );
        let delta_rotation = compute_rotation_svd(&h)
            .map_err(|e| IcprErrorRust { code: 3, message: e.to_string() })?;

        // Incremental translation: given the matched pairs and ΔR,
        //   model ≈ ΔR · (data - c_data) + c_model
        //          = ΔR · data + (c_model - ΔR · c_data)
        // so the incremental translation is t_inc = c_model - ΔR · c_data.
        let delta_t = centroid_model - delta_rotation * centroid_data;

        // Track the cumulative transform for the return value.
        //   R_{k+1} = ΔR · R_k
        //   t_{k+1} = ΔR · t_k + t_inc
        rotation = delta_rotation * rotation;
        translation = delta_rotation * translation + delta_t;

        // Apply the INCREMENTAL transform to the data in place.
        //   data_new[i] = ΔR · data[i] + Δt
        // This produces the same final pose as applying the cumulative
        // transform to the original data:
        //   ΔR · (R_k · data_0 + t_k) + Δt
        //     = (ΔR · R_k) · data_0 + (ΔR · t_k + Δt)
        //     = R_{k+1} · data_0 + t_{k+1}
        for i in 0..n_data {
            let idx = i * 3;
            let pt = Vector3::new(
                data_points[idx] as f64,
                data_points[idx + 1] as f64,
                data_points[idx + 2] as f64,
            );
            let new_pt = delta_rotation * pt + delta_t;
            data_points[idx]     = new_pt[0] as f32;
            data_points[idx + 1] = new_pt[1] as f32;
            data_points[idx + 2] = new_pt[2] as f32;
        }
    }

    Ok(IcprResultRust {
        rms: last_rms,
        converged,
        iterations: iter_used,
        transform: build_transform_matrix(&rotation, &translation),
    })
}

// ── Helpers ────────────────────────────────────────────────────────────────

/// Compute centroids of two point sets paired by index. Used when the
/// caller has explicit correspondences (e.g. NN indices from ICP).
fn compute_centroids_from_pairs(
    data: &[f32],
    model: &[f32],
    nn_indices: &[usize],
) -> (Vector3<f64>, Vector3<f64>) {
    let n = nn_indices.len();
    let mut c_data = Vector3::zeros();
    let mut c_model = Vector3::zeros();
    for i in 0..n {
        c_data += Vector3::new(
            data[i * 3] as f64,
            data[i * 3 + 1] as f64,
            data[i * 3 + 2] as f64,
        );
        let m = nn_indices[i];
        c_model += Vector3::new(
            model[m * 3] as f64,
            model[m * 3 + 1] as f64,
            model[m * 3 + 2] as f64,
        );
    }
    let inv = 1.0 / n as f64;
    (c_data * inv, c_model * inv)
}

/// Compute cross-covariance matrix H between paired (data, model) sets
/// after centring. The pairs come from explicit correspondences
/// (e.g. NN indices), not parallel index assumption.
fn compute_covariance_from_pairs(
    data: &[f32],
    model: &[f32],
    nn_indices: &[usize],
    centroid_data: Vector3<f64>,
    centroid_model: Vector3<f64>,
) -> nalgebra::Matrix3<f64> {
    let n = nn_indices.len();
    let mut h = nalgebra::Matrix3::<f64>::zeros();
    for i in 0..n {
        let dp = Vector3::new(
            data[i * 3] as f64,
            data[i * 3 + 1] as f64,
            data[i * 3 + 2] as f64,
        ) - centroid_data;
        let m = nn_indices[i];
        let mp = Vector3::new(
            model[m * 3] as f64,
            model[m * 3 + 1] as f64,
            model[m * 3 + 2] as f64,
        ) - centroid_model;
        h += dp * mp.transpose();
    }
    h
}

/// Brute-force O(n) nearest-neighbour. Returns (index, squared_distance).
/// Reused by the iterative ICP and by the unit tests.
fn nearest_neighbour_slow(model: &[f32], query: &Vector3<f64>) -> (usize, f64) {
    let n = model.len() / 3;
    let mut best_idx = 0;
    let mut best_dist_sq = f64::MAX;
    for i in 0..n {
        let dx = model[i * 3] as f64 - query[0];
        let dy = model[i * 3 + 1] as f64 - query[1];
        let dz = model[i * 3 + 2] as f64 - query[2];
        let d = dx * dx + dy * dy + dz * dz;
        if d < best_dist_sq {
            best_dist_sq = d;
            best_idx = i;
        }
    }
    (best_idx, best_dist_sq)
}

/// SVD-based rotation estimation (Horn 1987 / Arun 1987).
///
/// Given the 3×3 cross-covariance `H = Σ (data_i − c_data)(model_i − c_model)^T`,
/// this returns the optimal rotation matrix `ΔR` that minimises
/// `Σ ‖ΔR · (data_i − c_data) − (model_i − c_model)‖²`.
///
/// The SVD of `H` is `H = U · S · V^T` (note: V is transposed in the
/// `v_t` field — nalgebra stores V^T, not V). The Horn/Arun optimal
/// rotation is therefore:
///
/// ```text
///     R = V · U^T
///       = (V^T)^T · U^T
///       = v_t.transpose() * u.transpose()
/// ```
///
/// **Implementation note:** an earlier version computed `v_t * u.transpose()`,
/// which is V^T · U^T = (U · V)^T. That is the wrong matrix — it gives a
/// mirror/reflection transformation rather than a rotation, and ICP
/// diverges immediately. See D4 in `experimental/docs/decisions.md`
/// for the bug history.
fn compute_rotation_svd(
    h: &nalgebra::Matrix3<f64>,
) -> Result<nalgebra::Matrix3<f64>, RegistrationError> {
    use nalgebra::linalg::SVD;
    let svd = SVD::new(*h, true, true);
    let v_t = svd.v_t.ok_or(RegistrationError::SingularMatrix)?;
    let u = svd.u.ok_or(RegistrationError::SingularMatrix)?;
    // R = V · U^T, where V = (V^T)^T.
    let mut r = v_t.transpose() * u.transpose();
    // Enforce det(R) = +1 (proper rotation, not reflection).
    if r.determinant() < 0.0 {
        let n_cols = r.ncols();
        for i in 0..r.nrows() {
            r[(i, n_cols - 1)] = -r[(i, n_cols - 1)];
        }
    }
    Ok(r)
}

/// Build a 4×4 column-major transformation matrix from R and t.
fn build_transform_matrix(
    rot: &nalgebra::Matrix3<f64>,
    trans: &Vector3<f64>,
) -> Vec<f64> {
    let mut m = vec![0.0_f64; 16];
    // Column-major: m[col*4 + row]
    m[0]  = rot[(0, 0)]; m[1]  = rot[(1, 0)]; m[2]  = rot[(2, 0)]; m[3]  = 0.0;
    m[4]  = rot[(0, 1)]; m[5]  = rot[(1, 1)]; m[6]  = rot[(2, 1)]; m[7]  = 0.0;
    m[8]  = rot[(0, 2)]; m[9]  = rot[(1, 2)]; m[10] = rot[(2, 2)]; m[11] = 0.0;
    m[12] = trans[0];    m[13] = trans[1];    m[14] = trans[2];    m[15] = 1.0;
    m
}

// ── Tests ──────────────────────────────────────────────────────────────────
//
// **Note on test fixtures:** the early tests used the 8 cube corners as the
// model cloud, then translated/rotated it as the data. The cube is *too
// symmetric* for ICP: translating a cube along an axis produces a degenerate
// cross-covariance matrix (rank 2 instead of 3) because every model point
// sits on the same axis-aligned grid, and the X/Y/Z channels of the matched
// pairs decouple. The SVD of a rank-deficient H has no unique answer for
// the missing axis, and ICP picks a reflection instead of a translation.
//
// Real-world point clouds (e.g. building scans) are non-symmetric, so the
// algorithm is fine. The fix here is to use a *non-symmetric* test cloud.
// We use a "L"-shaped fixture: 8 cube corners + one off-axis point
// (1.5, 0.3, 0.7) that breaks the symmetry.

#[cfg(test)]
mod tests {
    use super::*;

    /// Cube corners (8 points) + one off-axis point (1.5, 0.3, 0.7).
    /// The off-axis point breaks the cube's symmetry, so the
    /// cross-covariance H is full-rank.
    fn asymmetric_cloud() -> Vec<f32> {
        let mut v = vec![
            0.0, 0.0, 0.0,  1.0, 0.0, 0.0,  0.0, 1.0, 0.0,  0.0, 0.0, 1.0,
            1.0, 1.0, 0.0,  1.0, 0.0, 1.0,  0.0, 1.0, 1.0,  1.0, 1.0, 1.0,
        ];
        // The "L" corner — off the centre, off the unit grid.
        v.extend_from_slice(&[1.5, 0.3, 0.7]);
        v
    }

    /// Apply a rigid transform (R, t) to a point cloud in place.
    fn apply_rigid(p: &mut [f32], rot: &nalgebra::Matrix3<f64>, t: &Vector3<f64>) {
        for i in 0..p.len() / 3 {
            let v = Vector3::new(p[i*3] as f64, p[i*3+1] as f64, p[i*3+2] as f64);
            let r = rot * v + t;
            p[i*3]   = r[0] as f32;
            p[i*3+1] = r[1] as f32;
            p[i*3+2] = r[2] as f32;
        }
    }

    fn rot_xyz(rx: f64, ry: f64, rz: f64) -> nalgebra::Matrix3<f64> {
        // Rz * Ry * Rx (column-major convention)
        let cx = rx.cos(); let sx = rx.sin();
        let cy = ry.cos(); let sy = ry.sin();
        let cz = rz.cos(); let sz = rz.sin();
        let rx_m = nalgebra::Matrix3::new(
            1.0, 0.0,  0.0,
            0.0, cx,  -sx,
            0.0, sx,   cx,
        );
        let ry_m = nalgebra::Matrix3::new(
             cy, 0.0, sy,
             0.0, 1.0, 0.0,
            -sy, 0.0, cy,
        );
        let rz_m = nalgebra::Matrix3::new(
            cz, -sz, 0.0,
            sz,  cz, 0.0,
            0.0, 0.0, 1.0,
        );
        rz_m * ry_m * rx_m
    }

    /// Run ICP and return the recovered transform.
    fn recover_transform(
        data_initial: &[f32],
        model: &[f32],
        params: &IcprParamsRust,
    ) -> IcprResultRust {
        let mut data = data_initial.to_vec();
        icp_iterate(&mut data, model, params).expect("ICP failed")
    }

    #[test]
    fn translation_only() {
        // Asymmetric cloud translated by (1, 0, 0). ICP returns the
        // "data → model" transform: t = (-1, 0, 0), R = I.
        let model = asymmetric_cloud();
        let data_offset = Vector3::new(1.0, 0.0, 0.0);
        let mut data = model.clone();
        apply_rigid(&mut data, &nalgebra::Matrix3::identity(), &data_offset);
        let expected_t = -data_offset;
        let params = IcprParamsRust { max_iterations: 50, min_rms_decrease: 1e-6, ..Default::default() };
        let r = recover_transform(&data, &model, &params);
        let t = Vector3::new(r.transform[12], r.transform[13], r.transform[14]);
        assert!((t - expected_t).norm() < 0.05,
                "expected translation {:?} (data→model), got {:?}, rms={}",
                expected_t, t, r.rms);
        assert!(r.rms < 0.05, "rms too high: {}", r.rms);
    }

    #[test]
    fn rotation_only() {
        // Asymmetric cloud rotated 30° around Z. Recovered (R, t) maps
        // data to model; since the data is purely a rotation, t ≈ 0.
        let model = asymmetric_cloud();
        let known_rot = rot_xyz(0.0, 0.0, 30.0_f64.to_radians());
        let mut data = model.clone();
        apply_rigid(&mut data, &known_rot, &Vector3::zeros());

        let params = IcprParamsRust { max_iterations: 100, min_rms_decrease: 1e-6, ..Default::default() };
        let r = recover_transform(&data, &model, &params);
        let tx = r.transform[12];
        let ty = r.transform[13];
        let tz = r.transform[14];
        assert!((tx*tx + ty*ty + tz*tz).sqrt() < 0.1,
                "translation should be ~0, got ({},{},{})", tx, ty, tz);
        assert!(r.rms < 0.1, "rms too high: {}", r.rms);
        // The recovered rotation matrix's angle should be ~30° around Z.
        // For a 30° Z-rotation, R[0,0] = cos(30°) ≈ 0.866.
        let r00 = r.transform[0];
        let r11 = r.transform[5];
        let cos_theta = (r00 + r11) * 0.5;
        assert!((cos_theta - 30.0_f64.to_radians().cos()).abs() < 0.05,
                "expected cos(30°) ≈ 0.866, got {}", cos_theta);
    }

    #[test]
    fn rotation_and_translation() {
        // Asymmetric cloud rotated 30° around Z and translated by
        // (0.5, -0.3, 0.2). After ICP, the residual should be small.
        let model = asymmetric_cloud();
        let known_rot = rot_xyz(0.0, 0.0, 30.0_f64.to_radians());
        let data_offset = Vector3::new(0.5, -0.3, 0.2);
        let mut data = model.clone();
        apply_rigid(&mut data, &known_rot, &data_offset);

        let params = IcprParamsRust { max_iterations: 200, min_rms_decrease: 1e-6, ..Default::default() };
        let r = recover_transform(&data, &model, &params);
        assert!(r.rms < 0.1,
                "rms too high: {} (translation+rotation recovery failed)", r.rms);
    }

    #[test]
    fn identity_converges_quickly() {
        // data = model → ICP should converge immediately.
        let model = asymmetric_cloud();
        let data = model.clone();
        let params = IcprParamsRust { max_iterations: 5, min_rms_decrease: 1e-6, ..Default::default() };
        let r = icp_iterate(&mut data.clone(), &model, &params).expect("ICP failed");
        assert!(r.converged, "should converge on identical clouds");
        assert!(r.iterations <= 2, "expected ≤2 iterations, got {}", r.iterations);
    }

    #[test]
    fn too_few_points() {
        let model = vec![0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0];
        let mut data = vec![0.0, 0.0, 0.0, 1.0, 0.0, 0.0];
        let params = IcprParamsRust { max_iterations: 5, min_rms_decrease: 1e-6, ..Default::default() };
        let r = icp_iterate(&mut data, &model, &params);
        assert!(r.is_err());
        assert_eq!(r.unwrap_err().code, 1);
    }
}
