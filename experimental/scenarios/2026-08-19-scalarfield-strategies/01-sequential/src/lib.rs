//! Sequential single-threaded ScalarField statistics.
//!
//! Strategy 1 of the Phase 1 scenario. The "obvious" implementation:
//! one loop per statistic, `is_finite()` check, accumulate.
//!
//! Mirrors CCCoreLib formulas: population std, IEEE NaN filter,
//! `computeMeanSquareScalarValue` returns `Σx²/n` (not RMS itself).

use log::warn;

#[derive(Debug, Clone, Copy, PartialEq)]
pub struct Stats {
    pub mean: f64,
    pub std: f64,
    pub min: f32,
    pub max: f32,
    pub valid_count: usize,
}

pub fn mean(values: &[f32]) -> f64 {
    let mut sum = 0.0_f64;
    let mut count = 0usize;
    for &v in values {
        if v.is_finite() {
            sum += v as f64;
            count += 1;
        }
    }
    if count == 0 { f64::NAN } else { sum / count as f64 }
}

pub fn std_dev(values: &[f32], mean: f64) -> f64 {
    let mut sum_sq = 0.0_f64;
    let mut count = 0usize;
    for &v in values {
        if v.is_finite() {
            let d = v as f64 - mean;
            sum_sq += d * d;
            count += 1;
        }
    }
    if count == 0 {
        warn!("std_dev called on empty/all-invalid field");
        0.0
    } else {
        (sum_sq / count as f64).sqrt()  // population std (no Bessel)
    }
}

pub fn min_max(values: &[f32]) -> (f32, f32) {
    let mut min = f32::INFINITY;
    let mut max = f32::NEG_INFINITY;
    for &v in values {
        if v.is_finite() {
            if v < min { min = v; }
            if v > max { max = v; }
        }
    }
    if min == f32::INFINITY { (f32::NAN, f32::NAN) } else { (min, max) }
}

pub fn rms(values: &[f32]) -> f64 {
    let mut sum_sq = 0.0_f64;
    let mut count = 0usize;
    for &v in values {
        if v.is_finite() {
            sum_sq += (v as f64) * (v as f64);
            count += 1;
        }
    }
    if count == 0 { f64::NAN } else { (sum_sq / count as f64).sqrt() }
}

pub fn valid_count(values: &[f32]) -> usize {
    values.iter().filter(|&&v| v.is_finite()).count()
}

pub fn compute_all(values: &[f32]) -> Option<Stats> {
    if values.is_empty() { return None; }
    let m = mean(values);
    let s = std_dev(values, m);
    let (mn, mx) = min_max(values);
    let n = valid_count(values);
    Some(Stats { mean: m, std: s, min: mn, max: mx, valid_count: n })
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn std_population() {
        let v = vec![2.0_f32, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0];
        let m = mean(&v);
        let s = std_dev(&v, m);
        // CCCoreLib: population std → 2.0 exactly
        assert!((s - 2.0).abs() < 1e-9, "expected 2.0, got {}", s);
    }

    #[test]
    fn nan_filter() {
        let v = vec![1.0_f32, f32::NAN, 3.0, f32::INFINITY, 5.0];
        let m = mean(&v);
        assert!((m - 3.0).abs() < 1e-9, "expected 3.0, got {}", m);
    }

    #[test]
    fn rms_3_4() {
        let v = vec![3.0_f32, 4.0];
        let r = rms(&v);
        // sqrt((9+16)/2) = sqrt(12.5) ≈ 3.5355
        assert!((r - 12.5_f64.sqrt()).abs() < 1e-9);
    }
}
