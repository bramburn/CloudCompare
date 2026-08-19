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
//! - CXX FFI: opt-in via `cargo build --features cxx-ffi` (requires MSVC)

#![doc = include_str!("../docs/PHASES.md")]

pub mod scalar_field;
pub mod registration;
pub mod octree;
pub mod io;
pub mod coarse_align;
pub mod dgm_octree;

pub use scalar_field::{
    mean, min_max, rms, std, valid_count, apply_offset, apply_scale,
    compute_stats, compare, ScalarStats, ScalarError,
};
pub use registration::{
    icp_iterate, IcprParamsRust as IcprParams, IcprResultRust as IcprResult,
    IcprErrorRust as IcprError,
};
pub use coarse_align::{
    coarse_align, apply_transform_in_place, CoarseAlignResult,
};
pub use dgm_octree::{
    DgmOctree, compute_cell_code, get_cell_pos, get_bit_shift,
    MAX_OCTREE_LEVEL,
};
