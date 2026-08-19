//! Benchmarks: sequential ScalarField.

use criterion::{criterion_group, criterion_main, BenchmarkId, Criterion, Throughput};
use scalarfield_v1_sequential::{mean, min_max, rms, std_dev};

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
    let mut group = c.benchmark_group("scalar_v1_sequential");
    for &n in &[1_000usize, 10_000, 100_000, 1_000_000] {
        let v = make_values(n);
        group.throughput(Throughput::Elements(n as u64));
        group.bench_with_input(BenchmarkId::from_parameter(n), &v, |b, v| {
            b.iter(|| {
                let m = mean(v);
                let s = std_dev(v, m);
                let r = rms(v);
                let mm = min_max(v);
                std::hint::black_box((m, s, r, mm));
            });
        });
    }
    group.finish();
}

criterion_group!(benches, bench_all);
criterion_main!(benches);
