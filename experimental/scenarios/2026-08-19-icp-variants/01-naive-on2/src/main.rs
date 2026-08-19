//! ICP variant 1 — naive O(n²) — CLI benchmark.

use icp_v1_naive::{icp_iterate, IcpParams};

fn main() {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();

    let args: Vec<String> = std::env::args().skip(1).collect();
    let n: usize = args.get(0).and_then(|s| s.parse().ok()).unwrap_or(2000);
    let seed: u64 = args.get(1).and_then(|s| s.parse().ok()).unwrap_or(42);

    // Generate a model cloud: random Gaussian blob
    let model = generate_gaussian(n, 0.4, seed);
    // Generate data = model translated by (0.5, 0, 0)
    let mut data: Vec<f32> = (0..n).flat_map(|i| vec![model[i*3] + 0.5, model[i*3+1], model[i*3+2]]).collect();

    eprintln!("Naive O(n²) ICP: {} points", n);
    eprintln!("Building (no tree needed)...");

    let start = std::time::Instant::now();
    let result = icp_iterate(&mut data, &model, &IcpParams::default()).unwrap();
    let elapsed = start.elapsed();

    println!("iterations  = {}", result.iterations);
    println!("converged   = {}", result.converged);
    println!("final rms   = {:.6}", result.rms);
    println!("time        = {:.3} s", elapsed.as_secs_f64());
    println!("NN time     = (per-iteration, total) ~{:.3} s", elapsed.as_secs_f64());
}

/// Generate a Gaussian point cloud with `n` points.
fn generate_gaussian(n: usize, sigma: f32, seed: u64) -> Vec<f32> {
    let mut state = seed;
    let mut next = || {
        // xorshift64 — small, fast, deterministic
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        (state as f32 / u64::MAX as f32)
    };
    let normal = || {
        // Box-Muller
        let u1 = next().max(1e-6);
        let u2 = next();
        (-2.0 * u1.ln()).sqrt() * (2.0 * std::f32::consts::PI * u2).cos() * sigma
    };
    let mut out = Vec::with_capacity(n * 3);
    for _ in 0..n {
        out.push(normal());
        out.push(normal());
        out.push(normal());
    }
    out
}
