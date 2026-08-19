//! End-to-end Rust ICP test on a real `.las` scan.
//!
//! 1. Read .las via pure-Rust `las` crate
//! 2. Subsample to a manageable N (e.g. 50k points) using a deterministic stride
//! 3. Create a "data" cloud by translating the model by a known offset
//! 4. Run ICP (cc-rust's Horn 1987 SVD-based) for N iterations
//! 5. Verify the recovered translation matches the known offset
//! 6. Report timing and quality

use std::path::PathBuf;
use std::time::Instant;
use clap::Parser;

use las_v1_pure_rust::read_all_xyz;  // re-exported from las-parsers/01-pure-rust-las via direct dep
use cc_rust::registration::{icp_iterate, IcprParamsRust};
use cc_rust::scalar_field::{mean, rms};

#[derive(Parser, Debug)]
#[command(name = "realdata_icp")]
struct Cli {
    /// Path to the .las file.
    file: PathBuf,
    /// Subsample stride (every Nth point). Default: 150 → ~50k pts from a 7.5M file.
    #[arg(short, long, default_value_t = 150)]
    stride: usize,
    /// Known translation to apply to the data cloud.
    #[arg(short, long, default_value_t = 0.5)]
    translation: f32,
    /// Max ICP iterations.
    #[arg(short, long, default_value_t = 30)]
    iterations: u32,
}

fn main() {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();
    let cli = Cli::parse();

    // ── Phase 4: read the .las file ──────────────────────────────────
    eprintln!("[Phase 4] Reading .las via pure-Rust `las` crate...");
    let t0 = Instant::now();
    let raw = read_all_xyz(&cli.file).expect("LAS read failed");
    eprintln!("  read {} points in {:.2}s", raw.len() / 4, t0.elapsed().as_secs_f64());

    // The crate returns interleaved [x, y, z, intensity] in f64.
    // We just want xyz. Subsample with a stride.
    let mut model: Vec<f32> = Vec::with_capacity(raw.len() / 4 / cli.stride * 3);
    let n_total = raw.len() / 4;
    for i in (0..n_total).step_by(cli.stride) {
        model.push(raw[i * 4] as f32);
        model.push(raw[i * 4 + 1] as f32);
        model.push(raw[i * 4 + 2] as f32);
    }
    let n = model.len() / 3;
    eprintln!("  subsampled to {} points", n);

    // ── Phase 1: statistics on the model cloud ─────────────────────
    let xs: Vec<f32> = (0..n).map(|i| model[i * 3]).collect();
    let ys: Vec<f32> = (0..n).map(|i| model[i * 3 + 1]).collect();
    let zs: Vec<f32> = (0..n).map(|i| model[i * 3 + 2]).collect();
    eprintln!("[Phase 1] ScalarField stats on model:");
    eprintln!("  X: mean={:.3} rms={:.3}", mean(&xs), rms(&xs));
    eprintln!("  Y: mean={:.3} rms={:.3}", mean(&ys), rms(&ys));
    eprintln!("  Z: mean={:.3} rms={:.3}", mean(&zs), rms(&zs));

    // ── Build the "data" cloud = model translated by (tx, 0, 0) ───
    let mut data: Vec<f32> = Vec::with_capacity(model.len());
    for i in 0..n {
        data.push(model[i * 3] + cli.translation);
        data.push(model[i * 3 + 1]);
        data.push(model[i * 3 + 2]);
    }
    eprintln!("[Setup] Created data cloud = model + ({}, 0, 0)", cli.translation);

    // ── Phase 2: run ICP ─────────────────────────────────────────────
    let params = IcprParamsRust { max_iterations: cli.iterations, min_rms_decrease: 1e-6 };
    eprintln!("[Phase 2] Running ICP ({} max iterations, O(n²) NN baseline)...", cli.iterations);
    let t1 = Instant::now();
    let result = icp_iterate(&mut data, &model, &params).expect("ICP failed");
    let elapsed = t1.elapsed();
    eprintln!("  ICP finished in {:.3}s ({:.1}s per iteration)", elapsed.as_secs_f64(), elapsed.as_secs_f64() / cli.iterations as f64);
    eprintln!("  converged={} final_rms={:.6}", result.converged, result.rms);

    // ── Verify the recovered transform ──────────────────────────────
    // The translation is at indices 12, 13, 14 of the column-major 4×4 matrix
    let recovered_tx = result.transform[12];
    let recovered_ty = result.transform[13];
    let recovered_tz = result.transform[14];
    eprintln!("[Verify] Recovered translation: ({:.4}, {:.4}, {:.4})", recovered_tx, recovered_ty, recovered_tz);
    eprintln!("[Verify] Expected translation:   ({:.4}, 0.0000, 0.0000)", cli.translation);
    let err = ((recovered_tx - cli.translation as f64).powi(2)
             + (recovered_ty).powi(2)
             + (recovered_tz).powi(2))
             .sqrt();
    eprintln!("[Verify] Error: {:.6}", err);
    if err < 0.05 {
        eprintln!("[OK] ICP recovered the translation within tolerance");
    } else {
        eprintln!("[WARN] ICP translation error is large — check the data");
        std::process::exit(1);
    }
}
