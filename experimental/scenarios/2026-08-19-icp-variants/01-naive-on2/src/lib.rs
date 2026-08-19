//! ICP variant 1: naive O(n²) nearest-neighbour.
//!
//! Same algorithm as `sessions/2026-08-19-rust-migration-icp-scalarfield/src/registration.rs`
//! but isolated here so we can A/B against the other variants.
//!
//! Complexity: O(n × m) per ICP iteration, where n = data size, m = model size.
//! For 100k data points and 100k model points, this is 10^10 ops per iteration
//! — completely infeasible for real data.

use nalgebra::Vector3;

#[derive(Debug, Clone)]
pub struct IcpParams {
    pub max_iterations: u32,
    pub min_rms_decrease: f64,
}

impl Default for IcpParams {
    fn default() -> Self {
        Self { max_iterations: 50, min_rms_decrease: 1e-8 }
    }
}

#[derive(Debug, Clone)]
pub struct IcpResult {
    pub rms: f64,
    pub converged: bool,
    pub iterations: u32,
    pub transform: Vec<f64>, // 4x4 column-major
}

/// Run ICP using O(n²) brute-force nearest-neighbour.
///
/// `data_points` is modified in place: by the end it should be aligned with `model_points`.
pub fn icp_iterate(
    data_points: &mut [f32],
    model_points: &[f32],
    params: &IcpParams,
) -> Result<IcpResult, String> {
    let n_data = data_points.len() / 3;
    let n_model = model_points.len() / 3;
    if n_data < 3 || n_model < 3 {
        return Err(format!("ICP needs ≥3 points: data={}, model={}", n_data, n_model));
    }

    let (centroid_data, centroid_model) = compute_centroids(data_points, model_points);
    let h = compute_covariance(data_points, model_points, centroid_data, centroid_model);
    let rotation = compute_rotation_svd(&h)?;
    let translation = centroid_model - rotation * centroid_data;

    let mut prev_rms = f64::INFINITY;
    for iter in 0..params.max_iterations {
        let mut sum_dist_sq = 0.0;
        for i in 0..n_data {
            let idx = i * 3;
            let pt = Vector3::new(
                data_points[idx] as f64,
                data_points[idx + 1] as f64,
                data_points[idx + 2] as f64,
            );
            let transformed = rotation * pt + translation;
            let (_nn_idx, dist_sq) = nearest_neighbour_brute(model_points, &transformed);
            data_points[idx] = transformed[0] as f32;
            data_points[idx + 1] = transformed[1] as f32;
            data_points[idx + 2] = transformed[2] as f32;
            sum_dist_sq += dist_sq;
        }
        let rms = (sum_dist_sq / n_data as f64).sqrt();
        let decrease = prev_rms - rms;
        prev_rms = rms;
        log::debug!("iter {}: rms={:.6}, decrease={:.6e}", iter, rms, decrease);
        if rms < params.min_rms_decrease {
            return Ok(IcpResult { rms, converged: true, iterations: iter + 1, transform: build_transform(&rotation, &translation) });
        }
    }
    Ok(IcpResult { rms: prev_rms, converged: false, iterations: params.max_iterations, transform: build_transform(&rotation, &translation) })
}

fn compute_centroids(data: &[f32], model: &[f32]) -> (Vector3<f64>, Vector3<f64>) {
    let n_data = data.len() / 3;
    let n_model = model.len() / 3;
    let mut c_data = Vector3::zeros();
    let mut c_model = Vector3::zeros();
    for i in 0..n_data {
        c_data += Vector3::new(data[i * 3] as f64, data[i * 3 + 1] as f64, data[i * 3 + 2] as f64);
    }
    c_data /= n_data as f64;
    for i in 0..n_model {
        c_model += Vector3::new(model[i * 3] as f64, model[i * 3 + 1] as f64, model[i * 3 + 2] as f64);
    }
    c_model /= n_model as f64;
    (c_data, c_model)
}

fn compute_covariance(
    data: &[f32],
    model: &[f32],
    cd: Vector3<f64>,
    cm: Vector3<f64>,
) -> nalgebra::Matrix3<f64> {
    let n = (data.len() / 3).min(model.len() / 3);
    let mut h = nalgebra::Matrix3::<f64>::zeros();
    for i in 0..n {
        let dp = Vector3::new(data[i * 3] as f64, data[i * 3 + 1] as f64, data[i * 3 + 2] as f64) - cd;
        let mp = Vector3::new(model[i * 3] as f64, model[i * 3 + 1] as f64, model[i * 3 + 2] as f64) - cm;
        h += dp * mp.transpose();
    }
    h
}

fn compute_rotation_svd(h: &nalgebra::Matrix3<f64>) -> Result<nalgebra::Matrix3<f64>, String> {
    use nalgebra::linalg::SVD;
    let svd = SVD::new(*h, true, true);
    let v = svd.v_t.ok_or("singular: v_t is None")?;
    let u = svd.u.ok_or("singular: u is None")?;
    let mut r = v * u.transpose();
    if r.determinant() < 0.0 {
        let n_cols = r.ncols();
        for i in 0..r.nrows() {
            r[(i, n_cols - 1)] = -r[(i, n_cols - 1)];
        }
    }
    Ok(r)
}

fn build_transform(rot: &nalgebra::Matrix3<f64>, trans: &Vector3<f64>) -> Vec<f64> {
    let mut m = vec![0.0_f64; 16];
    m[0] = rot[(0,0)]; m[4] = rot[(0,1)]; m[8]  = rot[(0,2)]; m[12] = trans[0];
    m[1] = rot[(1,0)]; m[5] = rot[(1,1)]; m[9]  = rot[(1,2)]; m[13] = trans[1];
    m[2] = rot[(2,0)]; m[6] = rot[(2,1)]; m[10] = rot[(2,2)]; m[14] = trans[2];
    m[3] = 0.0;        m[7] = 0.0;        m[11] = 0.0;        m[15] = 1.0;
    m
}

/// Brute-force O(n) nearest-neighbour. Returns (index, squared_distance).
fn nearest_neighbour_brute(model: &[f32], query: &Vector3<f64>) -> (usize, f64) {
    let n = model.len() / 3;
    let mut best_idx = 0;
    let mut best_dist_sq = f64::MAX;
    for i in 0..n {
        let dx = model[i * 3] as f64 - query[0];
        let dy = model[i * 3 + 1] as f64 - query[1];
        let dz = model[i * 3 + 2] as f64 - query[2];
        let d = dx*dx + dy*dy + dz*dz;
        if d < best_dist_sq { best_dist_sq = d; best_idx = i; }
    }
    (best_idx, best_dist_sq)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn icp_identity() {
        let model: Vec<f32> = (0..300).map(|i| (i % 3) as f32).collect();
        let mut data = model.clone();
        let r = icp_iterate(&mut data, &model, &IcpParams::default()).unwrap();
        assert!(r.rms < 1e-5, "RMS should be ~0 for identical clouds, got {}", r.rms);
    }

    #[test]
    fn icp_translation() {
        // 8 cube corners, data = model + (1, 0, 0)
        let model: Vec<f32> = vec![
            0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
            1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        ];
        let mut data: Vec<f32> = (0..model.len()/3)
            .flat_map(|i| vec![model[i*3] + 1.0, model[i*3+1], model[i*3+2]])
            .collect();
        let r = icp_iterate(&mut data, &model, &IcpParams::default()).unwrap();
        assert!(r.rms < 0.1, "ICP should achieve low RMS, got {}", r.rms);
    }
}
