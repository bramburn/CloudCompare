//! Real-data ICP using the recommended stack:
//! 1. Read .las via pure-Rust `las` crate.
//! 2. Subsample to a manageable N.
//! 3. Apply a known translation to make a synthetic "data" cloud.
//! 4. Run the recommended stack:
//!      a) coarse_align (PCA pre-alignment)
//!      b) icp_multi_resolution(0.1, 1.0) (coarse-to-fine)
//!         with outlier_rejection_fraction = 0.4
//! 5. Verify the recovered translation.
//!
//! Compare against the older realdata-icp session (which used
//! vanilla ICP and overshot by 4x).

use std::path::PathBuf;
use std::time::Instant;

use clap::Parser;
use las_v1_pure_rust::read_all_xyz;

use cc_rust::coarse_align::{apply_transform_in_place, coarse_align};
use cc_rust::registration::{icp_iterate, icp_multi_resolution, IcprParamsRust};

#[derive(Parser, Debug)]
#[command(name = "realdata_icp_stack")]
struct Cli {
    /// Path to the .las file.
    file: PathBuf,
    /// Subsample stride (every Nth point). Default: 150 → ~50k pts from a 7.5M file.
    #[arg(short, long, default_value_t = 150)]
    stride: usize,
    /// Known translation to apply (X axis, in metres).
    #[arg(short = 'x', long, default_value_t = 0.5)]
    translation: f32,
    /// Max ICP iterations per pass.
    #[arg(short, long, default_value_t = 30)]
    iterations: u32,
    /// Outlier rejection fraction. Default 0.4 = drop worst 40%.
    #[arg(long, default_value_t = 0.4)]
    outlier_rejection_fraction: f64,
    /// Multi-resolution coarse pass fraction. Default 0.1.
    #[arg(long, default_value_t = 0.1)]
    coarse_fraction: f64,
    /// Skip the recommended stack and run vanilla ICP only.
    #[arg(long)]
    vanilla_only: bool,
}

fn main() {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();
    let cli = Cli::parse();

    // ── Phase 4: read the .las file ──────────────────────────────────
    eprintln!("[Phase 4] Reading .las via pure-Rust `las` crate...");
    let t0 = Instant::now();
    let raw = read_all_xyz(&cli.file).expect("LAS read failed");
    eprintln!("  read {} points in {:.2}s", raw.len() / 4, t0.elapsed().as_secs_f64());

    // Subsample.
    let mut model: Vec<f32> = Vec::with_capacity(raw.len() / 4 / cli.stride * 3);
    let n_total = raw.len() / 4;
    for i in (0..n_total).step_by(cli.stride) {
        model.push(raw[i * 4] as f32);
        model.push(raw[i * 4 + 1] as f32);
        model.push(raw[i * 4 + 2] as f32);
    }
    let n = model.len() / 3;
    eprintln!("  subsampled to {} points", n);

    // Apply known translation to make the data cloud.
    let mut data: Vec<f32> = Vec::with_capacity(model.len());
    for i in 0..n {
        data.push(model[i * 3] + cli.translation);
        data.push(model[i * 3 + 1]);
        data.push(model[i * 3 + 2]);
    }
    eprintln!("[Setup] Data = model + ({}, 0, 0)", cli.translation);

    if cli.vanilla_only {
        run_vanilla(&data, &model, cli.iterations);
    } else {
        run_recommended_stack(&data, &model, &cli);
    }
}

fn run_vanilla(data: &[f32], model: &[f32], iterations: u32) {
    let mut data = data.to_vec();
    let params = IcprParamsRust {
        max_iterations: iterations,
        min_rms_decrease: 1e-6,
        outlier_rejection_fraction: 0.0,
        ..Default::default()
    };
    eprintln!("[Vanilla ICP] Running...");
    let t1 = Instant::now();
    let result = icp_iterate(&mut data, model, &params).expect("ICP failed");
    let elapsed = t1.elapsed();
    eprintln!("  ICP finished in {:.3}s", elapsed.as_secs_f64());
    eprintln!("  converged={} final_rms={:.6}", result.converged, result.rms);
    report("vanilla", result.transform[12], result.transform[13], result.transform[14], 0.5);
}

fn run_recommended_stack(data: &[f32], model: &[f32], cli: &Cli) {
    let mut data = data.to_vec();

    // Step 1: coarse_align (PCA pre-alignment)
    eprintln!("[1/3] coarse_align (PCA pre-alignment)...");
    let t1 = Instant::now();
    let pre = coarse_align(&data, model);
    eprintln!("  PCA done in {:.3}s", t1.elapsed().as_secs_f64());
    eprintln!("  centroids: data={:?} model={:?}",
              pre.centroid_data, pre.centroid_model);

    // Apply the pre-alignment to the data.
    let mut t = [0.0_f64; 16];
    t.copy_from_slice(&pre.transform);
    apply_transform_in_place(&mut data, &t);

    // Step 2: multi-resolution ICP
    eprintln!("[2/3] icp_multi_resolution(0.1, 1.0)...");
    let t2 = Instant::now();
    let params = IcprParamsRust {
        max_iterations: cli.iterations,
        min_rms_decrease: 1e-6,
        outlier_rejection_fraction: cli.outlier_rejection_fraction,
        ..Default::default()
    };
    let result = icp_multi_resolution(
        &mut data,
        model,
        &[cli.coarse_fraction, 1.0],
        &params,
    )
    .expect("multi-res ICP failed");
    eprintln!("  multi-res ICP done in {:.3}s", t2.elapsed().as_secs_f64());
    eprintln!("  final_rms={:.6}", result.rms);

    // Compose the cumulative transform: coarse_align then multi-res.
    // Both are 4x4 column-major. The composition is
    //   T_total = T_coarse * T_multi_res
    // (apply coarse_align first, then multi_res).
    let t_coarse: [f64; 16] = pre.transform
        .as_slice()
        .try_into()
        .expect("pre.transform should have 16 elements");
    let t_multi_slice: &[f64; 16] = result
        .transform
        .as_slice()
        .try_into()
        .expect("result.transform should have 16 elements");
    let mut t_total = [0.0_f64; 16];
    for col in 0..4 {
        for row in 0..4 {
            let mut s = 0.0;
            for k in 0..4 {
                s += t_coarse[col * 4 + k] * t_multi_slice[k * 4 + row];
            }
            t_total[col * 4 + row] = s;
        }
    }

    // Step 3 (diagnostic): single-iteration ICP at full resolution
    // with no trimming, to compare.
    let mut data2 = data.clone();
    // Re-apply coarse_align because multi-res mutated data.
    let mut t = [0.0_f64; 16];
    t.copy_from_slice(&pre.transform);
    apply_transform_in_place(&mut data2, &t);
    let params_vanilla = IcprParamsRust {
        max_iterations: cli.iterations,
        min_rms_decrease: 1e-6,
        outlier_rejection_fraction: 0.0,
        ..Default::default()
    };
    let vanilla = icp_iterate(&mut data2, model, &params_vanilla).expect("vanilla failed");
    eprintln!("  (vanilla ICP on the same starting pose, for comparison)");
    eprintln!("  vanilla_rms={:.6}", vanilla.rms);

    let tx = t_total[12];
    let ty = t_total[13];
    let tz = t_total[14];
    eprintln!("[Result] Recovered translation: ({:.4}, {:.4}, {:.4})", tx, ty, tz);
    // ICP returns the "data → model" transform. The test data is
    // model + cli.translation, so the expected transform is
    // -cli.translation.
    let expected_tx = -cli.translation as f64;
    eprintln!("[Result] Expected translation:   ({:.4}, 0.0000, 0.0000) (data→model direction)", expected_tx);
    let err = ((tx - expected_tx).powi(2) + ty.powi(2) + tz.powi(2)).sqrt();
    eprintln!("[Result] Error: {:.6}", err);
    eprintln!("[Result] RMS (recommended stack): {:.6}", result.rms);
    eprintln!("[Result] RMS (vanilla at same start): {:.6}", vanilla.rms);
    if err < 0.05 {
        eprintln!("[OK] recommended stack recovered the translation within 0.05");
    } else {
        eprintln!("[WARN] translation error is large");
    }
}

fn report(label: &str, tx: f64, ty: f64, tz: f64, expected: f64) {
    eprintln!("[{}] tx={:.4} ty={:.4} tz={:.4}", label, tx, ty, tz);
    let err = ((tx - expected).powi(2) + ty.powi(2) + tz.powi(2)).sqrt();
    eprintln!("[{}] error={:.6} (target < 0.05)", label, err);
}
