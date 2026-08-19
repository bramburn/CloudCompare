//! Benchmarks for the ScalarField statistics functions.
//!
//! Run with: `cargo bench --bench scalar_field_bench`
//! Reports land in `target/criterion/`.

use criterion::{criterion_group, criterion_main, BenchmarkId, Criterion, Throughput};
use cc_sandbox::scalar_field::{mean, min_max, rms, std as scalar_std};

/// Generate a deterministic but realistic test vector.
/// Mix of "inlier" range + a few NaN sentinels to exercise the filter.
fn make_values(n: usize, with_nans: bool) -> Vec<f32> {
    let mut state: u64 = 0xDEAD_BEEF_CAFE_BABE;
    let mut next = || -> f32 {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        (state as f64 / u64::MAX as f64) as f32
    };
    let mut out = Vec::with_capacity(n);
    for i in 0..n {
        // Realistic-looking: points clustered around 0 with std ~1
        let r = next();
        let z = r * 3.0 + (i as f32 * 0.001).sin();
        out.push(z);
    }
    if with_nans {
        // Sprinkle ~1% NaN at regular intervals
        let step = (n / 100).max(1);
        for i in (0..n).step_by(step) {
            out[i] = f32::NAN;
        }
    }
    out
}

fn bench_mean(c: &mut Criterion) {
    let mut group = c.benchmark_group("mean");
    for &n in &[1_000usize, 10_000, 100_000, 1_000_000] {
        let v = make_values(n, false);
        group.throughput(Throughput::Elements(n as u64));
        group.bench_with_input(BenchmarkId::from_parameter(n), &v, |b, v| {
            b.iter(|| {
                let m = mean(v);
                std::hint::black_box(m);
            });
        });
    }
    group.finish();
}

fn bench_std(c: &mut Criterion) {
    let mut group = c.benchmark_group("std");
    for &n in &[1_000usize, 10_000, 100_000, 1_000_000] {
        let v = make_values(n, false);
        let m = mean(&v);
        group.throughput(Throughput::Elements(n as u64));
        group.bench_with_input(BenchmarkId::from_parameter(n), &v, |b, v| {
            b.iter(|| {
                let s = scalar_std(v, m);
                std::hint::black_box(s);
            });
        });
    }
    group.finish();
}

fn bench_min_max(c: &mut Criterion) {
    let mut group = c.benchmark_group("min_max");
    for &n in &[1_000usize, 10_000, 100_000, 1_000_000] {
        let v = make_values(n, false);
        group.throughput(Throughput::Elements(n as u64));
        group.bench_with_input(BenchmarkId::from_parameter(n), &v, |b, v| {
            b.iter(|| {
                let r = min_max(v);
                std::hint::black_box(r);
            });
        });
    }
    group.finish();
}

fn bench_rms(c: &mut Criterion) {
    let mut group = c.benchmark_group("rms");
    for &n in &[1_000usize, 10_000, 100_000, 1_000_000] {
        let v = make_values(n, false);
        group.throughput(Throughput::Elements(n as u64));
        group.bench_with_input(BenchmarkId::from_parameter(n), &v, |b, v| {
            b.iter(|| {
                let r = rms(v);
                std::hint::black_box(r);
            });
        });
    }
    group.finish();
}

/// How much does the NaN-filter path cost vs. the all-valid path?
fn bench_nan_filter_cost(c: &mut Criterion) {
    let mut group = c.benchmark_group("nan_filter_cost");
    let n = 100_000;
    let clean = make_values(n, false);
    let dirty = make_values(n, true);
    group.throughput(Throughput::Elements(n as u64));
    group.bench_function("clean", |b| {
        b.iter(|| {
            let m = mean(&clean);
            std::hint::black_box(m);
        });
    });
    group.bench_function("1%_nan", |b| {
        b.iter(|| {
            let m = mean(&dirty);
            std::hint::black_box(m);
        });
    });
    group.finish();
}

criterion_group!(benches, bench_mean, bench_std, bench_min_max, bench_rms, bench_nan_filter_cost);
criterion_main!(benches);
