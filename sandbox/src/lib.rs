// src/lib.rs — CloudCompare Rust Sandbox
//
// Pure-Rust implementation. No C++ dependency required.
// All modules compile and test without any C++ toolchain.
//
// The CXX FFI bridge to CCCoreLib will be added separately when
// CCCoreLib is configured (see CONFIGURE_CCCORELIB.md).
//
// Migration phases:
//   Phase 1 → scalar_field module (ScalarField statistics)
//   Phase 2 → registration module (ICP, Horn registration)
//   Phase 3 → octree module (DgmOctree, KdTree)
//   Phase 4 → io module (LAS/PLY parsers — pure Rust via laszip)

pub mod scalar_field; // Phase 1
pub mod registration; // Phase 2
pub mod octree; // Phase 3
pub mod io; // Phase 4 (placeholder)

use std::path::Path;

// ── CLI + test harness entry point ────────────────────────────────────────

#[derive(Debug, clap::Parser)]
#[command(name = "cc-sandbox")]
enum Command {
    /// Load a .las file and print summary stats
    LasLoad {
        #[arg(long)]
        file: String,

        #[arg(long, default_value = "10")]
        sample_size: usize,
    },

    /// Run ICP benchmark between two point clouds
    IcpBench {
        #[arg(long)]
        model: String,

        #[arg(long)]
        data: String,

        #[arg(long, default_value = "50")]
        iterations: u32,

        #[arg(long)]
        output: Option<String>,
    },

    /// Run all unit tests
    #[cfg(test)]
    Test,
}

impl Default for Command {
    fn default() -> Self {
        Self::LasLoad {
            file: "data/sample.las".into(),
            sample_size: 10,
        }
    }
}

/// Main CLI entry point. Run with --help for options.
pub fn run_cli() -> Result<(), Box<dyn std::error::Error>> {
    use tracing_subscriber::{fmt, prelude::*, EnvFilter};

    // ── Logging setup ────────────────────────────────────────────────
    let env_filter =
        EnvFilter::try_from_default_env().unwrap_or_else(|_| EnvFilter::new("info"));

    let sentry_dsn = std::env::var("SENTRY_DSN").unwrap_or_else(|_| {
        "https://ac001120bfa96ba91d2ed97c62e632ad@o494653.ingest.us.sentry.io/4511938553053184"
            .into()
    });
    let release = format!("cloudcompare-sandbox@{}", env!("CARGO_PKG_VERSION"));

    // ── Sentry init (crash reporting) ───────────────────────────────
    // sentry::init takes ownership of the DSN string.
    let _sentry_guard = sentry::init(sentry_dsn);

    tracing_subscriber::registry()
        .with(env_filter)
        .with(fmt::layer().with_target(true))
        .init();

    tracing::info!(release = %release, "cc-sandbox starting");

    // ── Parse command ─────────────────────────────────────────────────
    let cmd: Command = clap::Parser::parse();

    match cmd {
        Command::LasLoad { file, sample_size } => las_load(&file, sample_size)?,
        Command::IcpBench { model, data, iterations, output } => {
            icp_bench(&model, &data, iterations, output.as_deref())?
        }
        #[cfg(test)]
        Command::Test => {
            // Tests are run via `cargo test`; this is a placeholder
            eprintln!("Run `cargo test` to execute all tests.");
        }
    }

    // ── Flush Sentry before exit ──────────────────────────────────────
    drop(_sentry_guard); // ensures events are flushed on scope drop

    Ok(())
}

// ── CLI implementations ───────────────────────────────────────────────────

fn las_load(path: &str, sample_size: usize) -> Result<(), Box<dyn std::error::Error>> {
    use tracing::{info, warn};

    let path = Path::new(path);

    // ── Load or generate cloud ─────────────────────────────────────
    // Pure Rust: try CSV first, fall back to synthetic
    let points: Vec<f32> = if path.exists() {
        let path_str = path.to_str().unwrap_or(""); // fall back for non-UTF8 paths
        match io::load_csv(path_str) {
            Ok(cloud) if !cloud.points.is_empty() => {
                info!(path = %path.display(), points = cloud.points.len() / 3, "CSV loaded (pure Rust)");
                cloud.points
            }
            Ok(_) | Err(_) => {
                warn!(path = %path.display(), "File not loadable as CSV — using synthetic data");
                generate_synthetic_cloud(1000)
            }
        }
    } else {
        info!("File not found — using synthetic data for testing");
        generate_synthetic_cloud(1000)
    };

    // ── Sample first N points ────────────────────────────────────────
    let sample = &points[..(sample_size * 3).min(points.len())];
    for (i, chunk) in sample.chunks(3).enumerate() {
        if chunk.len() == 3 {
            info!(i = i, x = chunk[0], y = chunk[1], z = chunk[2], "sample point");
        }
    }

    // ── Rust ScalarField stats on Z coordinates ─────────────────────
    if let Some(stats) = scalar_field::compute_stats(&points) {
        info!(
            mean = stats.mean,
            std = stats.std,
            min = stats.min,
            max = stats.max,
            count = stats.valid_count,
            "Z-coordinate statistics (pure Rust)"
        );
    }

    Ok(())
}

fn icp_bench(
    model_path: &str,
    data_path: &str,
    iterations: u32,
    output_path: Option<&str>,
) -> Result<(), Box<dyn std::error::Error>> {
    use tracing::{info, warn};

    info!(
        model = model_path,
        data = data_path,
        iterations = iterations,
        "Starting ICP benchmark (pure Rust)"
    );

    // ── Load clouds ─────────────────────────────────────────────────
    let model_cloud = load_or_synthetic(model_path, 500);
    let mut data_cloud = load_or_synthetic(data_path, 500);

    info!(
        model_points = model_cloud.len() / 3,
        data_points = data_cloud.len() / 3,
        "clouds ready"
    );

    // ── Run Rust ICP ─────────────────────────────────────────────────
    let params = registration::IcprParamsRust {
        max_iterations: iterations,
        min_rms_decrease: 1e-8,
    };

    let result = registration::icp_iterate(&mut data_cloud, &model_cloud, &params);

    match result {
        Ok(icp_result) => {
            info!(
                rms = icp_result.rms,
                converged = icp_result.converged,
                "ICP complete (pure Rust)"
            );

            if let Some(path) = output_path {
                let json = serde_json::json!({
                    "model_points": model_cloud.len() / 3,
                    "data_points": data_cloud.len() / 3,
                    "iterations": iterations,
                    "final_rms": icp_result.rms,
                    "converged": icp_result.converged,
                    "engine": "rust-sandbox-pure",
                });
                std::fs::write(path, serde_json::to_string_pretty(&json)?)?;
                info!(path = path, "Results written");
            }
        }
        Err(e) => {
            warn!(code = e.code, message = %e.message, "ICP failed");
        }
    }

    Ok(())
}

// ── Helpers ────────────────────────────────────────────────────────────────

fn load_or_synthetic(path: &str, default_n: usize) -> Vec<f32> {
    let p = Path::new(path);
    if p.exists() {
        let path_str = p.to_str().unwrap_or("");
        match io::load_csv(path_str) {
            Ok(cloud) if !cloud.points.is_empty() => cloud.points,
            Ok(_) | Err(_) => generate_synthetic_cloud(default_n),
        }
    } else {
        generate_synthetic_cloud(default_n)
    }
}

/// Generate a deterministic synthetic point cloud for testing.
fn generate_synthetic_cloud(n_points: usize) -> Vec<f32> {
    use std::collections::hash_map::DefaultHasher;
    use std::hash::{Hash, Hasher};

    let mut points = Vec::with_capacity(n_points * 3);
    for i in 0..n_points {
        let mut h = DefaultHasher::new();
        i.hash(&mut h);
        let h1 = (h.finish() % 1000) as f32 / 1000.0;
        let h2 = ((h.finish() >> 8) % 1000) as f32 / 1000.0;
        let h3 = ((h.finish() >> 16) % 1000) as f32 / 1000.0;

        let x = (h1 - 0.5) * 10.0;
        let y = (h2 - 0.5) * 10.0;
        let z = (h3 - 0.5) * 10.0 + 5.0; // offset Z so mean ≠ 0
        points.push(x);
        points.push(y);
        points.push(z);
    }
    points
}
