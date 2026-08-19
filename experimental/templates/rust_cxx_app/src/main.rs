//! Demo CLI for the rust_cxx_app template.
//!
//! Default (pure-Rust): `cargo run -- 1 2 3 4 5` — uses only Rust functions.
//! With CXX FFI: `cargo run --features cxx-ffi -- 1 2 3 4 5` — also calls C++.

use rust_cxx_app_template::{mean_rust, sum_rust};

#[cfg(feature = "cxx-ffi")]
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

    #[cfg(feature = "cxx-ffi")]
    {
        let cpp_greeting = ffi::greet_from_cpp("Rust");
        println!("[C++ says] {}", cpp_greeting);
    }

    #[cfg(not(feature = "cxx-ffi"))]
    println!("[pure-Rust build — pass --features cxx-ffi to call C++]");

    let s = sum_rust(&args);
    let m = mean_rust(&args);
    println!("[Rust says] sum = {}, mean = {:.6}", s, m);
}
