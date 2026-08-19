//! Benchmarks for cc_rust's ScalarField statistics.

use criterion::{criterion_group, criterion_main, BenchmarkId, Criterion, Throughput};
use cc_rust::scalar_field::{mean, min_max, rms, std_dev};

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
        let r = next();
        out.push(r * 3.0 + (i as f32 * 0.001).sin());
    }
    if with_nans {
        let step = (n / 100).max(1);
        for i in (0..n).step_by(step) {
            out[i] = f32::NAN;
        }
    }
    out
}

fn bench_mean(c: &mut Criterion) {
    let mut group = c.benchmark_group("cc_rust_mean");
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
    let mut group = c.benchmark_group("cc_rust_std");
    for &n in &[1_000usize, 10_000, 100_000, 1_000_000] {
        let v = make_values(n, false);
        let m = mean(&v);
        group.throughput(Throughput::Elements(n as u64));
        group.bench_with_input(BenchmarkId::from_parameter(n), &v, |b, v| {
            b.iter(|| {
                let s = std_dev(v, m);
                std::hint::black_box(s);
            });
        });
    }
    group.finish();
}

criterion_group!(benches, bench_mean, bench_std);
criterion_main!(benches);
