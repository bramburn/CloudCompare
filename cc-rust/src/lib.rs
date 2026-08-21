//! cc_rust — Rust migration of CCCoreLib compute kernels.
//!
//! The migration order (per `PRD/rust/05-roadmap.md`):
//!
//! - **Phase 0** (this commit): scaffolding, builds, runs
//! - **Phase 1**: ScalarField statistics — ported and characterised
//!   against CCCoreLib. Hybrid: sequential below 50K, rayon above.
//! - **Phase 2**: ICP / Horn registration — ported, KD-tree variant
//!   in progress. See `experimental/scenarios/2026-08-19-icp-variants/`
//! - **Phase 3**: DgmOctree / KdTree — variants A/B/C in
//!   `experimental/scenarios/2026-08-19-icp-variants/`
//! - **Phase 4**: File parsers (LAS, PLY) — not started
//!
//! All implementations are validated by characterisation tests
//! against the C++ source in
//! `libs/qCC_db/extern/CCCoreLib/src/ScalarField.cpp` and friends.
//!
//! **Build status:**
//! - Pure-Rust: builds and tests pass on stable Rust 1.89
//! - CXX FFI: opt-in via `cargo build --features cxx_ffi` (requires MSVC)

#![doc = include_str!("../docs/PHASES.md")]

pub mod scalar_field;
pub mod registration;
pub mod octree;
pub mod io;
pub mod coarse_align;
pub mod dgm_octree;

// CXX FFI bridge to CCCoreLib's `ICPRegistrationTools::Register`.
// Only compiled when the `cxx_ffi` feature is enabled (requires MSVC).
// See `src/ffi.rs` for what it does and the build setup.
#[cfg(feature = "cxx_ffi")]
pub mod ffi;

// Sentry crash reporting — opt-in via the `sentry` feature.
// Mirrors qCC's `CC_USE_SENTRY` opt-in. `cc_rust_cli` calls
// `sentry_init()` at the top of `main()` and holds the guard
// for the program's lifetime. See `src/sentry_init.rs`.
#[cfg(feature = "sentry")]
pub mod sentry_init;

pub use scalar_field::{
    mean, min_max, rms, std, valid_count, apply_offset, apply_scale,
    compute_stats, compare, ScalarStats, ScalarError,
};
pub use registration::{
    icp_iterate, icp_with_nn, icp_multi_resolution, icp_multi_resolution_with_nn,
    IcprParamsRust as IcprParams, IcprResultRust as IcprResult,
    IcprErrorRust as IcprError,
    NearestNeighbour, BruteForceNN,
};
pub use coarse_align::{
    coarse_align, apply_transform_in_place, CoarseAlignResult,
};
pub use dgm_octree::{
    DgmOctree, DgmOctreeNN, compute_cell_code, get_cell_pos, get_bit_shift,
    MAX_OCTREE_LEVEL,
};
