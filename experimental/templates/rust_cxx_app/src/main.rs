//! Demo CLI for the rust_cxx_app template.
//!
//! Run with: `cargo run --bin demo_cli -- 1 2 3 4 5`
//!
//! Demonstrates calling C++ from Rust (via the bridge) and printing the result.

use rust_cxx_app_template::ffi;

fn main() {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();

    let args: Vec<f64> = std::env::args()
        .skip(1)
        .filter_map(|s| s.parse().ok())
        .collect();

    if args.is_empty() {
        eprintln!("usage: demo_cli <num1> [num2] ...");
        std::process::exit(2);
    }

    // Call C++ from Rust
    let cpp_greeting = ffi::greet_from_cpp("Rust");
    println!("[C++ says] {}", cpp_greeting);

    // Call Rust from Rust
    let s = rust_cxx_app_template::sum_rust(&args);
    let m = rust_cxx_app_template::mean_rust(&args);
    println!("[Rust says] sum = {}, mean = {:.6}", s, m);
}
