//! ICP variant 3 — hand-rolled octree — CLI benchmark.

use icp_v3_octree::{icp_iterate, IcpParams};

fn main() {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();

    let args: Vec<String> = std::env::args().skip(1).collect();
    let n: usize = args.get(0).and_then(|s| s.parse().ok()).unwrap_or(2000);
    let seed: u64 = args.get(1).and_then(|s| s.parse().ok()).unwrap_or(42);

    let model = generate_gaussian(n, 0.4, seed);
    let mut data: Vec<f32> = (0..n).flat_map(|i| vec![model[i*3] + 0.5, model[i*3+1], model[i*3+2]]).collect();

    eprintln!("Octree ICP: {} points", n);
    let start = std::time::Instant::now();
    let result = icp_iterate(&mut data, &model, &IcpParams::default()).unwrap();
    let elapsed = start.elapsed();

    println!("iterations  = {}", result.iterations);
    println!("converged   = {}", result.converged);
    println!("final rms   = {:.6}", result.rms);
    println!("time        = {:.3} s (incl. octree build)", elapsed.as_secs_f64());
}

fn generate_gaussian(n: usize, sigma: f32, seed: u64) -> Vec<f32> {
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
        let theta = 2.0 * std::f32::consts::PI * u2;
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
