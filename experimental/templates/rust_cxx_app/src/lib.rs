//! rust_cxx_app template — Rust + CXX FFI.
//!
//! The CXX bridge is defined in this module under the `cxx-ffi` feature flag.
//! By default, this is a pure-Rust library that you can `cargo test` without
//! needing the MSVC toolchain.

#[cfg(feature = "cxx-ffi")]
#[cxx::bridge]
pub mod ffi {
    // ── Shared types (visible on both sides) ──────────────────────────
    struct Point3 {
        x: f64,
        y: f64,
        z: f64,
    }

    // ── Rust → C++ (Rust calls into C++) ──────────────────────────────
    // Note: CXX 1.0 requires `unsafe extern "C++"` if the block contains
    // any function declarations, even "safe" ones.
    unsafe extern "C++" {
        include!("cpp/bridge.h");
        fn greet_from_cpp(name: &str) -> String;
    }

    // ── C++ → Rust (C++ calls into Rust) ──────────────────────────────
    extern "Rust" {
        fn mean_rust(values: &[f64]) -> f64;
        fn sum_rust(values: &[f64]) -> f64;
    }
}

// ── Rust implementations (callable from C++ when cxx-ffi is on) ───

/// Mean of a slice. Returns 0.0 for an empty slice.
pub fn mean_rust(values: &[f64]) -> f64 {
    if values.is_empty() {
        log::warn!("mean_rust called on empty slice");
        return 0.0;
    }
    values.iter().sum::<f64>() / values.len() as f64
}

/// Sum of a slice. Returns 0.0 for an empty slice.
pub fn sum_rust(values: &[f64]) -> f64 {
    let s = values.iter().sum::<f64>();
    log::debug!("sum_rust({} values) = {}", values.len(), s);
    s
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn mean_rust_basic() {
        assert!((mean_rust(&[1.0, 2.0, 3.0, 4.0, 5.0]) - 3.0).abs() < 1e-9);
    }

    #[test]
    fn sum_rust_basic() {
        assert_eq!(sum_rust(&[1.0, 2.0, 3.0]), 6.0);
    }

    #[test]
    fn mean_rust_empty() {
        assert_eq!(mean_rust(&[]), 0.0);
    }
}
