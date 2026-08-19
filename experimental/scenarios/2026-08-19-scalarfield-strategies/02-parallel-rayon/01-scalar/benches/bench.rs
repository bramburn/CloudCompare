//! Benchmarks: parallel ScalarField with rayon.

use criterion::{criterion_group, criterion_main, BenchmarkId, Criterion, Throughput};
use scalarfield_v2_rayon::{compute_all_parallel, mean, min_max, rms, std_dev};

fn make_values(n: usize) -> Vec<f32> {
    let mut state: u64 = 0xDEAD_BEEF_CAFE_BABE;
    let mut out = Vec::with_capacity(n);
    for i in 0..n {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        let r = (state as f64 / u64::MAX as f64) as f32;
        out.push(r * 3.0 + (i as f32 * 0.001).sin());
    }
    out
}

fn bench_all(c: &mut Criterion) {
    let mut group = c.benchmark_group("scalar_v2_rayon");
    for &n in &[1_000usize, 10_000, 100_000, 1_000_000] {
        let v = make_values(n);
        group.throughput(Throughput::Elements(n as u64));
        group.bench_with_input(BenchmarkId::from_parameter(n), &v, |b, v| {
            b.iter(|| {
                let s = compute_all_parallel(v);
                std::hint::black_box(s);
            });
        });
    }
    group.finish();
}

fn bench_standalone(c: &mut Criterion) {
    let mut group = c.benchmark_group("scalar_v2_rayon_standalone");
    let n = 1_000_000;
    let v = make_values(n);
    group.throughput(Throughput::Elements(n as u64));
    group.bench_function("mean_1M", |b| {
        b.iter(|| {
            let m = mean(&v);
            std::hint::black_box(m);
        });
    });
    group.bench_function("min_max_1M", |b| {
        b.iter(|| {
            let r = min_max(&v);
            std::hint::black_box(r);
        });
    });
    group.bench_function("rms_1M", |b| {
        b.iter(|| {
            let r = rms(&v);
            std::hint::black_box(r);
        });
    });
    let m = mean(&v);
    group.bench_function("std_1M", |b| {
        b.iter(|| {
            let s = std_dev(&v, m);
            std::hint::black_box(s);
        });
    });
    group.finish();
}

criterion_group!(benches, bench_all, bench_standalone);
criterion_main!(benches);
