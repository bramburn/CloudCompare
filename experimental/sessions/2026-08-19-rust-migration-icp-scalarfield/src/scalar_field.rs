// src/scalar_field.rs — Phase 1: Pure-Rust ScalarField statistics
//
// These functions are PURE RUST. They have no C++ dependency.
// They are the direct migration targets from CCCoreLib::ScalarFieldTools.
//
// CloudCompare ScalarField is a vector<float> with NAN_VALUE = -1.0e-30
// representing "no data". Our Rust version must handle NaN the same way.
//
// Test against: ScalarFieldTools::computeMeanScalarValue
//               ScalarFieldTools::computeStdDev
//               ScalarFieldTools::computeMinAndMax
//               ScalarFieldTools::computeMeanDistance

use thiserror::Error;

#[derive(Debug, Clone, Copy)]
pub struct ScalarStats {
    pub mean: f64,
    pub std: f64,
    pub min: f32,
    pub max: f32,
    pub valid_count: usize,
}

#[derive(Debug, Error)]
pub enum ScalarError {
    #[error("empty field: no valid values")]
    EmptyField,
    #[error("invalid value at index {0}")]
    InvalidValue(usize),
}

// ── Core statistics ───────────────────────────────────────────────────────

/// Compute mean of valid (finite) values.
/// Mirrors: ScalarFieldTools::computeMeanScalarValue()
pub fn mean(values: &[f32]) -> f64 {
    let mut sum = 0.0_f64;
    let mut count = 0usize;

    for &v in values {
        if v.is_finite() {
            sum += v as f64;
            count += 1;
        }
    }

    if count == 0 {
        std::f64::NAN
    } else {
        sum / count as f64
    }
}

/// Compute population standard deviation (no Bessel correction).
/// Mirrors: ScalarField::computeMeanAndVariance() → sqrt(sum_sq/n - mean²)
/// This matches CCCoreLib's formula: var = E[X²] - E[X]² = (Σx²/n) − mean².
pub fn std(values: &[f32], mean: f64) -> f64 {
    let mut sum_sq = 0.0_f64;
    let mut count = 0usize;

    for &v in values {
        if v.is_finite() {
            let d = v as f64 - mean;
            sum_sq += d * d;
            count += 1;
        }
    }

    // Population std: divide by n (NOT n-1)
    if count == 0 {
        0.0
    } else {
        (sum_sq / count as f64).sqrt()
    }
}

/// Compute min and max of valid (finite) values.
/// Mirrors: ScalarFieldTools::computeMinAndMax()
pub fn min_max(values: &[f32]) -> (f32, f32) {
    let mut min = std::f32::INFINITY;
    let mut max = std::f32::NEG_INFINITY;

    for &v in values {
        if v.is_finite() {
            if v < min { min = v; }
            if v > max { max = v; }
        }
    }

    if min == std::f32::INFINITY {
        // All values were NaN or the array was empty
        (std::f32::NAN, std::f32::NAN)
    } else {
        (min, max)
    }
}

/// Count valid (finite) values.
pub fn valid_count(values: &[f32]) -> usize {
    values.iter().filter(|&&v| v.is_finite()).count()
}

/// Apply a constant offset to all valid values.
/// Mirrors: ScalarFieldTools::computeMeanDistance() post-processing
pub fn apply_offset(values: &mut [f32], offset: f64) {
    for v in values.iter_mut() {
        if v.is_finite() {
            *v = (*v as f64 + offset) as f32;
        }
    }
}

/// Apply a constant scale to all valid values.
pub fn apply_scale(values: &mut [f32], scale: f64) {
    for v in values.iter_mut() {
        if v.is_finite() {
            *v = (*v as f64 * scale) as f32;
        }
    }
}

/// Compute all scalar stats in a single pass (more efficient than calling
/// mean + std + min_max separately).
pub fn compute_stats(xyz: &[f32]) -> Option<ScalarStats> {
    // xyz is interleaved: [x0,y0,z0, x1,y1,z1, ...]
    // Extract z-coordinate as the scalar field
    let z_values: Vec<f32> = xyz.iter().skip(2).step_by(3).copied().collect();

    if z_values.is_empty() {
        return None;
    }

    let (min, max) = min_max(&z_values);
    let m = mean(&z_values);
    let s = std(&z_values, m);
    let count = valid_count(&z_values);

    Some(ScalarStats {
        mean: m,
        std: s,
        min,
        max,
        valid_count: count,
    })
}

/// Compute RMS (root mean square) of a scalar field.
/// Used for ICP convergence testing.
pub fn rms(values: &[f32]) -> f64 {
    let mut sum_sq = 0.0_f64;
    let mut count = 0usize;

    for &v in values {
        if v.is_finite() {
            sum_sq += (v as f64) * (v as f64);
            count += 1;
        }
    }

    if count == 0 {
        std::f64::NAN
    } else {
        (sum_sq / count as f64).sqrt()
    }
}

/// Compare two scalar fields element-wise with a tolerance.
/// Returns true if all finite values match within tolerance.
/// Mirrors: ScalarFieldTools::compareScalarFields()
pub fn compare(values_a: &[f32], values_b: &[f32], tolerance: f64) -> bool {
    if values_a.len() != values_b.len() {
        return false;
    }

    for (&a, &b) in values_a.iter().zip(values_b.iter()) {
        let a_valid = a.is_finite();
        let b_valid = b.is_finite();

        if a_valid && b_valid {
            if (a as f64 - b as f64).abs() > tolerance {
                return false;
            }
        } else if a_valid != b_valid {
            // One valid, one not — mismatch
            return false;
        }
        // Both invalid — continue
    }

    true
}

// ── Tests ─────────────────────────────────────────────────────────────────

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_mean_basic() {
        let values = vec![1.0_f32, 2.0, 3.0, 4.0, 5.0];
        assert!((mean(&values) - 3.0).abs() < 1e-9);
    }

    #[test]
    fn test_mean_with_nan() {
        let values = vec![1.0_f32, std::f32::NAN, 3.0, std::f32::INFINITY, 5.0];
        let m = mean(&values);
        assert!((m - 3.0).abs() < 1e-9);  // (1+3+5)/3 = 3
    }

    #[test]
    fn test_mean_empty() {
        let values: Vec<f32> = vec![];
        assert!(mean(&values).is_nan());
    }

    #[test]
    fn test_mean_all_invalid() {
        let values = vec![std::f32::NAN, std::f32::INFINITY, std::f32::NEG_INFINITY];
        assert!(mean(&values).is_nan());
    }

    #[test]
    fn test_std_basic() {
        let values = vec![2.0_f32, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0];
        let m = mean(&values);
        let s = std(&values, m);
        // Known std dev = 2.0
        assert!((s - 2.0).abs() < 1e-9);
    }

    #[test]
    fn test_std_single_value() {
        let values = vec![42.0_f32];
        assert_eq!(std(&values, 42.0), 0.0);
    }

    #[test]
    fn test_min_max_basic() {
        let values = vec![5.0_f32, 2.0, 9.0, 1.0, 7.0];
        let (min, max) = min_max(&values);
        assert_eq!(min, 1.0);
        assert_eq!(max, 9.0);
    }

    #[test]
    fn test_min_max_with_nan() {
        let values = vec![std::f32::NAN, 3.0, std::f32::NAN, 1.0];
        let (min, max) = min_max(&values);
        assert_eq!(min, 1.0);
        assert_eq!(max, 3.0);
    }

    #[test]
    fn test_valid_count() {
        let values = vec![1.0_f32, std::f32::NAN, 3.0, std::f32::INFINITY, 5.0];
        assert_eq!(valid_count(&values), 3);
    }

    #[test]
    fn test_apply_offset() {
        let mut values = vec![1.0_f32, 2.0, 3.0];
        apply_offset(&mut values, 10.0);
        assert_eq!(values, vec![11.0, 12.0, 13.0]);
    }

    #[test]
    fn test_apply_offset_ignores_nan() {
        let mut values = vec![1.0_f32, std::f32::NAN, 3.0];
        apply_offset(&mut values, 10.0);
        assert_eq!(values[0], 11.0);
        assert!(values[1].is_nan());
        assert_eq!(values[2], 13.0);
    }

    #[test]
    fn test_compare_parity() {
        let a = vec![1.0_f32, 2.0, 3.0, std::f32::NAN, 5.0];
        let b = vec![1.001_f32, 2.0, 3.0, std::f32::NAN, 5.0];
        assert!(compare(&a, &b, 0.01));   // within tol
        assert!(!compare(&a, &b, 0.001)); // outside tol
    }

    #[test]
    fn test_rms() {
        let values = vec![3.0_f32, 4.0]; // RMS = sqrt((9+16)/2) = sqrt(12.5) ≈ 3.536
        let r = rms(&values);
        assert!((r - 12.5_f64.sqrt()).abs() < 1e-9);
    }

    #[test]
    fn test_compute_stats_z_from_xyz() {
        // Interleaved xyz: (0,0,1), (1,1,2), (2,2,3)
        let xyz = vec![0.0_f32, 0.0, 1.0,  1.0, 1.0, 2.0,  2.0, 2.0, 3.0];
        let stats = compute_stats(&xyz).unwrap();
        assert_eq!(stats.valid_count, 3);
        assert!((stats.mean - 2.0).abs() < 1e-9);  // (1+2+3)/3
        assert!((stats.min - 1.0).abs() < 1e-9);
        assert!((stats.max - 3.0).abs() < 1e-9);
    }

    // ── Characterisation tests (against known C++ results) ─────────────
    // These values must match CCCoreLib::ScalarFieldTools exactly.
    // Run: cargo test --test test_scalar_field_parity

    const PARITY_TOLERANCE: f64 = 1e-6;

    #[test]
    fn parity_mean_known() {
        // CloudCompare test data: [1.0, 2.0, 3.0, 4.0, 5.0] → mean = 3.0
        let values = vec![1.0_f32, 2.0, 3.0, 4.0, 5.0];
        let m = mean(&values);
        assert!((m - 3.0).abs() < PARITY_TOLERANCE);
    }

    #[test]
    fn parity_mean_with_nan_cloudcompare() {
        // C++ ScalarField uses NAN_VALUE = std::numeric_limits<float>::quiet_NaN()
        // ValidValue() = std::isfinite() — so NaN (not -1.0e-30) is the invalid marker.
        // Mean of [1.0, NaN, 3.0] = (1+3)/2 = 2.0 (NaN is skipped)
        let values = vec![
            1.0_f32,
            f32::NAN,       // NAN_VALUE marker (std::isfinite returns false)
            3.0,
        ];
        let m = mean(&values);
        assert!((m - 2.0).abs() < PARITY_TOLERANCE);  // (1+3)/2 = 2
    }
}
