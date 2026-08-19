//! CLI: inspect a .las file and (optionally) read all points.

use std::path::PathBuf;
use clap::Parser;
use las_v1_pure_rust::{inspect, read_all_xyz};

#[derive(Parser, Debug)]
#[command(name = "las_inspect")]
struct Cli {
    /// Path to the .las file.
    file: PathBuf,
    /// Also load all points (slow for large files; default off).
    #[arg(long)]
    load: bool,
}

fn main() {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();
    let cli = Cli::parse();

    let start = std::time::Instant::now();
    let info = inspect(&cli.file).expect("inspect failed");
    let elapsed_header = start.elapsed();

    println!("== {} ==", cli.file.display());
    print!("{}", info);
    println!("Header parse:     {:.3} s", elapsed_header.as_secs_f64());

    if cli.load {
        let start = std::time::Instant::now();
        let pts = read_all_xyz(&cli.file).expect("read failed");
        let elapsed_load = start.elapsed();
        println!("Full read:        {:.3} s ({} points, {} floats)",
            elapsed_load.as_secs_f64(),
            pts.len() / 4,
            pts.len());

        // Quick sanity: first 3 points
        if pts.len() >= 12 {
            println!("First point:      ({:.3}, {:.3}, {:.3}) intensity={:.3}",
                pts[0], pts[1], pts[2], pts[3]);
        }
    }
}
