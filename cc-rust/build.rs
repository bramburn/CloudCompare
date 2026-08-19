//! cc_rust build script.
//!
//! The CXX bridge is gated behind the `cxx-ffi` feature flag. By default
//! this is a no-op so the build works on any toolchain (including
//! the GNU-flavored rustc on this machine, which has no MSVC).
//!
//! When `cargo build --features cxx-ffi` is used, the bridge compiles
//! a tiny C++ shim that calls into CCCoreLib. The shim currently is
//! a stub — see `docs/PHASES.md` for the Phase 0 → Phase 1 path.

fn main() {
    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=src/scalar_field.rs");
    println!("cargo:rerun-if-changed=src/registration.rs");
    println!("cargo:rerun-if-changed=src/octree.rs");
    println!("cargo:rerun-if-changed=src/io.rs");
    println!("cargo:rerun-if-changed=src/main.rs");

    #[cfg(feature = "cxx-ffi")]
    {
        use cxx_build::CxxBuild;
        // The C++ bridge will live at src/ffi.rs once Phase 0 is started.
        // For now, this branch is not exercised.
        let _bridge: CxxBuild = cxx_build::bridge("src/lib.rs");
        // TODO: file!("cpp/cc_rust_shim.cc") once the bridge is real.
    }
}
