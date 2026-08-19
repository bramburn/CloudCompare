//! Coarse pre-alignment for ICP.
//!
//! When the initial rotation between data and model is large
//! (e.g. 30°+), full ICP can take many iterations to converge or
//! get stuck in a local minimum. A cheap pre-alignment that
//! matches the **principal axes** of the two clouds gives ICP a
//! much better starting point.
//!
//! Algorithm:
//! 1. Compute centroid of each cloud.
//! 2. Centre each cloud.
//! 3. Compute the 3×3 covariance matrix C_data and C_model of
//!    the centred clouds.
//! 4. Compute eigenvectors of each covariance (PCA).
//! 5. Align data's principal axes to model's via the same SVD
//!    Horn formula used by ICP, but applied to the eigenvectors
//!    rather than NN pairs.
//! 6. Compose the result with the centroid-to-centroid
//!    translation.
//!
//! The output is a single 4×4 transform that takes data to the
//! model's frame. Call it once, then run ICP from the
//! transformed data.
//!
//! This is the same idea as "PCA pre-alignment" used by
//! RealSense, Azure Kinect, and most modern SLAM pipelines. It's
//! O(n) to build (one pass over the data) and O(1) to apply.

use nalgebra::{Matrix3, Vector3};

/// Result of coarse pre-alignment.
#[derive(Debug, Clone)]
pub struct CoarseAlignResult {
    /// 4×4 column-major transformation matrix.
    pub transform: Vec<f64>,
    /// Centroid of the data cloud (before alignment).
    pub centroid_data: [f64; 3],
    /// Centroid of the model cloud.
    pub centroid_model: [f64; 3],
}

/// Compute centroids of two point clouds.
fn compute_centroids(data: &[f32], model: &[f32]) -> (Vector3<f64>, Vector3<f64>) {
    let n_data = data.len() / 3;
    let n_model = model.len() / 3;
    let mut c_data = Vector3::zeros();
    let mut c_model = Vector3::zeros();
    for i in 0..n_data {
        c_data += Vector3::new(
            data[i * 3] as f64,
            data[i * 3 + 1] as f64,
            data[i * 3 + 2] as f64,
        );
    }
    for i in 0..n_model {
        c_model += Vector3::new(
            model[i * 3] as f64,
            model[i * 3 + 1] as f64,
            model[i * 3 + 2] as f64,
        );
    }
    (c_data / n_data as f64, c_model / n_model as f64)
}

/// Compute the 3×3 covariance of a centred cloud.
fn covariance(p: &[f32], centroid: Vector3<f64>) -> Matrix3<f64> {
    let n = p.len() / 3;
    let mut c = Matrix3::zeros();
    for i in 0..n {
        let v = Vector3::new(
            p[i * 3] as f64,
            p[i * 3 + 1] as f64,
            p[i * 3 + 2] as f64,
        ) - centroid;
        c += v * v.transpose();
    }
    c
}

/// Compute the principal axes (columns are unit eigenvectors) of
/// a symmetric 3×3 matrix, ordered by descending eigenvalue.
fn principal_axes(c: Matrix3<f64>) -> Matrix3<f64> {
    use nalgebra::linalg::SymmetricEigen;
    let eigen = SymmetricEigen::new(c);
    // SymmetricEigen returns eigenvalues in ascending order;
    // we want descending. Reverse the order of columns.
    let mut vectors = eigen.eigenvectors;
    let n = vectors.ncols();
    for i in 0..n / 2 {
        vectors.swap_columns(i, n - 1 - i);
    }
    vectors
}

/// Compute the rotation that takes data's principal axes to
/// model's. This is the same SVD-Horn formula as ICP, but applied
/// to eigenvectors rather than NN pairs.
fn align_axes(axes_data: &Matrix3<f64>, axes_model: &Matrix3<f64>) -> Matrix3<f64> {
    use nalgebra::linalg::SVD;
    // H = axes_data * axes_model^T (3×3 cross-covariance of axes).
    let h = axes_data * axes_model.transpose();
    let svd = SVD::new(h, true, true);
    let v_t = svd.v_t.expect("SVD v_t");
    let u = svd.u.expect("SVD u");
    // R = V * U^T. nalgebra stores v_t = V^T, so R = v_t^T * u^T.
    let mut r = v_t.transpose() * u.transpose();
    // Enforce det(R) = +1.
    if r.determinant() < 0.0 {
        let n_cols = r.ncols();
        for i in 0..r.nrows() {
            r[(i, n_cols - 1)] = -r[(i, n_cols - 1)];
        }
    }
    r
}

/// Run coarse pre-alignment. Returns the transform that takes
/// `data` to the model's frame.
pub fn coarse_align(data: &[f32], model: &[f32]) -> CoarseAlignResult {
    let (c_data, c_model) = compute_centroids(data, model);
    let cov_data = covariance(data, c_data);
    let cov_model = covariance(model, c_model);
    let axes_data = principal_axes(cov_data);
    let axes_model = principal_axes(cov_model);
    let rotation = align_axes(&axes_data, &axes_model);

    // Translation: after rotation, c_data is rotated, so we need
    // t = c_model - R * c_data.
    let translation = c_model - rotation * c_data;

    // 4×4 column-major matrix.
    let mut m = vec![0.0_f64; 16];
    m[0] = rotation[(0, 0)]; m[1] = rotation[(1, 0)]; m[2] = rotation[(2, 0)]; m[3] = 0.0;
    m[4] = rotation[(0, 1)]; m[5] = rotation[(1, 1)]; m[6] = rotation[(2, 1)]; m[7] = 0.0;
    m[8] = rotation[(0, 2)]; m[9] = rotation[(1, 2)]; m[10] = rotation[(2, 2)]; m[11] = 0.0;
    m[12] = translation[0]; m[13] = translation[1]; m[14] = translation[2]; m[15] = 1.0;

    CoarseAlignResult {
        transform: m,
        centroid_data: [c_data[0], c_data[1], c_data[2]],
        centroid_model: [c_model[0], c_model[1], c_model[2]],
    }
}

/// Apply a 4×4 column-major transform to a point cloud in place.
pub fn apply_transform_in_place(points: &mut [f32], transform: &[f64; 16]) {
    for i in 0..points.len() / 3 {
        let x = points[i * 3] as f64;
        let y = points[i * 3 + 1] as f64;
        let z = points[i * 3 + 2] as f64;
        let new_x = transform[0] * x + transform[4] * y + transform[8] * z + transform[12];
        let new_y = transform[1] * x + transform[5] * y + transform[9] * z + transform[13];
        let new_z = transform[2] * x + transform[6] * y + transform[10] * z + transform[14];
        points[i * 3] = new_x as f32;
        points[i * 3 + 1] = new_y as f32;
        points[i * 3 + 2] = new_z as f32;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn asymmetric_cloud() -> Vec<f32> {
        let mut v = vec![
            0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
            1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        ];
        v.extend_from_slice(&[1.5, 0.3, 0.7]);
        v
    }

    #[test]
    fn coarse_align_recenters_clouds() {
        // Data is the model translated by (1, 2, 3). Coarse
        // alignment should at least correct the centroid.
        let model = asymmetric_cloud();
        let mut data = model.clone();
        for i in 0..data.len() / 3 {
            data[i * 3] += 1.0;
            data[i * 3 + 1] += 2.0;
            data[i * 3 + 2] += 3.0;
        }
        let r = coarse_align(&data, &model);
        // After applying r.transform, the data centroid should
        // match the model centroid.
        let mut aligned = data.clone();
        let mut t = [0.0_f64; 16];
        t.copy_from_slice(&r.transform);
        apply_transform_in_place(&mut aligned, &t);
        let c = {
            let mut cx = 0.0;
            let mut cy = 0.0;
            let mut cz = 0.0;
            for i in 0..aligned.len() / 3 {
                cx += aligned[i * 3] as f64;
                cy += aligned[i * 3 + 1] as f64;
                cz += aligned[i * 3 + 2] as f64;
            }
            (cx, cy, cz)
        };
        let n = (aligned.len() / 3) as f64;
        let ac = (c.0 / n, c.1 / n, c.2 / n);
        let mc = r.centroid_model;
        // Centroid should be close to model centroid.
        let dx = ac.0 - mc[0];
        let dy = ac.1 - mc[1];
        let dz = ac.2 - mc[2];
        assert!((dx * dx + dy * dy + dz * dz).sqrt() < 0.05,
                "centroid should match: aligned centroid={:?}, model centroid={:?}",
                ac, mc);
    }

    #[test]
    fn coarse_align_handles_rotation() {
        // Apply a 30° rotation around Z to an L-shaped cloud,
        // then coarse-align. PCA pre-alignment has known
        // limitations on near-symmetric clouds (degenerate
        // principal axes), so we use a deliberately asymmetric
        // cloud.
        let model = vec![
            // An "L" stretched along X with arms in Y and Z.
            // X varies a lot, Y a little, Z a little — distinct
            // eigenvalues, so the principal axes are well-defined.
            0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            2.0, 0.0, 0.0,
            3.0, 0.0, 0.0,
            4.0, 0.0, 0.0,
            5.0, 0.0, 0.0,
            5.0, 1.0, 0.0,
            5.0, 2.0, 0.0,
            5.0, 3.0, 0.0,
            5.0, 0.0, 1.0,
            5.0, 0.0, 2.0,
            5.0, 0.0, 3.0,
        ];
        let c = std::f32::consts::PI / 6.0; // 30 degrees
        let cos = c.cos();
        let sin = c.sin();
        let mut data = model.clone();
        for i in 0..data.len() / 3 {
            let x = data[i * 3];
            let y = data[i * 3 + 1];
            data[i * 3] = cos * x - sin * y;
            data[i * 3 + 1] = sin * x + cos * y;
        }
        let r = coarse_align(&data, &model);
        let mut aligned = data.clone();
        let mut t = [0.0_f64; 16];
        t.copy_from_slice(&r.transform);
        apply_transform_in_place(&mut aligned, &t);

        // Compute RMS to model.
        let n = (model.len() / 3) as f64;
        let mut sum_d2 = 0.0;
        for i in 0..model.len() / 3 {
            let dx = aligned[i * 3] as f64 - model[i * 3] as f64;
            let dy = aligned[i * 3 + 1] as f64 - model[i * 3 + 1] as f64;
            let dz = aligned[i * 3 + 2] as f64 - model[i * 3 + 2] as f64;
            sum_d2 += dx * dx + dy * dy + dz * dz;
        }
        let rms = (sum_d2 / n).sqrt();
        // With distinct eigenvalues, PCA should recover the
        // rotation to within numerical precision. RMS < 0.05
        // for a unit-cube L is realistic.
        assert!(rms < 0.05, "coarse alignment RMS too high: {}", rms);
    }
}
