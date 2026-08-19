//! ScalarField statistics with portable SIMD (Rust 1.78+ `std::simd`).
//!
//! Strategy 3 of the Phase 1 scenario. Uses `std::simd::Simd<f32, 8>`
//! (one AVX2 register's worth of f32s) to process 8 elements at a time.
//! Falls back to scalar for the tail and for the `is_finite` check
//! (which currently isn't vectorised in stable std::simd).
//!
//! Why not AVX2 directly? `std::simd` is the portable path — same
//! code runs on x86_64, aarch64, wasm-simd. We get the SIMD speedup
//! everywhere with no `#[cfg(target_arch)]` clutter.

#![feature(portable_simd)]
use std::simd::Simd;
use std::simd::num::SimdFloat;
use log::warn;

const LANES: usize = 8;

#[derive(Debug, Clone, Copy, PartialEq)]
pub struct Stats {
    pub mean: f64,
    pub std: f64,
    pub min: f32,
    pub max: f32,
    pub valid_count: usize,
}

/// Process 8 f32 at a time, sum into f64 accumulator to preserve precision.
/// `is_finite` is per-element; non-finite lanes are masked out.
fn simd_sum_and_count(values: &[f32]) -> (f64, u64) {
    let mut sum = 0.0_f64;
    let mut count: u64 = 0;

    let chunks = values.chunks_exact(LANES);
    let remainder = chunks.remainder();

    for chunk in chunks {
        let v: Simd<f32, LANES> = Simd::from_slice(chunk);
        // For f32: NaN comparison is false, so `is_finite_simd` does what we want.
        let finite_mask = v.is_finite();
        // Cast to f64 and mask: for masked-out lanes, multiply by 0.
        let v_f64: Simd<f64, LANES> = v.cast();
        let zero: Simd<f64, LANES> = Simd::splat(0.0);
        let masked = v_f64.select(finite_mask, v_f64) * zero.select(finite_mask, v_f64.select(finite_mask, Simd::splat(1.0)));
        // Above is a bit awkward; simpler:
        // let masked = v_f64 * Simd::splat(1.0).select(finite_mask, zero);
        // For clarity use a clean select:
        let clean = v_f64.select(finite_mask, zero);
        sum += clean.reduce_sum();
        // Count finite lanes
        let ones_i64: Simd<i64, LANES> = finite_mask.to_int();
        count += ones_i64.reduce_sum() as u64;
    }

    // Scalar tail + filter (handles non-multiple-of-LANES)
    for &v in remainder {
        if v.is_finite() {
            sum += v as f64;
            count += 1;
        }
    }
    (sum, count)
}

pub fn mean(values: &[f32]) -> f64 {
    if values.is_empty() { return f64::NAN; }
    let (sum, count) = simd_sum_and_count(values);
    if count == 0 { f64::NAN } else { sum / count as f64 }
}

pub fn std_dev(values: &[f32], mean: f64) -> f64 {
    if values.is_empty() { return 0.0; }
    // For std, we need sum of (v - mean)^2
    let mut sum_sq = 0.0_f64;
    let mut count: u64 = 0;

    let chunks = values.chunks_exact(LANES);
    let remainder = chunks.remainder();

    for chunk in chunks {
        let v: Simd<f32, LANES> = Simd::from_slice(chunk);
        let finite_mask = v.is_finite();
        // (v - mean)^2 — note we cast to f64 early for accuracy
        let v_f64: Simd<f64, LANES> = v.cast();
        let diff = v_f64 - Simd::splat(mean);
        let sq = diff * diff;
        let clean = sq.select(finite_mask, Simd::splat(0.0));
        sum_sq += clean.reduce_sum();
        let ones_i64: Simd<i64, LANES> = finite_mask.to_int();
        count += ones_i64.reduce_sum() as u64;
    }
    for &v in remainder {
        if v.is_finite() {
            let d = v as f64 - mean;
            sum_sq += d * d;
            count += 1;
        }
    }
    if count == 0 { 0.0 } else { (sum_sq / count as f64).sqrt() }
}

pub fn min_max(values: &[f32]) -> (f32, f32) {
    if values.is_empty() { return (f32::NAN, f32::NAN); }
    let mut min_v = f32::INFINITY;
    let mut max_v = f32::NEG_INFINITY;

    let chunks = values.chunks_exact(LANES);
    let remainder = chunks.remainder();

    for chunk in chunks {
        let v: Simd<f32, LANES> = Simd::from_slice(chunk);
        let finite_mask = v.is_finite();
        // Use +INF for masked-out so they don't affect min, -INF for max
        let pos_inf: Simd<f32, LANES> = Simd::splat(f32::INFINITY);
        let neg_inf: Simd<f32, LANES> = Simd::splat(f32::NEG_INFINITY);
        let v_for_min = v.select(finite_mask, pos_inf);
        let v_for_max = v.select(finite_mask, neg_inf);
        let chunk_min = v_for_min.reduce_min();
        let chunk_max = v_for_max.reduce_max();
        if chunk_min < min_v { min_v = chunk_min; }
        if chunk_max > max_v { max_v = chunk_max; }
    }
    for &v in remainder {
        if v.is_finite() {
            if v < min_v { min_v = v; }
            if v > max_v { max_v = v; }
        }
    }
    if min_v == f32::INFINITY { (f32::NAN, f32::NAN) } else { (min_v, max_v) }
}

pub fn rms(values: &[f32]) -> f64 {
    if values.is_empty() { return f64::NAN; }
    let (sum_sq, count) = {
        let mut sum_sq = 0.0_f64;
        let mut count: u64 = 0;
        let chunks = values.chunks_exact(LANES);
        let remainder = chunks.remainder();
        for chunk in chunks {
            let v: Simd<f32, LANES> = Simd::from_slice(chunk);
            let finite_mask = v.is_finite();
            let v_f64: Simd<f64, LANES> = v.cast();
            let sq = v_f64 * v_f64;
            let clean = sq.select(finite_mask, Simd::splat(0.0));
            sum_sq += clean.reduce_sum();
            let ones_i64: Simd<i64, LANES> = finite_mask.to_int();
            count += ones_i64.reduce_sum() as u64;
        }
        for &v in remainder {
            if v.is_finite() {
                sum_sq += (v as f64) * (v as f64);
                count += 1;
            }
        }
        (sum_sq, count)
    };
    if count == 0 { f64::NAN } else { (sum_sq / count as f64).sqrt() }
}

pub fn valid_count(values: &[f32]) -> usize {
    if values.is_empty() { return 0; }
    let mut count: u64 = 0;
    let chunks = values.chunks_exact(LANES);
    let remainder = chunks.remainder();
    for chunk in chunks {
        let v: Simd<f32, LANES> = Simd::from_slice(chunk);
        let finite_mask = v.is_finite();
        let ones_i64: Simd<i64, LANES> = finite_mask.to_int();
        count += ones_i64.reduce_sum() as u64;
    }
    for &v in remainder {
        if v.is_finite() {
            count += 1;
        }
    }
    count as usize
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
        assert!((s - 2.0).abs() < 1e-9, "expected 2.0, got {}", s);
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
