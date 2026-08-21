//! cc_rust build script.
//!
//! The CXX bridge is gated behind the `cxx_ffi` feature flag. By default
//! this is a no-op so the build works on any toolchain (including
//! the GNU-flavored rustc on this machine, which has no MSVC).
//!
//! When `cargo build --features cxx_ffi` is used, the bridge compiles
//! a C++ shim (`src/cpp/icp_shim.cc`) that calls into
//! `CCCoreLib::ICPRegistrationTools::Register`. The shim is linked
//! against `CCCoreLib.lib` from the existing main CloudCompare build
//! (no standalone CCCoreLib build required).

use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=src/scalar_field.rs");
    println!("cargo:rerun-if-changed=src/registration.rs");
    println!("cargo:rerun-if-changed=src/octree.rs");
    println!("cargo:rerun-if-changed=src/io.rs");
    println!("cargo:rerun-if-changed=src/main.rs");
    println!("cargo:rerun-if-changed=src/ffi.rs");
    println!("cargo:rerun-if-changed=src/cpp/icp_shim.cc");
    println!("cargo:rerun-if-changed=src/cpp/icp_shim.h");

    #[cfg(feature = "cxx_ffi")]
    build_cxx();
}

#[cfg(feature = "cxx_ffi")]
fn build_cxx() {
    // Locate the existing CloudCompare build's CCCoreLib.lib / .dll.
    // Used both as a C++ include dir (for `<CCCoreLib.h>`) and as a
    // linker search path. The standalone CCCoreLib build is not
    // needed for the parity test.
    //
    // Override with `CCCORELIB_DIR=<path>` if the build is in a
    // non-standard location.
    let ccbuild: PathBuf = env::var("CCCORELIB_DIR")
        .map(PathBuf::from)
        .unwrap_or_else(|_| {
            // repo_root/cc-rust -> repo_root/build/libs/qCC_db/extern/CCCoreLib
            let mut p = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
            p.pop();
            p.push("build");
            p.push("libs");
            p.push("qCC_db");
            p.push("extern");
            p.push("CCCoreLib");
            p
        });

    // cxx 1.0.199 API: `cxx_build::bridge()` returns a `cc::Build`
    // builder. We call `.file()`, `.include()`, etc. on it, then
    // `.compile("static_lib_name")` to actually run the C++ compile.
    cxx_build::bridge("src/ffi.rs")
        .file("src/cpp/icp_shim.cc")
        // cxx-build's generated `ffi.rs.cc` does
        //   #include "src/cpp/icp_shim.h"
        // so we add the project root to the include path. This
        // makes `src/cpp/icp_shim.h` resolvable. The auto-generated
        // `cc_rust/src/ffi.rs.h` resolves via cxx-build's own
        // `OUT_DIR/cxxbridge/include` path.
        .include(".")
        // Add CCCoreLib's include dir so `<CCCoreLib.h>` etc. resolve.
        // `CCCoreLib.h` itself includes `CCCoreLibExport.h` (the
        // generated DLL export macros) which lives under
        // `<build>/libs/qCC_db/extern/CCCoreLib/exports/`.
        .include("../libs/qCC_db/extern/CCCoreLib/include")
        .include("../build/libs/qCC_db/extern/CCCoreLib/exports")
        .flag_if_supported("/std:c++17")
        .flag_if_supported("/EHsc")
        .compile("cc_rust_ffi");

    // Link the MSVC C++ runtime. Required on Windows because the
    // shim uses `<CCCoreLib.h>` which transitively includes standard
    // library headers. The `link_cplusplus` crate is a separate
    // dep in cxx 1.0.199 (it used to be inside `cxx_build`).
    println!("cargo:rustc-link-lib=dylib=msvcrt");

    // Link CCCoreLib (the existing CloudCompare build's import lib).
    println!("cargo:rustc-link-search=native={}", ccbuild.display());
    println!("cargo:rustc-link-lib=CCCoreLib");

    // ── Workaround for cargo not propagating link directives to test
    // targets on all toolchains. The cxx_build::bridge().compile()
    // call above only registers the static lib for the main library
    // target. `cargo test --features cxx_ffi` builds the test binary
    // with a separate rustc invocation that does NOT include
    // `-l static=cc_rust_ffi` or `-l CCCoreLib`, leading to unresolved
    // `cxxbridge1$199$run_icp_cpp` and `CCCoreLib.dll` import symbols
    // and a STATUS_DLL_NOT_FOUND crash at startup.
    //
    // The `links = "cc_rust_ffi"` attribute in Cargo.toml is the
    // cargo-supported way to opt every target (lib, bin, test,
    // example, bench) into receiving these link directives. With
    // that attribute set, the explicit re-emit here is a safety net
    // for older cargo versions that may not honor `links` correctly.
    println!("cargo:rustc-link-lib=static=cc_rust_ffi");

    // Copy CCCoreLib.dll next to the test binary so it can be found
    // at runtime. The test binary is in target/<profile>/deps/.
    let dll_src = ccbuild.join("CCCoreLib.dll");
    if dll_src.exists() {
        let out_dir = env::var("OUT_DIR").expect("OUT_DIR is set by cargo");
        let out_path = PathBuf::from(&out_dir);
        // Copy to OUT_DIR (in case the C++ side references it).
        let _ = std::fs::copy(&dll_src, out_path.join("CCCoreLib.dll"));
        // Copy to target/<profile>/deps/ for test binaries.
        if let Some(target_dir) = out_path.ancestors().nth(3) {
            let deps_dir = target_dir.join("deps");
            if deps_dir.exists() {
                let dll_dst_deps = deps_dir.join("CCCoreLib.dll");
                if let Err(e) = std::fs::copy(&dll_src, &dll_dst_deps) {
                    eprintln!("warning: could not copy {} -> {}: {}",
                        dll_src.display(), dll_dst_deps.display(), e);
                } else {
                    println!("cargo:rerun-if-changed={}", dll_src.display());
                }
            }
        }
    }
}
