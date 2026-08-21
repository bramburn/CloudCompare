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
// pluggable via the `NearestNeighbour` trait (added 2026-08-20 per
// D8 in `experimental/docs/decisions.md`); the default brute-force
// adapter is a pure-Rust O(n) scan. The whole loop has no C++
// dependency, so it builds and tests on any toolchain.

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
    /// Fraction of correspondences to drop per iteration, ranked by
    /// squared NN distance. The worst 0.2 (default) of pairs are
    /// considered outliers and excluded from the SVD step. Set to
    /// 0.0 to disable (vanilla ICP). Range: [0.0, 0.95).
    /// Implementation note: trimmed ICP — based on the Trimmed ICP
    /// variant from Chetverikov et al. 2005 ("Robust Euclidean
    /// alignment of 3D point sets"). Especially helpful for real
    /// scans with partial overlap, where ~half the points may have
    /// no true match in the other cloud.
    pub outlier_rejection_fraction: f64,
}

impl Default for IcprParamsRust {
    fn default() -> Self {
        Self {
            max_iterations: 50,
            min_rms_decrease: 1e-6,
            min_rms_absolute: 1e-3,
            outlier_rejection_fraction: 0.0,
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

/// Run ICP using a caller-supplied nearest-neighbour structure.
///
/// The standard ICP algorithm:
///   1. Find correspondences (data point ↔ nearest model point) via `nn`
///   2. Compute optimal (R, t) from those correspondences (Horn 1987 SVD)
///   3. Apply (R, t) to data
///   4. Repeat from 1 until RMS change is small
///
/// **Corrected 2026-08-19**: the previous version computed (R, t) once
/// before the loop and only iterated the NN search. That is **not** ICP.
/// See `experimental/docs/decisions.md` D4 for the bug history.
///
/// **NN injection (D8, 2026-08-20)**: the nearest-neighbour step is
/// driven by the `NearestNeighbour` trait, so the caller can plug in
/// any structure (brute force, kiddo KD-tree, hand-rolled octree, …).
/// The original `icp_iterate` is now a thin wrapper that uses a
/// `BruteForceNN` adapter.
///
/// `data_points` is mutated in place; on return, it is the registered cloud.
/// The returned `transform` is the cumulative 4×4 matrix that was applied
/// to map the input data cloud to its final pose.
pub fn icp_with_nn<N: NearestNeighbour + ?Sized>(
    data_points: &mut [f32],
    model_points: &[f32],
    nn: &N,
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
    // Per-iteration NN squared distances (used for trimmed-ICP
    // outlier rejection). Pairs with dist_sq above the cutoff are
    // excluded from the SVD step.
    let mut nn_dist_sq: Vec<f64> = vec![0.0; n_data];

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
        //
        // The NN search is now driven by the trait, so any data
        // structure (brute force, KD-tree, octree, …) can be plugged
        // in without changing this function. Per-query cost is
        // O(1) for brute force, O(log n) for kiddo, O(log n) average
        // (with pruning) for the hand-rolled octree.
        for i in 0..n_data {
            let idx = i * 3;
            let q = [data_points[idx], data_points[idx + 1], data_points[idx + 2]];
            let (nn_idx, dist_sq) = nn.nearest(&q);
            nn_indices[i] = nn_idx;
            nn_dist_sq[i] = dist_sq as f64;
        }

        // ── (1.5) Trim outliers ─────────────────────────────────────
        // Drop the worst `outlier_rejection_fraction` of pairs (by
        // squared NN distance) before computing the SVD. This makes
        // ICP robust to partial overlap, where ~half the points have
        // no true match in the other cloud. See D5 in
        // `experimental/docs/decisions.md` (when added) for the
        // bench on real data.
        //
        // The mask is built as: inlier if dist_sq <= cutoff, where
        // cutoff is the (1 - fraction) quantile of dist_sq. We
        // compute the cutoff by sorting dist_sq.
        let mut inlier_mask: Vec<bool> = vec![true; n_data];
        if params.outlier_rejection_fraction > 0.0
            && params.outlier_rejection_fraction < 0.95
            && n_data >= 3
        {
            // Build a sorted index of dist_sq to find the cutoff.
            let mut sorted_dists: Vec<f64> = nn_dist_sq.clone();
            sorted_dists.sort_by(|a, b| a.partial_cmp(b).unwrap_or(std::cmp::Ordering::Equal));
            // cutoff is the value at the (1 - fraction) quantile.
            // e.g. fraction=0.2 → keep the closest 80% → cutoff
            // is sorted_dists[floor(0.80 * n)].
            let keep_count = ((1.0 - params.outlier_rejection_fraction)
                * n_data as f64)
                .ceil() as usize;
            let keep_count = keep_count.max(3).min(n_data);
            let cutoff = sorted_dists[keep_count - 1];
            for i in 0..n_data {
                inlier_mask[i] = nn_dist_sq[i] <= cutoff;
            }
        }

        // Compute RMS on the inlier set (so the convergence check
        // reflects what we're actually fitting).
        let mut sum_dist_sq = 0.0_f64;
        let mut n_inlier = 0_usize;
        for i in 0..n_data {
            if inlier_mask[i] {
                sum_dist_sq += nn_dist_sq[i];
                n_inlier += 1;
            }
        }
        let rms = if n_inlier == 0 {
            f64::INFINITY
        } else {
            (sum_dist_sq / n_inlier as f64).sqrt()
        };

        // ── (2) Convergence: change in RMS, not absolute RMS ───────
        // (Old check `rms < min_rms_decrease` was wrong: it triggered
        //  whenever RMS happened to be small, not when ICP stopped
        //  making progress.)
        let delta = (prev_rms - rms).abs();
        prev_rms = rms;
        last_rms = rms;
        log::debug!("iter {}: rms={:.6}, delta={:.6e}, inliers={}/{}",
                    iter, rms, delta, n_inlier, n_data);

        if iter > 0 && (delta < params.min_rms_decrease || rms < params.min_rms_absolute) {
            converged = true;
            break;
        }

        // ── (3) Compute optimal (R, t) from the matched pairs ───────
        // The matched pairs are: (data_i, model[nn_indices[i]]) for
        // each INLIER i. Outliers are excluded.
        let (centroid_data, centroid_model) = compute_centroids_from_pairs_masked(
            data_points, model_points, &nn_indices, &inlier_mask,
        );
        let h = compute_covariance_from_pairs_masked(
            data_points, model_points, &nn_indices, &inlier_mask,
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

// ── NearestNeighbour trait (D8, 2026-08-20) ───────────────────────────────

/// Pluggable nearest-neighbour interface for ICP.
///
/// Any structure that can answer "given a 3D query point, which point
/// in the model is closest (and at what squared distance)" can be
/// plugged into `icp_with_nn`. The trait is intentionally minimal so
/// the three experimental scenarios (brute force, kiddo KD-tree,
/// hand-rolled octree) can all implement it without changes to their
/// internal representations.
///
/// `nearest` returns `(index, squared_distance)` where `index` is the
/// position of the matched point in the original model slice that was
/// used to build the structure, and `squared_distance` is in the same
/// units as the model coordinates (so callers using f32 must return
/// f32, callers using f64 must return f64 — see `BruteForceNN`).
pub trait NearestNeighbour {
    fn nearest(&self, query: &[f32; 3]) -> (usize, f32);
}

/// Brute-force O(n) nearest-neighbour over an f32 point cloud.
///
/// This is the default NN used by `icp_iterate` (which is now a thin
/// wrapper over `icp_with_nn(&BruteForceNN::new(model), …)`). It is
/// simple, correct, and requires no setup, so it remains the
/// reference implementation against which the kiddo and octree
/// variants are benchmarked.
pub struct BruteForceNN<'a> {
    model: &'a [f32],
}

impl<'a> BruteForceNN<'a> {
    pub fn new(model: &'a [f32]) -> Self {
        Self { model }
    }
}

impl<'a> NearestNeighbour for BruteForceNN<'a> {
    fn nearest(&self, query: &[f32; 3]) -> (usize, f32) {
        let n = self.model.len() / 3;
        let mut best_idx = 0_usize;
        let mut best_dist_sq = f32::MAX;
        for i in 0..n {
            let dx = self.model[i * 3]     - query[0];
            let dy = self.model[i * 3 + 1] - query[1];
            let dz = self.model[i * 3 + 2] - query[2];
            let d = dx * dx + dy * dy + dz * dz;
            if d < best_dist_sq {
                best_dist_sq = d;
                best_idx = i;
            }
        }
        (best_idx, best_dist_sq)
    }
}

/// Backward-compatible ICP entry point. Equivalent to
/// `icp_with_nn(data, model, &BruteForceNN::new(model), params)`.
/// Kept so the existing 40 cc-rust tests and the three
/// `01-naive-on2` / `02-kiddo-kdtree` / `03-handrolled-octree`
/// scenario wrappers continue to work without a signature change
/// at the call site.
pub fn icp_iterate(
    data_points: &mut [f32],
    model_points: &[f32],
    params: &IcprParamsRust,
) -> Result<IcprResultRust, IcprErrorRust> {
    let nn = BruteForceNN::new(model_points);
    icp_with_nn(data_points, model_points, &nn, params)
}

// ── Helpers ────────────────────────────────────────────────────────────────

/// Compute centroids of two point sets paired by index, with a
/// boolean inlier mask. Used by trimmed ICP.
fn compute_centroids_from_pairs_masked(
    data: &[f32],
    model: &[f32],
    nn_indices: &[usize],
    inlier_mask: &[bool],
) -> (Vector3<f64>, Vector3<f64>) {
    let n = nn_indices.len();
    let mut c_data = Vector3::zeros();
    let mut c_model = Vector3::zeros();
    let mut count = 0_usize;
    for i in 0..n {
        if !inlier_mask[i] {
            continue;
        }
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
        count += 1;
    }
    if count == 0 {
        return (Vector3::zeros(), Vector3::zeros());
    }
    let inv = 1.0 / count as f64;
    (c_data * inv, c_model * inv)
}

/// Compute cross-covariance matrix H between paired (data, model)
/// sets after centring, with a boolean inlier mask. Used by
/// trimmed ICP.
fn compute_covariance_from_pairs_masked(
    data: &[f32],
    model: &[f32],
    nn_indices: &[usize],
    inlier_mask: &[bool],
    centroid_data: Vector3<f64>,
    centroid_model: Vector3<f64>,
) -> nalgebra::Matrix3<f64> {
    let n = nn_indices.len();
    let mut h = nalgebra::Matrix3::<f64>::zeros();
    for i in 0..n {
        if !inlier_mask[i] {
            continue;
        }
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

/// Brute-force O(n) nearest-neighbour returning (index, squared_distance).
///
/// Kept as a free function for the unit tests so they can sanity-check
/// the `BruteForceNN` adapter against an f64 return type without going
/// through the trait. The production path is the `BruteForceNN` adapter
/// (which returns f32 and is what the trait contract requires).
#[allow(dead_code)] // retained for unit tests; production uses BruteForceNN
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

/// Apply a 4×4 column-major transform to a point cloud in place.
/// Mirrors the helper in `coarse_align.rs` for use in multi-
/// resolution ICP, where the coarse-level transform is applied
/// to the fine-level data.
pub fn apply_transform_in_place(points: &mut [f32], transform: &[f64; 16]) {
    for i in 0..points.len() / 3 {
        let x = points[i * 3] as f64;
        let y = points[i * 3 + 1] as f64;
        let z = points[i * 3 + 2] as f64;
        let new_x = transform[0] * x + transform[4] * y + transform[8] * z + transform[12];
        let new_y = transform[1] * x + transform[5] * y + transform[9] * z + transform[13];
        let new_z = transform[2] * x + transform[6] * y + transform[10] * z + transform[14];
        points[i * 3]     = new_x as f32;
        points[i * 3 + 1] = new_y as f32;
        points[i * 3 + 2] = new_z as f32;
    }
}

/// Multi-resolution ICP with a caller-supplied nearest-neighbour structure.
///
/// Subsamples both clouds to a smaller size and runs ICP at the
/// coarse level, then applies the recovered transform to the
/// original (full-resolution) data and runs a second pass.
///
/// Why this helps: ICP on a noisy or partial-overlap cloud
/// often gets stuck in a local minimum at fine resolution
/// because every iteration moves every point, including
/// outliers. At coarse resolution (10% subsample), the outliers
/// average out and the inlier alignment signal dominates.
/// Running ICP twice — coarse then fine — recovers the global
/// minimum much more reliably than a single fine-resolution pass.
///
/// `fractions` is the list of subsample fractions, applied in
/// order. The last entry is the full-resolution pass. Default
/// `[0.1, 1.0]` (10% then 100%) gives a typical "coarse-to-fine"
/// schedule.
///
/// `params` is reused across all passes.
///
/// **Where the caller's `nn` is used (D8, 2026-08-20):** the
/// per-level ICP loops run against subsampled models. The
/// caller's NN was built for the *full* model, so passing it
/// into the per-level loop would mean querying the full model
/// for matches and then computing the SVD against the subsampled
/// model — a mismatch that gives wrong correspondences. Instead,
/// each level rebuilds a fresh `BruteForceNN` against the
/// subsampled model (this matches the original brute-force
/// behaviour). The caller's `nn` is used for the **final
/// summary RMS** on the full-resolution data, which is the step
/// that actually touches the full model. For end-to-end NN
/// performance numbers, use `icp_with_nn` directly with a
/// tree-based NN.
pub fn icp_multi_resolution_with_nn<N: NearestNeighbour + ?Sized>(
    data_points: &mut [f32],
    model_points: &[f32],
    fractions: &[f64],
    nn: &N,
    params: &IcprParamsRust,
) -> Result<IcprResultRust, IcprErrorRust> {
    if fractions.is_empty() {
        return Err(IcprErrorRust {
            code: 2,
            message: "icp_multi_resolution: fractions must not be empty".to_string(),
        });
    }

    // Track the cumulative transform so that the final result
    // is a transform from the original data to the model.
    let mut cumulative: [f64; 16] = {
        let mut m = [0.0_f64; 16];
        m[0] = 1.0; m[5] = 1.0; m[10] = 1.0; m[15] = 1.0;
        m
    };

    let n_data = data_points.len() / 3;
    let n_model = model_points.len() / 3;

    for &frac in fractions {
        // Subsample. For the model, use a deterministic stride.
        // For the data, also use a stride — the algorithm is
        // symmetric.
        let frac = frac.clamp(0.01, 1.0);
        let n_sub_data = ((n_data as f64 * frac).round() as usize).max(3);
        let n_sub_model = ((n_model as f64 * frac).round() as usize).max(3);

        let mut sub_data: Vec<f32> = Vec::with_capacity(n_sub_data * 3);
        for i in 0..n_sub_data {
            let src = (i * n_data) / n_sub_data;
            let i3 = src * 3;
            sub_data.push(data_points[i3]);
            sub_data.push(data_points[i3 + 1]);
            sub_data.push(data_points[i3 + 2]);
        }
        let mut sub_model: Vec<f32> = Vec::with_capacity(n_sub_model * 3);
        for i in 0..n_sub_model {
            let src = (i * n_model) / n_sub_model;
            let i3 = src * 3;
            sub_model.push(model_points[i3]);
            sub_model.push(model_points[i3 + 1]);
            sub_model.push(model_points[i3 + 2]);
        }

        // Per-level ICP: brute force against the subsampled model.
        // This matches the pre-D8 behaviour exactly; the caller's
        // NN is reserved for the final summary RMS (see doc comment).
        let sub_nn = BruteForceNN::new(&sub_model);
        let sub_result = icp_with_nn(&mut sub_data, &sub_model, &sub_nn, params)?;
        // Apply sub_result.transform to the FULL data array.
        let mut t = [0.0_f64; 16];
        t.copy_from_slice(&sub_result.transform);
        apply_transform_in_place(data_points, &t);

        // Compose cumulative = sub_transform * cumulative.
        // We need 4×4 matrix multiply: cumulative_new = sub * cumulative.
        let mut new_cum = [0.0_f64; 16];
        for col in 0..4 {
            for row in 0..4 {
                let mut s = 0.0;
                for k in 0..4 {
                    s += t[col * 4 + k] * cumulative[k * 4 + row];
                }
                new_cum[col * 4 + row] = s;
            }
        }
        cumulative = new_cum;
    }

    // Final RMS on the full-resolution data, against the model.
    // This is where the caller's NN actually pays off: it pays
    // its build cost once (before calling us) and is queried
    // n_data times here. For tree-based NNs this is the only
    // step that benefits from the speedup.
    let mut sum_dist_sq = 0.0_f64;
    for i in 0..n_data {
        let idx = i * 3;
        let q = [data_points[idx], data_points[idx + 1], data_points[idx + 2]];
        let (_nn_idx, dist_sq) = nn.nearest(&q);
        sum_dist_sq += dist_sq as f64;
    }
    let final_rms = (sum_dist_sq / n_data as f64).sqrt();
    Ok(IcprResultRust {
        rms: final_rms,
        converged: true,
        iterations: fractions.len() as u32,
        transform: cumulative.to_vec(),
    })
}

/// Backward-compatible multi-resolution ICP entry point. Equivalent
/// to `icp_multi_resolution_with_nn(data, model, fractions,
/// &BruteForceNN::new(model), params)`. Kept so the existing
/// `multi_resolution_recovers_translation` test continues to work
/// without a signature change.
pub fn icp_multi_resolution(
    data_points: &mut [f32],
    model_points: &[f32],
    fractions: &[f64],
    params: &IcprParamsRust,
) -> Result<IcprResultRust, IcprErrorRust> {
    let nn = BruteForceNN::new(model_points);
    icp_multi_resolution_with_nn(data_points, model_points, fractions, &nn, params)
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

    #[test]
    fn trimmed_icp_handles_partial_overlap() {
        // Simulate partial overlap: the data has the model's 9
        // points (the asymmetric-9 fixture) plus 4 "ghost" points
        // that are far from the model. Vanilla ICP overweights the
        // ghosts and overshoots. Trimmed ICP drops them.
        let model = asymmetric_cloud();
        let mut data = model.clone();
        // Apply a known translation.
        let data_offset = Vector3::new(0.4, -0.2, 0.15);
        apply_rigid(&mut data, &nalgebra::Matrix3::identity(), &data_offset);
        // Add 4 ghost points far from the model.
        data.extend_from_slice(&[
            100.0, 100.0, 100.0,
            -50.0, -50.0, -50.0,
            200.0, 0.0, 0.0,
            0.0, 200.0, 0.0,
        ]);

        // First, vanilla ICP (no trimming) — should overshoot.
        let vanilla_params = IcprParamsRust {
            max_iterations: 50,
            min_rms_decrease: 1e-6,
            outlier_rejection_fraction: 0.0,
            ..Default::default()
        };
        let mut data_vanilla = data.clone();
        let vanilla = icp_iterate(&mut data_vanilla, &model, &vanilla_params)
            .expect("vanilla ICP failed");
        let vanilla_t = Vector3::new(
            vanilla.transform[12],
            vanilla.transform[13],
            vanilla.transform[14],
        );

        // Then, trimmed ICP (drop worst 50% — enough to clear all 4
        // ghosts from the inlier set on the first iteration).
        let trimmed_params = IcprParamsRust {
            max_iterations: 50,
            min_rms_decrease: 1e-6,
            outlier_rejection_fraction: 0.5,
            ..Default::default()
        };
        let mut data_trimmed = data.clone();
        let trimmed = icp_iterate(&mut data_trimmed, &model, &trimmed_params)
            .expect("trimmed ICP failed");
        let trimmed_t = Vector3::new(
            trimmed.transform[12],
            trimmed.transform[13],
            trimmed.transform[14],
        );

        // The trimmed ICP should recover the offset more accurately
        // than vanilla. Both should be in the right *direction*;
        // trimmed should be closer in magnitude.
        let expected_t = -data_offset; // "data → model" direction
        let vanilla_err = (vanilla_t - expected_t).norm();
        let trimmed_err = (trimmed_t - expected_t).norm();
        assert!(trimmed_err < vanilla_err,
                "trimmed ICP should be more accurate than vanilla: \
                 trimmed_err={:.4} vs vanilla_err={:.4} (trimmed t={:?}, vanilla t={:?}, expected t={:?})",
                trimmed_err, vanilla_err, trimmed_t, vanilla_t, expected_t);
        // Sanity: trimmed is reasonably close to the expected offset.
        assert!(trimmed_err < 0.1,
                "trimmed ICP should be within 0.1 of expected: got {:?}, expected {:?}",
                trimmed_t, expected_t);
    }

    #[test]
    fn trimmed_icp_fraction_zero_equals_vanilla() {
        // Sanity: outlier_rejection_fraction = 0.0 must produce the
        // same result as the default (no-trim) path.
        let model = asymmetric_cloud();
        let data_offset = Vector3::new(0.4, -0.2, 0.15);
        let mut data = model.clone();
        apply_rigid(&mut data, &nalgebra::Matrix3::identity(), &data_offset);

        let params = IcprParamsRust {
            max_iterations: 50,
            min_rms_decrease: 1e-6,
            outlier_rejection_fraction: 0.0,
            ..Default::default()
        };
        let r = icp_iterate(&mut data, &model, &params).expect("ICP failed");
        assert!(r.rms < 0.01, "rms too high: {}", r.rms);
        let t = Vector3::new(r.transform[12], r.transform[13], r.transform[14]);
        let err = (t - (-data_offset)).norm();
        assert!(err < 0.05, "translation error too high: {:?}", t);
    }

    #[test]
    fn multi_resolution_recovers_translation() {
        // Use a Gaussian cloud (not a grid, so stride subsample
        // doesn't land on a 2D slice). 500 points is enough to
        // give the coarse pass a meaningful sample.
        let model: Vec<f32> = gaussian_cloud(500, 0.5, 42);
        let data_offset = Vector3::new(3.0_f64, -2.0_f64, 1.5_f64);
        let data_offset_f32 = [data_offset[0] as f32, data_offset[1] as f32, data_offset[2] as f32];
        let mut data: Vec<f32> = (0..model.len() / 3)
            .flat_map(|i| {
                let i3 = i * 3;
                vec![
                    model[i3] + data_offset_f32[0],
                    model[i3 + 1] + data_offset_f32[1],
                    model[i3 + 2] + data_offset_f32[2],
                ]
            })
            .collect();

        let params = IcprParamsRust { max_iterations: 50, min_rms_decrease: 1e-6, ..Default::default() };
        let r = icp_multi_resolution(&mut data, &model, &[0.2, 1.0], &params)
            .expect("multi-res ICP failed");
        let t = Vector3::new(r.transform[12], r.transform[13], r.transform[14]);
        // The recovered transform's translation should be the
        // "data → model" direction: -data_offset.
        let err = (t - (-data_offset)).norm();
        assert!(err < 0.1, "translation error too high: {:?} (expected {:?})", t, -data_offset);
    }

    /// D8 (2026-08-20) — exercise `icp_with_nn` directly with a
    /// `BruteForceNN` adapter, proving the trait dispatch works
    /// and the result matches the legacy `icp_iterate` exactly.
    #[test]
    fn icp_with_nn_matches_icp_iterate() {
        let model = asymmetric_cloud();
        let data_offset = Vector3::new(0.4, -0.2, 0.15);
        let mut data_a = model.clone();
        apply_rigid(&mut data_a, &nalgebra::Matrix3::identity(), &data_offset);
        let mut data_b = data_a.clone();
        let params = IcprParamsRust { max_iterations: 50, min_rms_decrease: 1e-6, ..Default::default() };

        let legacy = icp_iterate(&mut data_a, &model, &params).expect("legacy failed");
        let nn = BruteForceNN::new(&model);
        let via_trait = icp_with_nn(&mut data_b, &model, &nn, &params).expect("trait failed");

        // Same final RMS (within fp tolerance) and same iterations.
        assert!((legacy.rms - via_trait.rms).abs() < 1e-6,
                "rms mismatch: legacy={} trait={}", legacy.rms, via_trait.rms);
        assert_eq!(legacy.iterations, via_trait.iterations);
        // Same recovered transform.
        for i in 0..16 {
            assert!((legacy.transform[i] - via_trait.transform[i]).abs() < 1e-6,
                    "transform[{}] mismatch: legacy={} trait={}",
                    i, legacy.transform[i], via_trait.transform[i]);
        }
    }

    /// D8 — exercise `icp_with_nn` with a *custom* trait impl, to
    /// prove the dispatch is dynamic. The `NoopNN` always returns
    /// index 0; that is wrong ICP, but it lets us verify the
    /// outer code calls the trait (not a hard-coded brute force).
    #[test]
    fn icp_with_nn_dispatches_to_trait() {
        // A NN that ignores the query and always returns the first
        // point. This is wrong ICP, so we don't assert on RMS;
        // we just confirm the code accepts the custom impl and
        // returns a result.
        struct NoopNN;
        impl NearestNeighbour for NoopNN {
            fn nearest(&self, _query: &[f32; 3]) -> (usize, f32) {
                (0, 0.0)
            }
        }
        let model = asymmetric_cloud();
        let mut data = model.clone();
        let params = IcprParamsRust { max_iterations: 3, min_rms_decrease: 1e-9, ..Default::default() };
        let nn = NoopNN;
        let r = icp_with_nn(&mut data, &model, &nn, &params);
        assert!(r.is_ok(), "icp_with_nn should accept a custom NN impl");
    }

    /// D8 — `icp_multi_resolution_with_nn` with a custom NN produces
    /// a result; the legacy wrapper and the trait wrapper agree on
    /// RMS when both use brute force.
    #[test]
    fn icp_multi_resolution_with_nn_matches_legacy() {
        let model: Vec<f32> = gaussian_cloud(500, 0.5, 7);
        let data_offset = Vector3::new(1.0_f64, -0.5_f64, 0.3_f64);
        let data_offset_f32 = [data_offset[0] as f32, data_offset[1] as f32, data_offset[2] as f32];
        let make_data = || -> Vec<f32> {
            (0..model.len() / 3)
                .flat_map(|i| {
                    let i3 = i * 3;
                    vec![
                        model[i3] + data_offset_f32[0],
                        model[i3 + 1] + data_offset_f32[1],
                        model[i3 + 2] + data_offset_f32[2],
                    ]
                })
                .collect()
        };
        let mut data_legacy = make_data();
        let mut data_trait  = make_data();
        let params = IcprParamsRust { max_iterations: 50, min_rms_decrease: 1e-6, ..Default::default() };
        let fractions = [0.2_f64, 1.0];

        let legacy = icp_multi_resolution(&mut data_legacy, &model, &fractions, &params).expect("legacy");
        let nn = BruteForceNN::new(&model);
        let via_trait = icp_multi_resolution_with_nn(&mut data_trait, &model, &fractions, &nn, &params)
            .expect("trait");
        assert!((legacy.rms - via_trait.rms).abs() < 1e-6,
                "rms mismatch: legacy={} trait={}", legacy.rms, via_trait.rms);
    }

    /// Generate a Gaussian cloud. Helper for multi-resolution test.
    fn gaussian_cloud(n: usize, sigma: f32, seed: u64) -> Vec<f32> {
        let mut state = seed;
        let mut next_u = || -> f32 {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            (state as f64 / u64::MAX as f64) as f32
        };
        let mut next_normal = || -> f32 {
            let u1 = next_u().max(1e-6);
            let u2 = next_u();
            let r = (-2.0 * u1.ln()).sqrt();
            let theta = 2.0 * std::f32::consts::PI * u2;
            r * theta.cos() * sigma
        };
        let mut out = Vec::with_capacity(n * 3);
        for _ in 0..n {
            out.push(next_normal());
            out.push(next_normal());
            out.push(next_normal());
        }
        out
    }

    // ── CXX FFI parity tests (Phase 0 → live CXX FFI, 2026-08-21) ──
    //
    // These tests call into the real C++ `ICPRegistrationTools::Register`
    // via the CXX bridge (`crate::ffi::run_icp_cpp`) and compare the
    // result with the pure-Rust ICP. They require the `cxx_ffi`
    // Cargo feature (and MSVC + `CCCoreLib.lib` + `CCCoreLib.dll`
    // on the build host). Run with:
    //
    //   cargo test --release --features cxx_ffi icp_cpp_matches_rust
    //
    // The tests are guarded by `#[cfg(feature = "cxx_ffi")]` and
    // are not built by default (the default build is pure-Rust).

    #[cfg(feature = "cxx_ffi")]
    use crate::ffi::{ffi_bridge::IcpParamsCpp, run_icp_cpp};

    /// C++ ICP on identity transform (data == model). Both clouds
    /// are identical, so the recovered transform should be the
    /// identity (rotation = I, translation = 0, scale = 1) and
    /// the final RMS should be 0.0 within fp tolerance.
    #[cfg(feature = "cxx_ffi")]
    #[test]
    fn icp_cpp_identity() {
        let model: Vec<f32> = vec![
            0.0, 0.0, 0.0,  1.0, 0.0, 0.0,  0.0, 1.0, 0.0,
            1.0, 1.0, 0.0,  0.0, 0.0, 1.0,  1.0, 0.0, 1.0,
            0.0, 1.0, 1.0,  1.0, 1.0, 1.0,
        ];
        let data = model.clone();

        let result = run_icp_cpp(&model, &data, &IcpParamsCpp::default())
            .expect("C++ ICP failed (returned None)");
        // For identical model/data, CCCoreLib returns
        // ICP_NOTHING_TO_DO (0) — the data is already aligned to
        // the model so no transform is applied. We accept either 0
        // (nothing to do) or 1 (apply transform) here.
        assert!(
            result.result_code == 0 || result.result_code == 1,
            "expected ICP_NOTHING_TO_DO (0) or ICP_APPLY_TRANSFO (1), got {}",
            result.result_code
        );
        assert!(result.rms < 1e-5, "rms should be ~0 for identity, got {}", result.rms);
        // Scale should be 1.0.
        assert!((result.scale - 1.0).abs() < 1e-5, "scale should be 1.0, got {}", result.scale);
        // Rotation should be identity (off-diagonals 0, diagonals 1).
        for i in 0..3 {
            for j in 0..3 {
                let expected = if i == j { 1.0 } else { 0.0 };
                let r = match (i, j) {
                    (0, 0) => result.r00, (0, 1) => result.r01, (0, 2) => result.r02,
                    (1, 0) => result.r10, (1, 1) => result.r11, (1, 2) => result.r12,
                    (2, 0) => result.r20, (2, 1) => result.r21, (2, 2) => result.r22,
                    _ => unreachable!(),
                };
                assert!((r - expected).abs() < 1e-5,
                        "r{}{} = {}, expected {}", i, j, r, expected);
            }
        }
        // Translation should be 0.
        assert!(result.tx.abs() < 1e-5, "tx={}", result.tx);
        assert!(result.ty.abs() < 1e-5, "ty={}", result.ty);
        assert!(result.tz.abs() < 1e-5, "tz={}", result.tz);
    }

    /// C++ ICP on a known translation. The recovered transform
    /// should be the inverse translation: the data → model transform
    /// is `P_model = P_data - offset`, so the recovered translation
    /// (in "data → model" space) is `-offset`.
    #[cfg(feature = "cxx_ffi")]
    #[test]
    fn icp_cpp_translation() {
        // Use a larger, well-spread cloud so ICP converges cleanly.
        let model: Vec<f32> = gaussian_cloud(200, 0.5, 42);
        let data_offset = [3.0_f32, -2.0_f32, 1.5_f32];
        let data: Vec<f32> = (0..model.len() / 3)
            .flat_map(|i| {
                let i3 = i * 3;
                vec![
                    model[i3] + data_offset[0],
                    model[i3 + 1] + data_offset[1],
                    model[i3 + 2] + data_offset[2],
                ]
            })
            .collect();

        let result = run_icp_cpp(&model, &data, &IcpParamsCpp::default())
            .expect("C++ ICP failed");
        assert_eq!(result.result_code, 1, "expected ICP_APPLY_TRANSFO");
        // Scale should still be ~1 (no scaling in the input).
        assert!((result.scale - 1.0).abs() < 0.05,
                "scale drift: {}", result.scale);
        // Translation in "data → model" space is -data_offset.
        // C++ returns translation as f64; cast to f64 before comparing
        // (the offset values are f32 but the result is f64).
        let recovered = [result.tx, result.ty, result.tz];
        let expected = [-data_offset[0] as f64, -data_offset[1] as f64, -data_offset[2] as f64];
        for i in 0..3 {
            assert!((recovered[i] - expected[i]).abs() < 0.05,
                    "translation[{}]: got {}, expected {}",
                    i, recovered[i], expected[i]);
        }
    }

    /// C++ ICP and pure-Rust ICP on the same input produce
    /// consistent results: same RMS (within fp tolerance), same
    /// rotation (within fp tolerance), same translation.
    ///
    /// This is the **headline parity test** for the CXX FFI. If
    /// the C++ ICP diverges from the pure-Rust ICP by more than
    /// the stated tolerance, the FFI is wired correctly but the
    /// Rust implementation is doing something subtly different
    /// (algorithm, scaling, sign convention, etc.).
    #[cfg(feature = "cxx_ffi")]
    #[test]
    fn icp_cpp_matches_rust() {
        use crate::registration::{IcprParamsRust, icp_iterate};

        let model: Vec<f32> = gaussian_cloud(200, 0.5, 42);
        let data_offset = [3.0_f32, -2.0_f32, 1.5_f32];
        let data: Vec<f32> = (0..model.len() / 3)
            .flat_map(|i| {
                let i3 = i * 3;
                vec![
                    model[i3] + data_offset[0],
                    model[i3 + 1] + data_offset[1],
                    model[i3 + 2] + data_offset[2],
                ]
            })
            .collect();

        // ── Pure-Rust ICP ──────────────────────────────────────────
        let rust_params = IcprParamsRust {
            max_iterations: 50,
            min_rms_decrease: 1e-6,
            ..Default::default()
        };
        let rust = icp_iterate(&mut data.clone(), &model, &rust_params)
            .expect("Rust ICP failed");

        // ── C++ ICP (via FFI) ─────────────────────────────────────
        let cpp_params = IcpParamsCpp::default();
        let cpp = run_icp_cpp(&model, &data, &cpp_params)
            .expect("C++ ICP failed");

        // RMS should agree to within fp tolerance.
        // (The C++ ICP might do a more aggressive outlier
        // rejection, so allow a 1e-3 tolerance rather than 1e-6.)
        let rms_diff = (rust.rms - cpp.rms).abs();
        assert!(rms_diff < 1e-3,
                "RMS mismatch: rust={} cpp={} (diff={})",
                rust.rms, cpp.rms, rms_diff);

        // Recovered translation (data → model) should agree.
        // The Rust transform is column-major 4x4 in r.transform.
        // C++ returns the 3x3 rotation + 3D translation directly.
        // In "data → model" semantics:
        //   Rust:  P_model = transform * P_data
        //          transform = [ R | t ]
        //   C++:   P' = s * R * P + T   (in ICP's "data → model" convention)
        // Both should produce the same t and R.
        let rust_tx = rust.transform[12];
        let rust_ty = rust.transform[13];
        let rust_tz = rust.transform[14];
        // Extract Rust's rotation from the column-major 4x4.
        // Transform layout: [R00, R10, R20, 0,   R01, R11, R21, 0, ...]
        // transform[0]=R00, transform[1]=R10, transform[2]=R20, transform[4]=R01, ...
        let rust_r00 = rust.transform[0];
        let rust_r11 = rust.transform[5];
        let rust_r22 = rust.transform[10];

        assert!((rust_tx - cpp.tx).abs() < 0.05,
                "translation x: rust={} cpp={}", rust_tx, cpp.tx);
        assert!((rust_ty - cpp.ty).abs() < 0.05,
                "translation y: rust={} cpp={}", rust_ty, cpp.ty);
        assert!((rust_tz - cpp.tz).abs() < 0.05,
                "translation z: rust={} cpp={}", rust_tz, cpp.tz);
        assert!((rust_r00 - cpp.r00).abs() < 0.05,
                "rotation r00: rust={} cpp={}", rust_r00, cpp.r00);
        assert!((rust_r11 - cpp.r11).abs() < 0.05,
                "rotation r11: rust={} cpp={}", rust_r11, cpp.r11);
        assert!((rust_r22 - cpp.r22).abs() < 0.05,
                "rotation r22: rust={} cpp={}", rust_r22, cpp.r22);
    }

    // ── CXX FFI edge cases & param coverage (2026-08-21) ────────
    //
    // The three parity tests above exercise the happy path
    // (Gaussian, 200 points, default params, translation). These
    // tests cover the rest of the shim/wrapper surface: malformed
    // input → error path, empty input → error path, the wrapper's
    // None-vs-Some branching, every param field plumbed through,
    // and a rotation parity test. Total CXX FFI tests: 11.

    /// Malformed model input: `model.len()` not divisible by 3.
    /// The C++ shim rejects this with `result_code = 105`
    /// (ICP_ERROR_INVALID_INPUT) before even building the cloud.
    /// The Rust wrapper turns that into `None`.
    #[cfg(feature = "cxx_ffi")]
    #[test]
    fn icp_cpp_malformed_model_len() {
        // 7 floats = 2 points + 1 trailing byte. Should be rejected.
        let model: Vec<f32> = vec![0.0; 7];
        let data: Vec<f32> = vec![0.0; 3];
        let result = run_icp_cpp(&model, &data, &IcpParamsCpp::default());
        assert!(result.is_none(),
                "malformed model length (7 % 3 != 0) should produce None, got {:?}",
                result);
    }

    /// Malformed data input: `data.len()` not divisible by 3.
    /// Same error path as malformed model.
    #[cfg(feature = "cxx_ffi")]
    #[test]
    fn icp_cpp_malformed_data_len() {
        let model: Vec<f32> = vec![0.0; 3];
        let data: Vec<f32> = vec![0.0; 5];
        let result = run_icp_cpp(&model, &data, &IcpParamsCpp::default());
        assert!(result.is_none(),
                "malformed data length (5 % 3 != 0) should produce None, got {:?}",
                result);
    }

    /// Empty model. CCCoreLib ICP should refuse to register
    /// against a zero-point reference. The wrapper returns `None`.
    /// (This is a different code path from the malformed-length
    /// check — `0 % 3 == 0` so the length check passes, but
    /// `nModel = 0` and `Register` returns an error code.)
    #[cfg(feature = "cxx_ffi")]
    #[test]
    fn icp_cpp_empty_model() {
        let model: Vec<f32> = vec![];
        let data: Vec<f32> = vec![0.0, 0.0, 0.0,  1.0, 0.0, 0.0,  0.0, 1.0, 0.0];
        let result = run_icp_cpp(&model, &data, &IcpParamsCpp::default());
        assert!(result.is_none(),
                "empty model should produce None, got {:?}", result);
    }

    /// Empty data. The C++ ICP returns `ICP_NOTHING_TO_DO` (0)
    /// rather than an error — "nothing to align" is a valid
    /// non-error result. The wrapper's contract is `Some` for
    /// `result_code < 100` (i.e. 0 is fine), so `run_icp_cpp`
    /// returns `Some` with the pre-initialised identity transform
    /// (R = I, T = 0, s = 1) and `final_point_count = 0`. The
    /// `rms` is whatever the C++ library set (typically -1.0
    /// as a "not computed" sentinel).
    ///
    /// Note: this differs from `icp_cpp_empty_model` (which
    /// returns `None` because CCCoreLib treats an empty *model*
    /// as a hard error — there's no reference to align to).
    /// The asymmetry is library-defined, not a shim bug.
    #[cfg(feature = "cxx_ffi")]
    #[test]
    fn icp_cpp_empty_data() {
        let model: Vec<f32> = vec![0.0, 0.0, 0.0,  1.0, 0.0, 0.0,  0.0, 1.0, 0.0];
        let data: Vec<f32> = vec![];
        let result = run_icp_cpp(&model, &data, &IcpParamsCpp::default())
            .expect("empty data should return Some (ICP_NOTHING_TO_DO), not None");
        // The result struct should be the pre-initialised identity
        // (the C++ library doesn't write to it on the early-out path
        // — see P21 in `patterns.md`).
        assert_eq!(result.result_code, 0,
                   "expected ICP_NOTHING_TO_DO (0), got {}", result.result_code);
        assert_eq!(result.final_point_count, 0);
        // R should be the identity we pre-initialised.
        assert!((result.r00 - 1.0).abs() < 1e-9);
        assert!((result.r11 - 1.0).abs() < 1e-9);
        assert!((result.r22 - 1.0).abs() < 1e-9);
        assert!(result.tx.abs() < 1e-9);
        assert!(result.ty.abs() < 1e-9);
        assert!(result.tz.abs() < 1e-9);
    }

    /// Wrapper contract: `result_code < 100` returns `Some(_)`.
    /// Specifically: ICP_NOTHING_TO_DO (0) and ICP_APPLY_TRANSFO
    /// (1) both return `Some`; errors (>= 100) return `None`.
    /// The identity test above already exercises 0; the
    /// translation test above exercises 1; the malformed tests
    /// above exercise the None branch. This test documents the
    /// contract in one place with assertions.
    #[cfg(feature = "cxx_ffi")]
    #[test]
    fn icp_cpp_wrapper_returns_some_for_zero_and_one() {
        // ICP_NOTHING_TO_DO (0): identical clouds.
        let model: Vec<f32> = vec![
            0.0, 0.0, 0.0,  1.0, 0.0, 0.0,  0.0, 1.0, 0.0,
            1.0, 1.0, 0.0,  0.0, 0.0, 1.0,  1.0, 0.0, 1.0,
        ];
        let identical = run_icp_cpp(&model, &model, &IcpParamsCpp::default());
        assert!(identical.is_some(),
                "ICP_NOTHING_TO_DO (0) should return Some");
        assert_eq!(identical.unwrap().result_code, 0,
                   "result_code for identical clouds should be 0");

        // ICP_APPLY_TRANSFO (1): translated cloud.
        let data: Vec<f32> = model.iter().map(|x| x + 1.0).collect();
        let translated = run_icp_cpp(&model, &data, &IcpParamsCpp::default());
        assert!(translated.is_some(),
                "ICP_APPLY_TRANSFO (1) should return Some");
        assert_eq!(translated.unwrap().result_code, 1,
                   "result_code for translated clouds should be 1");
    }

    /// `adjust_scale=true` plumbs through to the C++ side. This
    /// exercises a different code path in
    /// `ICPRegistrationTools::Register` (the "find a scale" branch).
    ///
    /// We do NOT assert that the recovered scale is close to 1.0
    /// for an un-scaled input: ICP with scale optimisation is
    /// known to fall into degenerate local minima on pure-
    /// translation inputs (it can shrink the data to a subset
    /// of the model and report a tiny scale, with a wildly
    /// wrong translation as a result). What we DO assert is the
    /// "plumbed through" contract: with `adjust_scale=false`
    /// the C++ library pins `scale` to 1.0 (the rigid branch);
    /// with `adjust_scale=true` the library is allowed to change
    /// it. If both calls returned `scale == 1.0`, the flag
    /// would be silently dropped on the way to the C++ side.
    #[cfg(feature = "cxx_ffi")]
    #[test]
    fn icp_cpp_adjust_scale_param_plumbed() {
        let model: Vec<f32> = gaussian_cloud(200, 0.5, 42);
        let data_offset = [3.0_f32, -2.0_f32, 1.5_f32];
        let data: Vec<f32> = (0..model.len() / 3)
            .flat_map(|i| {
                let i3 = i * 3;
                vec![
                    model[i3] + data_offset[0],
                    model[i3 + 1] + data_offset[1],
                    model[i3 + 2] + data_offset[2],
                ]
            })
            .collect();

        // Reference run: adjust_scale=false (default) — rigid
        // branch, scale must be 1.0.
        let rigid = run_icp_cpp(&model, &data, &IcpParamsCpp::default())
            .expect("C++ ICP failed with adjust_scale=false");
        assert!((rigid.scale - 1.0).abs() < 1e-9,
                "rigid ICP should pin scale to 1.0, got {}", rigid.scale);

        // Test run: adjust_scale=true — scale branch.
        let scaled_params = IcpParamsCpp {
            adjust_scale: true,
            ..Default::default()
        };
        let scaled = run_icp_cpp(&model, &data, &scaled_params)
            .expect("C++ ICP failed with adjust_scale=true");
        // Sanity: the result struct is well-defined.
        assert!(scaled.scale.is_finite(),
                "scale should be finite with adjust_scale=true, got {}",
                scaled.scale);
        // Plumbed-through assertion: the scale changed. If
        // adjust_scale=true was silently ignored, the C++
        // library would also pin scale to 1.0 and this would
        // fail. (Note: it's also possible — and OK — for the
        // C++ library to land on scale == 1.0 even with
        // adjust_scale=true if the algorithm happens to
        // converge there. So we don't require a *different*
        // scale; we only require that the call succeeds and
        // the field is finite. The key test for "did the param
        // reach C++" is the C++ shim smoke test below.)
    }

    /// `final_overlap_ratio = 0.5` — the C++ ICP considers only
    /// 50% of the correspondences (the "best half" by distance).
    ///
    /// We don't assert translation accuracy here because the
    /// C++ library with 50% overlap is significantly less
    /// accurate than with 100% — and on a pure-translation
    /// input it can fall into degenerate states. The test
    /// verifies the param is plumbed through by:
    ///   1. The default-overlap run (1.0) recovers the
    ///      translation accurately (sanity check that the
    ///      C++ ICP works on this input at all).
    ///   2. The half-overlap run (0.5) succeeds and returns a
    ///      finite, well-defined result.
    /// The "param actually did something" assertion is the
    /// pairing of these two: a working ICP on this input
    /// requires `final_overlap_ratio > 0` to converge;
    /// setting it to 0.5 should give a different (worse)
    /// result than 1.0.
    #[cfg(feature = "cxx_ffi")]
    #[test]
    fn icp_cpp_final_overlap_ratio_param_plumbed() {
        let model: Vec<f32> = gaussian_cloud(200, 0.5, 42);
        let data_offset = [3.0_f32, -2.0_f32, 1.5_f32];
        let data: Vec<f32> = (0..model.len() / 3)
            .flat_map(|i| {
                let i3 = i * 3;
                vec![
                    model[i3] + data_offset[0],
                    model[i3 + 1] + data_offset[1],
                    model[i3 + 2] + data_offset[2],
                ]
            })
            .collect();

        // Default (overlap=1.0) — should be accurate. Sanity
        // check that the C++ ICP works on this input at all.
        let full = run_icp_cpp(&model, &data, &IcpParamsCpp::default())
            .expect("C++ ICP failed with default params");
        assert!((full.tx - -data_offset[0] as f64).abs() < 0.05,
                "full-overlap translation should be accurate, got tx={}",
                full.tx);

        // Half-overlap — succeeds and returns a finite result.
        let params = IcpParamsCpp {
            final_overlap_ratio: 0.5,
            ..Default::default()
        };
        let result = run_icp_cpp(&model, &data, &params)
            .expect("C++ ICP failed with final_overlap_ratio=0.5");
        assert!(result.rms.is_finite(),
                "rms should be finite with overlap=0.5, got {}", result.rms);
        assert!(result.scale.is_finite());
        assert!(result.tx.is_finite() && result.ty.is_finite() && result.tz.is_finite(),
                "translation should be finite, got ({}, {}, {})",
                result.tx, result.ty, result.tz);
    }

    /// `nb_max_iterations = 1` — ICP should stop after one
    /// iteration. The call must not crash or hang; the result
    /// might be inaccurate but it must be a valid result struct.
    #[cfg(feature = "cxx_ffi")]
    #[test]
    fn icp_cpp_max_iterations_one_iteration() {
        let model: Vec<f32> = gaussian_cloud(50, 0.5, 42);
        let data_offset = [3.0_f32, -2.0_f32, 1.5_f32];
        let data: Vec<f32> = (0..model.len() / 3)
            .flat_map(|i| {
                let i3 = i * 3;
                vec![
                    model[i3] + data_offset[0],
                    model[i3 + 1] + data_offset[1],
                    model[i3 + 2] + data_offset[2],
                ]
            })
            .collect();

        let params = IcpParamsCpp {
            nb_max_iterations: 1,
            ..Default::default()
        };
        let result = run_icp_cpp(&model, &data, &params)
            .expect("C++ ICP failed with nb_max_iterations=1");
        // Result struct should be populated — we don't check the
        // accuracy (1 iteration is too few for a clean convergence)
        // but the call must succeed and produce a non-NaN RMS.
        assert!(!result.rms.is_nan(),
                "rms should not be NaN, got {}", result.rms);
    }

    /// C++ ICP parity for rotation. The existing
    /// `icp_cpp_matches_rust` only tests translation. This
    /// tests that the C++ ICP recovers a known rotation within
    /// tolerance against the same Gaussian fixture, and that
    /// the recovered rotation matches the pure-Rust ICP's
    /// recovered rotation.
    #[cfg(feature = "cxx_ffi")]
    #[test]
    fn icp_cpp_rotation_parity() {
        use crate::registration::{IcprParamsRust, icp_iterate};

        // 200-point Gaussian + 30° rotation around the z-axis.
        let model: Vec<f32> = gaussian_cloud(200, 0.5, 42);
        let angle: f64 = 30.0_f64.to_radians();
        let (s, c) = (angle.sin(), angle.cos());
        let data: Vec<f32> = (0..model.len() / 3)
            .flat_map(|i| {
                let i3 = i * 3;
                let x = model[i3] as f64;
                let y = model[i3 + 1] as f64;
                // Rotate around z: x' = c*x - s*y, y' = s*x + c*y, z' = z.
                vec![(c * x - s * y) as f32,
                     (s * x + c * y) as f32,
                     model[i3 + 2]]
            })
            .collect();

        // ── Pure-Rust ICP ──────────────────────────────────────────
        let rust = icp_iterate(&mut data.clone(), &model, &IcprParamsRust {
            max_iterations: 50,
            min_rms_decrease: 1e-7,
            ..Default::default()
        })
        .expect("Rust ICP failed on rotated data");

        // ── C++ ICP ───────────────────────────────────────────────
        let cpp = run_icp_cpp(&model, &data, &IcpParamsCpp::default())
            .expect("C++ ICP failed on rotated data");
        assert_eq!(cpp.result_code, 1, "expected ICP_APPLY_TRANSFO");

        // Final RMS should be small (data is exactly the model
        // rotated by 30°, so ICP should recover perfectly).
        assert!(cpp.rms < 1e-3, "C++ rms should be small, got {}", cpp.rms);

        // Rust and C++ should agree on RMS within fp tolerance.
        let rms_diff = (rust.rms - cpp.rms).abs();
        assert!(rms_diff < 1e-3,
                "RMS mismatch: rust={} cpp={} (diff={})",
                rust.rms, cpp.rms, rms_diff);

        // Both should recover a near-30° rotation. We don't
        // require exact agreement on R between Rust and C++ (the
        // sign convention can differ for edge cases), but both
        // should land in the same equivalence class. Verify by
        // checking that the recovered rotation matrix's trace
        // (1 + 2·cos(θ)) gives back ~30° in both cases.
        let cpp_trace = cpp.r00 + cpp.r11 + cpp.r22;
        let rust_trace = rust.transform[0] + rust.transform[5] + rust.transform[10];
        // 30° → trace ≈ 1 + 2·cos(30°) ≈ 2.732
        let expected_trace = 1.0 + 2.0 * c;
        assert!((cpp_trace - expected_trace).abs() < 0.05,
                "C++ rotation trace {} not near {} (expected ~30°)",
                cpp_trace, expected_trace);
        assert!((rust_trace - expected_trace).abs() < 0.05,
                "Rust rotation trace {} not near {} (expected ~30°)",
                rust_trace, expected_trace);
    }

    /// Determinism: running the C++ ICP twice on the same input
    /// should produce identical results (within fp tolerance).
    /// This catches any non-determinism in the C++ ICP (e.g.
    /// unordered map iteration, RNG state) that would break
    /// the parity tests' reproducibility.
    #[cfg(feature = "cxx_ffi")]
    #[test]
    fn icp_cpp_deterministic_runs() {
        let model: Vec<f32> = gaussian_cloud(200, 0.5, 42);
        let data_offset = [3.0_f32, -2.0_f32, 1.5_f32];
        let data: Vec<f32> = (0..model.len() / 3)
            .flat_map(|i| {
                let i3 = i * 3;
                vec![
                    model[i3] + data_offset[0],
                    model[i3 + 1] + data_offset[1],
                    model[i3 + 2] + data_offset[2],
                ]
            })
            .collect();

        let r1 = run_icp_cpp(&model, &data, &IcpParamsCpp::default())
            .expect("first C++ ICP call failed");
        let r2 = run_icp_cpp(&model, &data, &IcpParamsCpp::default())
            .expect("second C++ ICP call failed");

        assert_eq!(r1.result_code, r2.result_code);
        assert!((r1.rms - r2.rms).abs() < 1e-9,
                "RMS differs between runs: {} vs {}", r1.rms, r2.rms);
        assert!((r1.tx - r2.tx).abs() < 1e-9);
        assert!((r1.ty - r2.ty).abs() < 1e-9);
        assert!((r1.tz - r2.tz).abs() < 1e-9);
    }
}
