//! Demo CLI for the rust_lib template.
//!
//! Run with: `cargo run --bin demo_cli -- 1 2 3 4 5`
//!
//! Replace with your experiment's CLI.

use rust_lib_template::{init_sentry, mean, variance};

fn main() {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();

    // Initialise Sentry. Reads DSN from `SENTRY_DSN` env; defaults to no-op.
    let dsn = std::env::var("SENTRY_DSN").unwrap_or_default();
    let release = format!("rust_lib_template@{}", env!("CARGO_PKG_VERSION"));
    let _guard = init_sentry(&dsn, &release);

    let args: Vec<f64> = std::env::args()
        .skip(1)
        .filter_map(|s| s.parse().ok())
        .collect();

    if args.is_empty() {
        eprintln!("usage: demo_cli <num1> [num2] ...");
        std::process::exit(2);
    }

    let m = mean(&args).expect("non-empty args");
    let v = variance(&args).expect("non-empty args");

    println!("n      = {}", args.len());
    println!("mean   = {:.6}", m);
    println!("var    = {:.6}", v);
    println!("stddev = {:.6}", v.sqrt());
}
