//! ICP variant 4 — DgmOctree cell-code NN — CLI benchmark.

use cc_rust::registration::NearestNeighbour;
use icp_v4_dgm_octree::{default_params, icp_iterate};

fn main() {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();

    let args: Vec<String> = std::env::args().skip(1).collect();
    let n: usize = args.get(0).and_then(|s| s.parse().ok()).unwrap_or(2000);
    let seed: u64 = args.get(1).and_then(|s| s.parse().ok()).unwrap_or(42);

    let model = generate_gaussian(n, 0.4, seed);
    let mut data: Vec<f32> = (0..n)
        .flat_map(|i| vec![model[i * 3] + 0.5, model[i * 3 + 1] - 0.2, model[i * 3 + 2] + 0.1])
        .collect();

    eprintln!("DgmOctree ICP: {} points", n);

    // Phase 1: build the cell-code octree. The adapter picks
    // the optimal level (~4 pts/cell) for us, so the same
    // DgmOctreeNN drives both the standalone query bench
    // below and the ICP path.
    let build_start = std::time::Instant::now();
    let nn = cc_rust::dgm_octree::DgmOctreeNN::build(&model);
    let build_elapsed = build_start.elapsed();
    println!("build_time   = {:.3} s", build_elapsed.as_secs_f64());

    // Phase 2: 1000 NN queries
    let n_model = model.len() / 3;
    let query_points: Vec<[f32; 3]> = (0..1000)
        .map(|i| {
            let idx = (i * 7) % n_model;
            [model[idx * 3] + 0.1, model[idx * 3 + 1], model[idx * 3 + 2]]
        })
        .collect();
    let query_start = std::time::Instant::now();
    let mut total_nn = 0;
    for q in &query_points {
        let _ = nn.nearest(q);
        total_nn += 1;
    }
    let query_elapsed = query_start.elapsed();
    let per_query_us = query_elapsed.as_micros() as f64 / total_nn as f64;
    println!("query_time   = {:.3} s ({} queries, {:.2} µs/query)",
             query_elapsed.as_secs_f64(), total_nn, per_query_us);

    // Phase 3: full ICP via cc-rust (DgmOctree-driven NN)
    let start = std::time::Instant::now();
    let result = icp_iterate(&mut data, &model, &default_params()).unwrap();
    let elapsed = start.elapsed();
    println!("iterations   = {}", result.iterations);
    println!("converged    = {}", result.converged);
    println!("final rms    = {:.6}", result.rms);
    println!("icp_time     = {:.3} s", elapsed.as_secs_f64());
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
