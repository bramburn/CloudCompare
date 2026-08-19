//! cc_rust CLI entry point.

use clap::{Parser, Subcommand};
use std::path::PathBuf;

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
    /// Run ICP between two point clouds (CSV: x,y,z per row).
    Icp {
        /// Path to the model CSV.
        #[arg(short, long)]
        model: PathBuf,
        /// Path to the data CSV (will be transformed).
        #[arg(short, long)]
        data: PathBuf,
        /// Maximum iterations.
        #[arg(short = 'i', long, default_value_t = 50)]
        iterations: u32,
        /// Output path for the transformed data CSV.
        #[arg(short, long)]
        output: Option<PathBuf>,
    },
    /// Show the build configuration (Phase 0 status).
    Status,
}

fn main() {
    let cli = Cli::parse();

    match cli.cmd {
        Cmd::ScalarStats { file, column, json } => {
            eprintln!("scalar-stats: file={:?} column={} json={}", file, column, json);
            eprintln!("  (Not implemented yet — this is the Phase 0 CLI scaffold)");
            eprintln!("  See experimental/sessions/2026-08-19-rust-migration-icp-scalarfield/ for the working lib.");
            std::process::exit(1);
        }
        Cmd::Icp { model, data, iterations, output } => {
            eprintln!("icp: model={:?} data={:?} iters={} output={:?}", model, data, iterations, output);
            eprintln!("  (Not implemented yet — see experimental/sessions/2026-08-19-rust-migration-icp-scalarfield/)");
            std::process::exit(1);
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
