//! Benchmarks for cc_rust's ICP registration.

use criterion::{criterion_group, criterion_main, BenchmarkId, Criterion, Throughput};
use cc_rust::registration::{icp_iterate, IcprParams};

fn make_cloud(n: usize, sigma: f32, seed: u64) -> Vec<f32> {
    let mut state = seed;
    let mut next_u = || -> f32 {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        (state as f64 / u64::MAX as f64) as f32
    };
    let mut next_normal = || -> f32 {
        let u1 = next_u().max(1e-6);
        let u2 = next_u();
        let r = (-2.0 * u1.ln()).sqrt();
        let theta = 2.0_f32 * std::f32::consts::PI * u2;
        r * theta.cos() * sigma
    };
    let mut out = Vec::with_capacity(n * 3);
    for _ in 0..n {
        out.push(next_normal());
        out.push(next_normal());
        out.push(next_normal());
    }
    out
}

fn bench_icp(c: &mut Criterion) {
    let mut group = c.benchmark_group("cc_rust_icp");
    for &n in &[100usize, 1_000, 5_000, 10_000] {
        group.throughput(Throughput::Elements(n as u64));
        group.bench_with_input(BenchmarkId::from_parameter(n), &n, |b, &n| {
            b.iter(|| {
                let model = make_cloud(n, 0.4, 42);
                let mut data: Vec<f32> = (0..n)
                    .flat_map(|i| vec![model[i * 3] + 0.5, model[i * 3 + 1], model[i * 3 + 2]])
                    .collect();
                let params = IcprParams { max_iterations: 10, min_rms_decrease: 1e-8 };
                let r = icp_iterate(&mut data, &model, &params).unwrap();
                std::hint::black_box(r);
            });
        });
    }
    group.finish();
}

criterion_group!(benches, bench_icp);
criterion_main!(benches);
