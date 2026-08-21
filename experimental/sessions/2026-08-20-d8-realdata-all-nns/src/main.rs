//! D8 end-to-end real-data test: run `icp_with_nn` with each of
//! the 3 NNs (naive, kiddo, octree) on a subsample of the
//! brook-avenue `.las` scan. Same data, same algorithm, different
//! NN — compare wall time and correctness.
//!
//! This is the D8 deliverable on real survey data. The previous
//! `2026-08-20-realdata-icp-stack` session tested the
//! **recommended stack** (coarse_align + multi-resolution) with
//! cc-rust's brute force NN. This session tests the
//! **pluggable-NN step** with each variant's NN — confirming the
//! D8 trait dispatch is real, not just a signature change.

use std::path::PathBuf;
use std::time::Instant;

use clap::Parser;

use cc_rust::registration::IcprParamsRust;
use las_v1_pure_rust::read_all_xyz;

// Re-import the variant `icp_iterate` entry points. Each one
// goes through `icp_with_nn` with the variant's NN, so this
// session exercises the D8 trait dispatch end-to-end.
use icp_v1_naive::icp_iterate as icp_naive;
use icp_v2_kiddo::icp_iterate as icp_kiddo;
use icp_v3_octree::icp_iterate as icp_octree;
use icp_v4_dgm_octree::icp_iterate as icp_dgm;

#[derive(clap::ValueEnum, Clone, Copy, Debug, PartialEq, Eq)]
enum NnChoice {
    /// All 4 NNs (naive, kiddo, handrolled-octree, dgm-octree).
    /// Naive is O(n²) — infeasible above ~50k points.
    All,
    /// Brute force NN (O(n²), for reference only).
    Naive,
    /// kiddo 6.0 KD-tree.
    Kiddo,
    /// Hand-rolled octree (no AABB pruning, 100-300x slower
    /// than D9 — for comparison only).
    Handrolled,
    /// D9 cell-code-ordered NN (cc-rust/src/dgm_octree.rs). The
    /// C++-compat NN — slower than kiddo by 1.5-3x but matches
    /// the C++ DgmOctree nearest-neighbour exactly.
    Dgm,
}

#[derive(Parser, Debug)]
#[command(name = "d8_realdata_all_nns")]
struct Cli {
    /// Path to the .las file.
    file: PathBuf,
    /// Subsample stride. Default 150 → ~50k pts from the 7.5M-point brook-avenue scan.
    /// Use `--stride 1` to use the full 7.5M-point scan (only practical with kiddo).
    #[arg(short, long, default_value_t = 150)]
    stride: usize,
    /// Which NN variants to run. Default: all (the 3 from D8).
    /// Use `kiddo` for the full 7.5M run (naive is O(n²), infeasible).
    #[arg(long, value_enum, default_value_t = NnChoice::All)]
    nn: NnChoice,
    /// Known translation to apply (X axis, in metres).
    #[arg(short = 'x', long, default_value_t = 0.5)]
    translation: f32,
    /// Max ICP iterations per pass.
    #[arg(short, long, default_value_t = 30)]
    iterations: u32,
}

#[derive(Debug)]
struct NnResult {
    variant: &'static str,
    wall_seconds: f64,
    iterations: u32,
    converged: bool,
    rms: f64,
    recovered_tx: f64,
    recovered_ty: f64,
    recovered_tz: f64,
}

fn main() {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();
    let cli = Cli::parse();

    // ── Read the .las ───────────────────────────────────────────────────
    eprintln!("[LAS] Reading {}...", cli.file.display());
    let t0 = Instant::now();
    let raw = read_all_xyz(&cli.file).expect("LAS read failed");
    eprintln!("  read {} points in {:.2}s", raw.len() / 4, t0.elapsed().as_secs_f64());

    // Subsample.
    let n_total = raw.len() / 4;
    let mut model: Vec<f32> = Vec::with_capacity(n_total / cli.stride * 3);
    for i in (0..n_total).step_by(cli.stride) {
        model.push(raw[i * 4] as f32);
        model.push(raw[i * 4 + 1] as f32);
        model.push(raw[i * 4 + 2] as f32);
    }
    let n = model.len() / 3;
    eprintln!("  subsampled to {} points (stride {})", n, cli.stride);

    // Apply known translation to make the data cloud.
    let mut data: Vec<f32> = model.clone();
    for i in 0..n {
        data[i * 3] += cli.translation;
    }
    eprintln!("[Setup] data = model + ({}, 0, 0)", cli.translation);
    eprintln!();

    // ── Run each NN variant ─────────────────────────────────────────────
    let params = IcprParamsRust {
        max_iterations: cli.iterations,
        min_rms_decrease: 1e-6,
        ..Default::default()
    };

    let mut results = Vec::new();

    // IMPORTANT: `icp_iterate` mutates `data` in place. To compare
    // NNs fairly, we must hand each variant a *fresh* copy of the
    // shifted data. Otherwise the first run's ICP moves the data
    // to the model, and subsequent runs see data == model (RMS = 0
    // immediately, identity transform, 2 iters "converged" but
    // the translation wasn't actually recovered).
    let data_template = data.clone();

    eprintln!("=== 01-naive-on2 (brute force NN, via icp_with_nn) ===");
    if matches!(cli.nn, NnChoice::All | NnChoice::Naive) {
        let mut data = data_template.clone();
        let r = run_one("01-naive-on2", &mut data, &model, &params, icp_naive);
        print_row(&r, cli.translation as f64);
        results.push(r);
    } else {
        eprintln!("  (skipped — use --nn naive to enable; O(n^2), infeasible above ~50k pts)");
    }

    eprintln!();
    eprintln!("=== 02-kiddo-kdtree (kiddo 6.0 KD-tree, via icp_with_nn) ===");
    if matches!(cli.nn, NnChoice::All | NnChoice::Kiddo) {
        let mut data = data_template.clone();
        let r = run_one("02-kiddo-kdtree", &mut data, &model, &params, icp_kiddo);
        print_row(&r, cli.translation as f64);
        results.push(r);
    } else {
        eprintln!("  (skipped — use --nn kiddo to enable)");
    }

    eprintln!();
    eprintln!("=== 03-handrolled-octree (hand-rolled octree, via icp_with_nn) ===");
    if matches!(cli.nn, NnChoice::All | NnChoice::Handrolled) {
        let mut data = data_template.clone();
        let r = run_one("03-handrolled-octree", &mut data, &model, &params, icp_octree);
        print_row(&r, cli.translation as f64);
        results.push(r);
    } else {
        eprintln!("  (skipped — use --nn handrolled to enable)");
    }

    eprintln!();
    eprintln!("=== 04-dgm-octree (D9 cell-code-ordered NN, via icp_with_nn) ===");
    if matches!(cli.nn, NnChoice::All | NnChoice::Dgm) {
        let mut data = data_template.clone();
        let r = run_one("04-dgm-octree", &mut data, &model, &params, icp_dgm);
        print_row(&r, cli.translation as f64);
        results.push(r);
    } else {
        eprintln!("  (skipped — use --nn dgm to enable)");
    }

    // ── Summary ─────────────────────────────────────────────────────────
    eprintln!();
    eprintln!("=== Summary ===");
    eprintln!(
        "{:<22} {:>10} {:>10} {:>12} {:>14} {:>14}",
        "variant", "iter", "conv", "wall (s)", "rms", "tx_err"
    );
    for r in &results {
        let tx_err = (r.recovered_tx - (-cli.translation as f64)).abs();
        eprintln!(
            "{:<22} {:>10} {:>10} {:>12.3} {:>14.6} {:>14.6}",
            r.variant,
            r.iterations,
            r.converged,
            r.wall_seconds,
            r.rms,
            tx_err
        );
    }

    // Verify the trait dispatch is correct: all variants should
    // produce the same RMS to within fp tolerance (using the
    // first variant as the baseline).
    let max_diff = if !results.is_empty() {
        let rms0 = results[0].rms;
        let max = results
            .iter()
            .map(|r| (r.rms - rms0).abs())
            .fold(0.0_f64, f64::max);
        eprintln!();
        eprintln!("[Correctness] max |RMS_variant - RMS_first| = {:.2e}", max);
        if max < 1e-3 {
            eprintln!("[OK] All variants produce the same RMS (D8 trait dispatch is correct)");
        } else {
            eprintln!("[WARN] RMS differs across variants — investigate");
            std::process::exit(1);
        }
        max
    } else {
        eprintln!();
        eprintln!("[Correctness] (skipped — no variants ran)");
        0.0
    };

    // The translation recovery: all three should be close to the
    // expected -cli.translation on the X axis.
    let expected_tx = -cli.translation as f64;
    let max_tx_err = results
        .iter()
        .map(|r| (r.recovered_tx - expected_tx).abs())
        .fold(0.0_f64, f64::max);
    eprintln!(
        "[Correctness] max |tx_recovered - expected| = {:.4} (expected: {:.4})",
        max_tx_err, expected_tx
    );
    if max_tx_err < 0.05 {
        eprintln!("[OK] All three NNs recover the translation within 0.05m");
    } else {
        eprintln!("[WARN] translation recovery is off — investigate");
        std::process::exit(1);
    }

    // Write results.json for the run.ps1 / tests harness.
    let json_path = std::env::current_dir().unwrap().join("results.json");
    let json = serde_json::json!({
        "scenario": "2026-08-20-d8-realdata-all-nns",
        "question": "D8 end-to-end: same data, same ICP algorithm, different NN — compare wall time and correctness.",
        "las_file": cli.file.to_string_lossy(),
        "n_total": n_total,
        "n_subsample": n,
        "stride": cli.stride,
        "translation": cli.translation,
        "iterations": cli.iterations,
        "results": results.iter().map(|r| serde_json::json!({
            "variant": r.variant,
            "wall_seconds": r.wall_seconds,
            "iterations": r.iterations,
            "converged": r.converged,
            "rms": r.rms,
            "recovered_tx": r.recovered_tx,
            "recovered_ty": r.recovered_ty,
            "recovered_tz": r.recovered_tz,
            "tx_err": (r.recovered_tx - expected_tx).abs(),
        })).collect::<Vec<_>>(),
        "max_rms_diff": max_diff,
        "max_tx_err": max_tx_err,
    });
    std::fs::write(&json_path, serde_json::to_string_pretty(&json).unwrap())
        .expect("write results.json");
    eprintln!();
    eprintln!("Wrote {}", json_path.display());
}

fn run_one<F>(
    variant: &'static str,
    data: &mut [f32],
    model: &[f32],
    params: &IcprParamsRust,
    icp_fn: F,
) -> NnResult
where
    F: Fn(&mut [f32], &[f32], &IcprParamsRust) -> Result<cc_rust::registration::IcprResultRust, cc_rust::registration::IcprErrorRust>,
{
    // Diagnostic: for the first 3 data points, find the brute-force
    // NN distance so we can compare against what the variant's NN
    // returns. The brute-force distance is the ground truth.
    let n = data.len() / 3;
    let n_model = model.len() / 3;
    eprintln!("  [diag] first 3 brute-force NN distances (ground truth):");
    for i in 0..3.min(n) {
        let dx = data[i * 3] as f64;
        let dy = data[i * 3 + 1] as f64;
        let dz = data[i * 3 + 2] as f64;
        let mut best_idx = 0_usize;
        let mut best_d2 = f64::MAX;
        for j in 0..n_model {
            let ddx = model[j * 3] as f64 - dx;
            let ddy = model[j * 3 + 1] as f64 - dy;
            let ddz = model[j * 3 + 2] as f64 - dz;
            let d2 = ddx * ddx + ddy * ddy + ddz * ddz;
            if d2 < best_d2 { best_d2 = d2; best_idx = j; }
        }
        eprintln!(
            "    data[{}]=({:.3},{:.3},{:.3}) -> model[{}]={:.3} d={:.4}",
            i, dx, dy, dz, best_idx, best_d2.sqrt(), best_d2
        );
    }
    eprintln!("  [diag] data[0] is at {:?}, model[0] is at {:?}, expected offset 0.5 on X",
              (data[0], data[1], data[2]),
              (model[0], model[1], model[2]));

    let t0 = Instant::now();
    let result = icp_fn(data, model, params).expect("ICP failed");
    let wall = t0.elapsed().as_secs_f64();
    NnResult {
        variant,
        wall_seconds: wall,
        iterations: result.iterations,
        converged: result.converged,
        rms: result.rms,
        recovered_tx: result.transform[12],
        recovered_ty: result.transform[13],
        recovered_tz: result.transform[14],
    }
}

fn print_row(r: &NnResult, translation: f64) {
    eprintln!(
        "  iterations={} converged={} wall={:.3}s rms={:.6}",
        r.iterations, r.converged, r.wall_seconds, r.rms
    );
    eprintln!(
        "  recovered t=({:.4}, {:.4}, {:.4})  expected -x=({:.4}, 0, 0)",
        r.recovered_tx, r.recovered_ty, r.recovered_tz, -translation
    );
    let tx_err = (r.recovered_tx - (-translation)).abs();
    eprintln!("  tx_err = {:.4}", tx_err);
}
