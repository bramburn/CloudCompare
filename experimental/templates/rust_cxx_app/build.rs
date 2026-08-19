//! CXX build script.
//!
//! The `#[cxx::bridge]` module lives in src/lib.rs. cxx-build compiles the
//! generated C++ glue (cpp/bridge.cc) and the CXX runtime, then links them
//! into the Rust crate. The resulting staticlib can be linked from a C++
//! Qt GUI or any other C++ binary.

fn main() {
    // Tell the MSVC linker where the Windows SDK libraries live.
    // (Required for cxx-build to resolve d3d11, dxgi, etc. when linking
    // against Qt6::OpenGLWidgets downstream.)
    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=cpp/bridge.cc");
    println!("cargo:rerun-if-changed=cpp/bridge.h");

    let mut bridge = cxx_build::bridge("src/lib.rs");
    bridge
        .file("cpp/bridge.cc")
        .flag_if_supported("/std:c++17")
        .flag_if_supported("/EHsc")
        .compile("rust_cxx_app_template");

    println!("cargo:rustc-link-lib=static=rust_cxx_app_template");
}
