// build.rs — CloudCompare Rust Sandbox
//
// Pure-Rust build: no C++ needed, no CXX bridge.
// The CXX FFI layer to CCCoreLib will be added separately once
// CCCoreLib is configured (see CONFIGURE_CCCORELIB.md).
//
// To enable the C++ FFI layer:
//   1. Configure CCCoreLib: see sandbox/CONFIGURE_CCCORELIB.md Track B
//   2. cmake --build external/CCCoreLib_build --parallel 8
//   3. Add back the cxx_build::bridge() code and re-run this build

fn main() {
    let manifest_dir = std::env::var("CARGO_MANIFEST_DIR").unwrap();
    let sandbox_root = std::path::Path::new(&manifest_dir);

    // ── Check for CCCoreLib (for future CXX FFI layer) ─────────────
    let cc_build_dir = sandbox_root.join("external/CCCoreLib_build");
    let cc_lib_path = cc_build_dir.join("lib").join("CCCoreLib.lib");

    if cc_lib_path.exists() {
        println!("cargo:warning=CCCoreLib found — C++ FFI layer can now be enabled");
        println!("cargo:warning=Add cxx_build::bridge() back to this build.rs to enable it");
    } else {
        println!("cargo:warning=Pure-Rust build — no C++ needed");
        println!("cargo:warning=To add CCCoreLib FFI: see sandbox/CONFIGURE_CCCORELIB.md");
    }

    // ── Rebuild triggers ──────────────────────────────────────────────────
    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=src/scalar_field.rs");
    println!("cargo:rerun-if-changed=src/registration.rs");
    println!("cargo:rerun-if-changed=src/octree.rs");
    println!("cargo:rerun-if-changed=src/io.rs");
    println!("cargo:rerun-if-changed=build.rs");
}
