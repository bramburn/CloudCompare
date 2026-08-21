//! cc_rust CLI entry point.
//!
//! Subcommands:
//!   - `scalar-stats` — print stats on a CSV column (stub)
//!   - `icp` — run pure-Rust ICP on two point clouds (CSV or LAS),
//!     apply the recovered 4x4 transform to the data cloud, and
//!     optionally write the result to a new file + JSON metadata.
//!     The JSON format matches what the qRustICP plugin reads
//!     back from the subprocess.
//!   - `status` — print the build configuration

use std::fs::File;
use std::io::{BufWriter, Write};
use std::path::{Path, PathBuf};
use std::time::Instant;

use clap::{Parser, Subcommand};

use cc_rust::io::read_las_points;
use cc_rust::registration::{icp_with_nn, IcprParamsRust, NearestNeighbour};
use cc_rust::KiddoNN;

#[derive(Parser, Debug)]
#[command(name = "cc_rust")]
#[command(about = "Rust migration of CCCoreLib compute kernels")]
struct Cli {
    #[command(subcommand)]
    cmd: Cmd,
}

#[derive(Subcommand, Debug)]
enum Cmd {
    /// Compute ScalarField statistics on a CSV file (column = values).
    ScalarStats {
        /// Path to the input CSV file.
        #[arg(short, long)]
        file: PathBuf,
        /// Column index (0-based). Default: 0.
        #[arg(short, long, default_value_t = 0)]
        column: usize,
        /// Output as JSON instead of human-readable text.
        #[arg(long)]
        json: bool,
    },
    /// Run ICP between two point clouds.
    ///
    /// Inputs are auto-detected by extension: `.las` / `.laz` are
    /// read via the pure-Rust `las` crate; anything else is read
    /// as CSV (x, y, z per row, with header).
    ///
    /// The recovered 4x4 transform (column-major, `glam::Mat4`
    /// layout) is written to `--json`. If `--output` is given,
    /// the transformed data cloud is also written there as a
    /// CSV (`x,y,z` per row) or a LAS file (matching the input
    /// extension).
    Icp {
        /// Path to the model point cloud (reference, won't move).
        #[arg(short, long)]
        model: PathBuf,
        /// Path to the data point cloud (will be transformed).
        #[arg(short, long)]
        data: PathBuf,
        /// Maximum ICP iterations. Default: 50.
        #[arg(short = 'i', long, default_value_t = 50)]
        iterations: u32,
        /// Path to write the recovered 4x4 transform as JSON
        /// (column-major 16 floats, plus rms, iterations,
        /// wall_seconds). Matches the qRustICP plugin's
        /// contract — the plugin reads this file to apply
        /// the transform to the in-process cloud.
        #[arg(short, long)]
        json: PathBuf,
        /// Optional: write the transformed data cloud here.
        /// Extension determines the format: `.las`/`.laz` for
        /// LAS (LAZ is not yet supported on the write path —
        /// we write uncompressed LAS), `.csv` for CSV.
        #[arg(short, long)]
        output: Option<PathBuf>,
        /// Min RMS decrease for convergence. Default: 1e-6.
        #[arg(long, default_value_t = 1e-6)]
        min_rms: f32,
        /// Print human-readable progress to stderr in addition
        /// to the JSON file.
        #[arg(long, default_value_t = true)]
        verbose: bool,
    },
    /// Show the build configuration (Phase 0 status).
    Status,
}

fn main() {
    // ── Sentry crash reporting (opt-in via the `sentry` feature) ──
    //
    // Mirrors qCC's `CC_USE_SENTRY` opt-in. The guard is held for
    // the program's lifetime; on drop it flushes any queued
    // events and closes the Sentry transport (2-second deadline).
    // If the `sentry` feature is not enabled, the call is a no-op
    // (the function returns `None` and the module is empty).
    #[cfg(feature = "sentry")]
    let _sentry_guard = cc_rust::sentry_init::sentry_init();

    let cli = Cli::parse();

    match cli.cmd {
        Cmd::ScalarStats { file, column, json } => {
            eprintln!("scalar-stats: file={:?} column={} json={}", file, column, json);
            eprintln!("  (Not implemented yet — this is the Phase 0 CLI scaffold)");
            eprintln!("  See experimental/sessions/2026-08-19-rust-migration-icp-scalarfield/ for the working lib.");
            std::process::exit(1);
        }
        Cmd::Icp { model, data, iterations, json, output, min_rms, verbose } => {
            if let Err(e) = run_icp(&model, &data, iterations, &json, output.as_deref(), min_rms, verbose) {
                eprintln!("icp error: {e}");
                std::process::exit(1);
            }
        }
        Cmd::Status => {
            println!("cc_rust — Phase 0 scaffold");
            println!("=================================");
            println!("Build:        pure-Rust (no FFI)");
            println!("CXX FFI:      opt-in via --features cxx-ffi (requires MSVC)");
            println!();
            println!("Modules:");
            println!("  scalar_field  Phase 1 — done (28/28 tests, hybrid seq+rayon)");
            println!("  registration  Phase 2 — done (3/3 tests, O(n²) NN baseline)");
            println!("  octree        Phase 3 — skeleton (KD-tree, octree stubs)");
            println!("  io            Phase 4 — skeleton (CSV only; LAS/PLY TODO)");
            println!();
            println!("Next: Phase 0 → live FFI (see docs/PHASES.md)");
        }
    }
}

// ── Icp subcommand implementation ──────────────────────────────────────

/// Load a point cloud from a file, auto-detecting LAS vs CSV
/// by extension. Returns a flat `Vec<f32>` of `[x, y, z]` in
/// row-major (cloud) layout, which is what `icp_with_nn`
/// expects via its `to_f32_slice()` constructor.
fn load_cloud(path: &Path, verbose: bool) -> Result<Vec<[f32; 3]>, String> {
    let ext = path
        .extension()
        .and_then(|e| e.to_str())
        .map(|s| s.to_ascii_lowercase())
        .unwrap_or_default();
    match ext.as_str() {
        "las" | "laz" => {
            if verbose {
                eprintln!("  reading LAS: {}", path.display());
            }
            read_las_points(path).map_err(|e| format!("LAS read {}: {e}", path.display()))
        }
        "csv" | "txt" | "" => {
            if verbose {
                eprintln!("  reading CSV: {}", path.display());
            }
            let mut rdr = csv::ReaderBuilder::new()
                .has_headers(true)
                .flexible(true)
                .from_path(path)
                .map_err(|e| format!("CSV open {}: {e}", path.display()))?;
            let mut points = Vec::new();
            for (i, result) in rdr.records().enumerate() {
                let record = result.map_err(|e| format!("CSV row {i}: {e}"))?;
                if record.len() < 3 {
                    return Err(format!(
                        "CSV row {i}: expected at least 3 columns, got {}",
                        record.len()
                    ));
                }
                let x: f32 = record
                    .get(0)
                    .unwrap()
                    .parse()
                    .map_err(|e| format!("CSV row {i} x: {e}"))?;
                let y: f32 = record
                    .get(1)
                    .unwrap()
                    .parse()
                    .map_err(|e| format!("CSV row {i} y: {e}"))?;
                let z: f32 = record
                    .get(2)
                    .unwrap()
                    .parse()
                    .map_err(|e| format!("CSV row {i} z: {e}"))?;
                points.push([x, y, z]);
            }
            Ok(points)
        }
        other => Err(format!(
            "unrecognized extension '.{other}' — expected .las/.laz or .csv"
        )),
    }
}

/// Write a point cloud to a file. Auto-detects LAS vs CSV by
/// extension. The CSV path is the simple `[x, y, z]` writer;
/// the LAS path delegates to [`write_las_transformed`] which
/// requires a `las_template` (a path to a real LAS file
/// whose header + point format we copy).
fn write_cloud(
    path: &Path,
    points: &[[f32; 3]],
    las_template: Option<&Path>,
    verbose: bool,
) -> Result<(), String> {
    let ext = path
        .extension()
        .and_then(|e| e.to_str())
        .map(|s| s.to_ascii_lowercase())
        .unwrap_or_default();
    match ext.as_str() {
        "las" | "laz" => {
            if verbose {
                eprintln!(
                    "  writing LAS ({} pts): {}",
                    points.len(),
                    path.display()
                );
            }
            let template = las_template
                .ok_or_else(|| "LAS output requires a template (source data file)".to_string())?;
            write_las_transformed(path, template, points, verbose)
        }
        "csv" | "txt" | "" => {
            if verbose {
                eprintln!(
                    "  writing CSV ({} pts): {}",
                    points.len(),
                    path.display()
                );
            }
            let f = File::create(path).map_err(|e| format!("create {}: {e}", path.display()))?;
            let mut w = BufWriter::new(f);
            writeln!(w, "x,y,z").map_err(|e| e.to_string())?;
            for p in points {
                writeln!(w, "{},{},{}", p[0], p[1], p[2]).map_err(|e| e.to_string())?;
            }
            w.flush().map_err(|e| e.to_string())?;
            Ok(())
        }
        other => Err(format!(
            "unrecognized extension '.{other}' — expected .las or .csv"
        )),
    }
}

/// Write the transformed points as a LAS file. To preserve
/// the full point format (GPS time, color, etc. — brook-avenue
/// has both), we round-trip through the source data file:
/// read each original point, overwrite its x/y/z with the
/// transformed coordinates, and write the modified point. This
/// is the only way to preserve all the optional fields — the
/// `las` crate's `WritePoint` trait rejects points whose
/// optional-field set doesn't match the format exactly.
fn write_las_transformed(
    out_path: &Path,
    data_path: &Path,
    transformed: &[[f32; 3]],
    verbose: bool,
) -> Result<(), String> {
    use las::Writer;

    if verbose {
        eprintln!(
            "  writing LAS ({} pts, preserving all attributes): {}",
            transformed.len(),
            out_path.display()
        );
    }
    let src_file = File::open(data_path)
        .map_err(|e| format!("open source {data_path:?}: {e}"))?;
    let mut reader = las::Reader::new(std::io::BufReader::new(src_file))
        .map_err(|e| format!("read source header: {e}"))?;
    let header = reader.header().clone();
    if reader.header().number_of_points() as usize != transformed.len() {
        return Err(format!(
            "transformed point count ({}) doesn't match data file's \
             declared point count ({})",
            transformed.len(),
            reader.header().number_of_points()
        ));
    }

    let cursor = std::io::Cursor::new(Vec::<u8>::new());
    let mut writer = Writer::new(cursor, header)
        .map_err(|e| format!("LAS writer: {e}"))?;
    let mut i = 0usize;
    for p in reader.points() {
        let mut p = p.map_err(|e| format!("read source point {i}: {e}"))?;
        // Overwrite the position with the transformed
        // coordinates. All other fields (intensity, GPS time,
        // color, classification, etc.) are preserved
        // exactly as the source had them.
        let t = transformed[i];
        p.x = t[0] as f64;
        p.y = t[1] as f64;
        p.z = t[2] as f64;
        writer.write_point(p).map_err(|e| format!("LAS write point {i}: {e}"))?;
        i += 1;
    }
    let cursor = writer.into_inner().map_err(|e| format!("LAS flush: {e}"))?;
    let bytes = cursor.into_inner();
    std::fs::write(out_path, bytes).map_err(|e| format!("write {out_path:?}: {e}"))?;
    Ok(())
}

/// Convert a row-major `Vec<f64>` (4x4, last row `[0, 0, 0, 1]`)
/// into the column-major `[f64; 16]` representation expected by
/// JSON consumers (glam/nalgebra parity). The translation
/// goes in indices 12, 13, 14 (the last column of a
/// column-major 4x4).
fn to_col_major_4x4(transform: &[f64]) -> [f64; 16] {
    // `icp_with_nn` returns a row-major 4x4 in a `Vec<f64>`
    // (CCCoreLib uses f64 for ICP numerics to match the C++
    // `ccGLMatrix`). The JSON we emit uses column-major (the
    // convention of every mainstream Rust math crate, and
    // what the qRustICP plugin's `RustICP.cpp` does when it
    // reads it back). We transpose on the way out.
    let mut out = [0.0_f64; 16];
    for r in 0..4 {
        for c in 0..4 {
            // Row-major input → column-major output:
            // output[c * 4 + r] = input[r * 4 + c]
            out[c * 4 + r] = transform[r * 4 + c];
        }
    }
    out
}

/// Run the ICP subcommand end-to-end: load model + data,
/// run `icp_with_nn::<KiddoNN>`, write JSON + (optional)
/// transformed data cloud.
fn run_icp(
    model_path: &Path,
    data_path: &Path,
    iterations: u32,
    json_path: &Path,
    output_path: Option<&Path>,
    min_rms: f32,
    verbose: bool,
) -> Result<(), String> {
    if verbose {
        eprintln!("[icp] model  = {}", model_path.display());
        eprintln!("[icp] data   = {}", data_path.display());
        eprintln!("[icp] json   = {}", json_path.display());
        eprintln!("[icp] iters  = {}, min_rms = {}", iterations, min_rms);
    }
    let t_total = Instant::now();

    // Load both clouds.
    let t = Instant::now();
    let model = load_cloud(model_path, verbose).map_err(|e| format!("load model: {e}"))?;
    if verbose {
        eprintln!("[icp] loaded model: {} pts in {:.2}s", model.len(), t.elapsed().as_secs_f64());
    }
    let t = Instant::now();
    let mut data = load_cloud(data_path, verbose).map_err(|e| format!("load data: {e}"))?;
    if verbose {
        eprintln!("[icp] loaded data:  {} pts in {:.2}s", data.len(), t.elapsed().as_secs_f64());
    }
    if model.is_empty() || data.is_empty() {
        return Err(format!(
            "empty point cloud: model={} pts, data={} pts",
            model.len(),
            data.len()
        ));
    }

    // Convert from `Vec<[f32; 3]>` to a flat `Vec<f32>` in the
    // `icp_with_nn` row-major [x, y, z, x, y, z, ...] layout.
    let n_model = model.len();
    let n_data = data.len();
    let model_flat: Vec<f32> = model
        .iter()
        .flat_map(|p| [p[0], p[1], p[2]])
        .collect();
    let mut data_flat: Vec<f32> = data
        .iter()
        .flat_map(|p| [p[0], p[1], p[2]])
        .collect();

    // Build the Kiddo NN from the model. The build cost is
    // paid once; each ICP iteration is then O(n_data * log
    // n_model) for the NN search. Kiddo is the recommended
    // NN (1.5-3x faster than D9 at this scale, see
    // experimental/docs/SUMMARY.md §D9).
    let t = Instant::now();
    let nn = KiddoNN::build(&model_flat);
    if verbose {
        eprintln!("[icp] built kiddo NN in {:.2}s", t.elapsed().as_secs_f64());
    }

    // Run ICP. `icp_with_nn` mutates `data_flat` in place
    // (so the per-iteration transform composes into the
    // current data, not just the first-iteration delta).
    let params = IcprParamsRust {
        max_iterations: iterations,
        min_rms_decrease: min_rms as f64,
        ..Default::default()
    };
    let t = Instant::now();
    let result = icp_with_nn(&mut data_flat, &model_flat, &nn, &params)
        .map_err(|e| format!("ICP failed: {e:?}"))?;
    let wall = t.elapsed().as_secs_f64();

    // Convert the in-place mutated `data_flat` back to a
    // `Vec<[f32; 3]>` for the optional output file.
    let mut transformed: Vec<[f32; 3]> = Vec::with_capacity(n_data);
    for i in 0..n_data {
        transformed.push([
            data_flat[i * 3],
            data_flat[i * 3 + 1],
            data_flat[i * 3 + 2],
        ]);
    }

    // Build the JSON output. The `transform` field is the
    // recovered 4x4 transform in column-major order, matching
    // the convention of `glam::Mat4` and the qRustICP plugin's
    // `RustICP.cpp` parser. `result.transform` is a `Vec<f64>`
    // (CCCoreLib uses f64 for ICP numerics to match the C++
    // `ccGLMatrix`); we transpose on the way out to column-major.
    let col_major = to_col_major_4x4(&result.transform);
    let transform_json: Vec<f64> = col_major.to_vec();
    let recovered_t: [f64; 3] = [
        result.transform[12],
        result.transform[13],
        result.transform[14],
    ];
    let json = serde_json::json!({
        "scenario": "cc_rust_cli::icp",
        "model_path": model_path.to_string_lossy(),
        "data_path": data_path.to_string_lossy(),
        "n_model": n_model,
        "n_data": n_data,
        "iterations": result.iterations,
        "converged": result.converged,
        "rms": result.rms,
        "min_rms_decrease": min_rms,
        "max_iterations": iterations,
        "wall_seconds": wall,
        "recovered_t": {
            "tx": recovered_t[0],
            "ty": recovered_t[1],
            "tz": recovered_t[2],
        },
        "transform": transform_json,
    });
    let json_str = serde_json::to_string_pretty(&json)
        .map_err(|e| format!("JSON serialise: {e}"))?;
    std::fs::write(json_path, json_str).map_err(|e| format!("write {}: {e}", json_path.display()))?;
    if verbose {
        eprintln!("[icp] wrote JSON: {}", json_path.display());
    }

    // Optional: write the transformed data cloud. Use the
    // original data file as the LAS template (preserves PDRF
    // version + VLRs if any).
    if let Some(out) = output_path {
        // For LAS output, we use the data file as the
        // template so the output preserves the point format
        // (PDRF version, optional fields like GPS time and
        // color, etc.). For CSV output, no template is
        // needed — we just write x, y, z.
        let las_template = if out
            .extension()
            .and_then(|e| e.to_str())
            .map(|s| s.to_ascii_lowercase())
            .map(|s| s == "las" || s == "laz")
            .unwrap_or(false)
        {
            Some(data_path)
        } else {
            None
        };
        // For LAS output, we need to pass the data file as
        // BOTH the template AND the source of the points
        // (since `write_las` round-trips through the data
        // file to preserve all point attributes, not just
        // x, y, z).
        write_las_transformed(out, data_path, &transformed, verbose)
            .map_err(|e| format!("write output: {e}"))?;
    }

    // Drop the unused `data` Vec — we used `data_flat` in place.
    drop(data);

    if verbose {
        eprintln!();
        eprintln!("=== Summary ===");
        eprintln!("iterations      : {}", result.iterations);
        eprintln!("converged       : {}", result.converged);
        eprintln!("final RMS       : {:.6}", result.rms);
        eprintln!(
            "recovered t     : ({:.4}, {:.4}, {:.4})",
            recovered_t[0], recovered_t[1], recovered_t[2]
        );
        eprintln!("ICP wall (s)    : {:.3}", wall);
        eprintln!("total wall (s)  : {:.3}", t_total.elapsed().as_secs_f64());
    }
    Ok(())
}
