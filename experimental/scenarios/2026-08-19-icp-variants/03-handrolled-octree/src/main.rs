//! ICP variant 3 — hand-rolled octree — CLI benchmark.
//!
//! Times three things separately so they can be compared to
//! `01-naive-on2`:
//!  1. **Build**: octree construction from a Gaussian model cloud.
//!  2. **NN query**: 1000 nearest-neighbour queries on the tree
//!     (this is the hot loop in ICP).
//!  3. **Full ICP**: corrected ICP via cc-rust, as a sanity check.

use icp_v3_octree::{icp_iterate, default_params, Octree};

fn main() {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();

    let args: Vec<String> = std::env::args().skip(1).collect();
    let n: usize = args.get(0).and_then(|s| s.parse().ok()).unwrap_or(2000);
    let seed: u64 = args.get(1).and_then(|s| s.parse().ok()).unwrap_or(42);

    // Generate a model cloud: random Gaussian blob.
    let model = generate_gaussian(n, 0.4, seed);
    // Generate data = model translated by (0.5, -0.2, 0.1).
    let mut data: Vec<f32> = (0..n)
        .flat_map(|i| vec![model[i * 3] + 0.5, model[i * 3 + 1] - 0.2, model[i * 3 + 2] + 0.1])
        .collect();

    eprintln!("Hand-rolled octree ICP: {} points", n);

    // Phase 1: build the octree
    let n_model = model.len() / 3;
    let model_arr: Vec<[f32; 3]> = (0..n_model)
        .map(|i| [model[i*3], model[i*3+1], model[i*3+2]])
        .collect();
    let build_start = std::time::Instant::now();
    let tree = Octree::from_points(&model_arr);
    let build_elapsed = build_start.elapsed();
    println!("build_time   = {:.3} s", build_elapsed.as_secs_f64());

    // Phase 2: 1000 NN queries (representative of one ICP iter
    // over a 1000-point subset)
    let query_points: Vec<[f32; 3]> = (0..1000)
        .map(|i| {
            let idx = (i * 7) % n_model; // spread out
            [model[idx*3] + 0.1, model[idx*3+1], model[idx*3+2]]
        })
        .collect();
    let query_start = std::time::Instant::now();
    let mut total_nn = 0;
    for q in &query_points {
        let _ = tree.nearest(*q);
        total_nn += 1;
    }
    let query_elapsed = query_start.elapsed();
    let per_query_us = query_elapsed.as_micros() as f64 / total_nn as f64;
    println!("query_time   = {:.3} s ({} queries, {:.2} µs/query)",
             query_elapsed.as_secs_f64(), total_nn, per_query_us);

    // Phase 3: full ICP via cc-rust (corrected algorithm)
    let start = std::time::Instant::now();
    let result = icp_iterate(&mut data, &model, &default_params()).unwrap();
    let elapsed = start.elapsed();
    println!("iterations   = {}", result.iterations);
    println!("converged    = {}", result.converged);
    println!("final rms    = {:.6}", result.rms);
    println!("icp_time     = {:.3} s", elapsed.as_secs_f64());
}

/// Generate a Gaussian point cloud with `n` points.
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
