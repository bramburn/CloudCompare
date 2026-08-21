//! CXX FFI bridge to CCCoreLib's `ICPRegistrationTools::Register`.
//!
//! This module is gated behind the `cxx_ffi` Cargo feature. Build with
//! `cargo build --features cxx_ffi` (requires the MSVC toolchain).
//!
//! **What it does:** exposes a single Rust-callable function,
//! [`run_icp_cpp`], that takes two flat `&[f32]` point clouds and a
//! set of ICP parameters, calls the C++ `ICPRegistrationTools::Register`
//! under the hood, and returns the recovered transform + final RMS.
//! Used to validate the pure-Rust ICP against the C++ reference.
//!
//! **What it does NOT do:** replace the pure-Rust ICP. The
//! production code path stays pure-Rust (D8 trait dispatch, the
//! 4-variant scenario at `experimental/scenarios/2026-08-20-icp-nn-comparison/`).
//! This FFI is parity testing only.
//!
//! ## Build setup
//!
//! 1. Build CloudCompare (so `build/libs/qCC_db/extern/CCCoreLib/CCCoreLib.lib`
//!    and `.dll` exist). See `BUILD-LOCAL.md` and `tools/cc-configure.cmd`.
//! 2. `cd cc-rust && cargo build --release --features cxx_ffi`
//! 3. Run parity tests:
//!    `cargo test --release --features cxx_ffi icp_cpp_matches_rust`
//!
//! The build script copies `CCCoreLib.dll` next to the test binary
//! at build time so the runtime can find it.

#[cfg(feature = "cxx_ffi")]
#[cxx::bridge]
pub mod ffi_bridge {
    /// Parameters for the C++ ICP. Mirrors the relevant fields of
    /// `CCCoreLib::ICPRegistrationTools::Parameters`. We expose only
    /// the fields needed for parity testing; the rest take their
    /// C++ defaults (set in the C++ shim's `Parameters` constructor).
    pub struct IcpParamsCpp {
        pub min_rms_decrease: f64,
        pub nb_max_iterations: u32,
        pub adjust_scale: bool,
        pub sampling_limit: u32,
        pub final_overlap_ratio: f64,
    }

    /// Result of the C++ ICP. Returned by value.
    ///
    /// `result_code` is `CCCoreLib::ICPRegistrationTools::RESULT_TYPE`:
    /// `0` = `ICP_NOTHING_TO_DO`, `1` = `ICP_APPLY_TRANSFO`, `>=100` = error.
    /// `r00..r22` is the 3×3 rotation matrix in **row-major** order.
    /// `tx, ty, tz` is the translation. `scale` is the optional
    /// uniform scale (1.0 for rigid).
    pub struct IcpResultCpp {
        pub result_code: i32,
        pub rms: f64,
        pub final_point_count: u32,
        pub scale: f64,
        pub r00: f64, pub r01: f64, pub r02: f64,
        pub r10: f64, pub r11: f64, pub r12: f64,
        pub r20: f64, pub r21: f64, pub r22: f64,
        pub tx: f64, pub ty: f64, pub tz: f64,
    }

    unsafe extern "C++" {
        include!("src/cpp/icp_shim.h");

        /// Run ICP on the C++ side. Both point clouds are passed as
        /// flat `f32` arrays in the layout `[x0, y0, z0, x1, y1, z1, ...]`.
        /// The C++ side copies them into temporary `CCCoreLib::PointCloud`
        /// instances, calls `ICPRegistrationTools::Register`, and returns
        /// the recovered transform.
        ///
        /// `unsafe` is required because the arguments are raw pointers
        /// (the caller must guarantee the array lengths and lifetimes).
        unsafe fn run_icp_cpp(
            model_points: *const f32,
            model_count: usize,
            data_points: *const f32,
            data_count: usize,
            params: &IcpParamsCpp,
        ) -> IcpResultCpp;
    }
}

/// High-level wrapper around the C++ ICP. Safe to call from any
/// Rust code. Returns `None` only if the C++ ICP returns an
/// **error** code (>= 100). For success (1 = ICP_APPLY_TRANSFO)
/// and for "nothing to do" (0 = ICP_NOTHING_TO_DO), returns
/// `Some(result)` so the caller can still inspect the recovered
/// (or implicit identity) transform.
#[cfg(feature = "cxx_ffi")]
pub fn run_icp_cpp(
    model_points: &[f32],
    data_points: &[f32],
    params: &self::ffi_bridge::IcpParamsCpp,
) -> Option<self::ffi_bridge::IcpResultCpp> {
    use self::ffi_bridge::run_icp_cpp as ffi_run;
    let result = unsafe {
        ffi_run(
            model_points.as_ptr(),
            model_points.len(),
            data_points.as_ptr(),
            data_points.len(),
            params,
        )
    };
    // result_code 0 (ICP_NOTHING_TO_DO) and 1 (ICP_APPLY_TRANSFO)
    // are both non-error returns. >= 100 are errors.
    if result.result_code < 100 {
        Some(result)
    } else {
        None
    }
}

#[cfg(feature = "cxx_ffi")]
impl Default for self::ffi_bridge::IcpParamsCpp {
    /// Defaults: `minRMSDecrease=1e-5`, `nbMaxIterations=20`,
    /// `adjustScale=false`, `samplingLimit=50000`, `finalOverlapRatio=1.0`.
    /// Matches the C++ `ICPRegistrationTools::Parameters()` default
    /// constructor.
    fn default() -> Self {
        Self {
            min_rms_decrease: 1.0e-5,
            nb_max_iterations: 20,
            adjust_scale: false,
            sampling_limit: 50000,
            final_overlap_ratio: 1.0,
        }
    }
}
