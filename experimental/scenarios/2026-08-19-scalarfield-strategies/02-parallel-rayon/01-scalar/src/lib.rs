//! Parallel ScalarField statistics with rayon.
//!
//! Strategy 2 of the Phase 1 scenario. Each statistic becomes a
//! `par_iter().map(...).reduce(...)` over the data, with per-chunk
//! partial sums that we sum at the end.

use log::warn;
use rayon::prelude::*;

#[derive(Debug, Clone, Copy, PartialEq)]
pub struct Stats {
    pub mean: f64,
    pub std: f64,
    pub min: f32,
    pub max: f32,
    pub valid_count: usize,
}

/// Four partial accumulators packed into a tuple, so a single reduce
/// can compute all four statistics in one pass over the data.
/// (sum, sum_sq, min, max, count) — note that min and max are `f32`
/// to match the input element type.
#[derive(Clone, Copy)]
struct Acc {
    sum: f64,
    sum_sq: f64,
    min: f32,
    max: f32,
    count: u64,
}

impl Acc {
    fn new() -> Self { Acc { sum: 0.0, sum_sq: 0.0, min: f32::INFINITY, max: f32::NEG_INFINITY, count: 0 } }
    fn add(&mut self, v: f32) {
        self.sum += v as f64;
        self.sum_sq += (v as f64) * (v as f64);
        if v < self.min { self.min = v; }
        if v > self.max { self.max = v; }
        self.count += 1;
    }
    fn merge(mut self, other: Acc) -> Acc {
        self.sum += other.sum;
        self.sum_sq += other.sum_sq;
        if other.min < self.min { self.min = other.min; }
        if other.max > self.max { self.max = other.max; }
        self.count += other.count;
        self
    }
}

/// Single parallel pass that gives us all four statistics + valid count.
pub fn compute_all_parallel(values: &[f32]) -> Stats {
    let acc = values
        .par_iter()
        .filter(|&&v| v.is_finite())
        .map(|&v| {
            let mut a = Acc::new();
            a.add(v);
            a
        })
        .reduce(Acc::new, |a, b| a.merge(b));

    if acc.count == 0 {
        warn!("compute_all_parallel called on empty/all-invalid field");
        return Stats { mean: f64::NAN, std: 0.0, min: f32::NAN, max: f32::NAN, valid_count: 0 };
    }
    let mean = acc.sum / acc.count as f64;
    // Population std: var = sum_sq/n - mean^2
    let var = (acc.sum_sq / acc.count as f64) - mean * mean;
    let std = var.max(0.0).sqrt();

    Stats {
        mean,
        std,
        min: acc.min,
        max: acc.max,
        valid_count: acc.count as usize,
    }
}

/// Standalone parallel mean (exposed for the FFI surface).
pub fn mean(values: &[f32]) -> f64 {
    if values.is_empty() { return f64::NAN; }
    let (sum, count) = values
        .par_iter()
        .filter(|&&v| v.is_finite())
        .map(|&v| (v as f64, 1u64))
        .reduce(|| (0.0, 0u64), |(sa, ca), (sb, cb)| (sa + sb, ca + cb));
    if count == 0 { f64::NAN } else { sum / count as f64 }
}

pub fn std_dev(values: &[f32], mean: f64) -> f64 {
    if values.is_empty() { return 0.0; }
    let (sum_sq, count) = values
        .par_iter()
        .filter(|&&v| v.is_finite())
        .map(|&v| {
            let d = v as f64 - mean;
            (d * d, 1u64)
        })
        .reduce(|| (0.0, 0u64), |(sa, ca), (sb, cb)| (sa + sb, ca + cb));
    if count == 0 { 0.0 } else { (sum_sq / count as f64).sqrt() }
}

pub fn min_max(values: &[f32]) -> (f32, f32) {
    if values.is_empty() { return (f32::NAN, f32::NAN); }
    values
        .par_iter()
        .filter(|&&v| v.is_finite())
        .fold(|| (f32::INFINITY, f32::NEG_INFINITY), |(mn, mx), &v| {
            (mn.min(v), mx.max(v))
        })
        .reduce(|| (f32::INFINITY, f32::NEG_INFINITY), |(mna, mxa), (mnb, mxb)| {
            (mna.min(mnb), mxa.max(mxb))
        })
}

pub fn rms(values: &[f32]) -> f64 {
    if values.is_empty() { return f64::NAN; }
    let (sum_sq, count) = values
        .par_iter()
        .filter(|&&v| v.is_finite())
        .map(|&v| ((v as f64) * (v as f64), 1u64))
        .reduce(|| (0.0, 0u64), |(sa, ca), (sb, cb)| (sa + sb, ca + cb));
    if count == 0 { f64::NAN } else { (sum_sq / count as f64).sqrt() }
}

pub fn valid_count(values: &[f32]) -> usize {
    values.par_iter().filter(|&&v| v.is_finite()).count()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn compute_all_parity() {
        let v = vec![2.0_f32, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0];
        let s = compute_all_parallel(&v);
        assert!((s.mean - 5.0).abs() < 1e-9);
        assert!((s.std - 2.0).abs() < 1e-9, "expected 2.0, got {}", s.std);
        assert_eq!(s.min, 2.0);
        assert_eq!(s.max, 9.0);
        assert_eq!(s.valid_count, 8);
    }

    #[test]
    fn nan_filter() {
        let v = vec![1.0_f32, f32::NAN, 3.0, f32::INFINITY, 5.0];
        let m = mean(&v);
        assert!((m - 3.0).abs() < 1e-9);
    }

    #[test]
    fn rms_3_4() {
        let v = vec![3.0_f32, 4.0];
        let r = rms(&v);
        assert!((r - 12.5_f64.sqrt()).abs() < 1e-9);
    }
}
