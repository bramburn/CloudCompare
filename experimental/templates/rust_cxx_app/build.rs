//! CXX build script — only runs when the `cxx-ffi` feature is enabled.

#[cfg(feature = "cxx-ffi")]
fn build_cxx() {
    use cxx_build::CxxBuild;

    let mut bridge: CxxBuild = cxx_build::bridge("src/lib.rs");
    bridge
        .file("cpp/bridge.cc")
        // cxx-build's generated `lib.rs.cc` does `#include "cpp/bridge.h"`,
        // so we add the project root (the cargo manifest dir) to the include
        // path. This makes `cpp/bridge.h` resolvable.
        .include(".")
        .flag_if_supported("/std:c++17")
        .flag_if_supported("/EHsc")
        .compile("rust_cxx_app_template");

    println!("cargo:rustc-link-lib=static=rust_cxx_app_template");
}

fn main() {
    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=src/main.rs");
    println!("cargo:rerun-if-changed=cpp/bridge.cc");
    println!("cargo:rerun-if-changed=cpp/bridge.h");

    #[cfg(feature = "cxx-ffi")]
    build_cxx();
}
