// src/main.rs — Sandbox CLI entry point
//
// Usage examples:
//   cargo run -- las-load --file data/scan.las
//   cargo run -- icp-bench --model m.las --data d.las --iterations 50
//   cargo test
//   cargo bench

fn main() {
    if let Err(e) = cc_sandbox::run_cli() {
        eprintln!("error: {}", e);
        std::process::exit(1);
    }
}
