# Template: rust_cxx_app

## Purpose

A **Rust + CXX FFI** template. The Rust crate compiles to a static library
(`crate-type = ["staticlib"]`) and a C++ implementation provides a bridge
function callable from Rust. CXX generates all the unsafe glue.

Use this when you want to:

- Call existing C++ from Rust (e.g. CCCoreLib functions, `ccPointCloud::addPoint`)
- Expose Rust functions to a C++ Qt GUI (combine with `cpp_qt_gui/`)
- Test that an FFI boundary is sound before integrating

## When NOT to use

- You don't actually need to cross the FFI boundary — use `rust_lib/`
- You need a full Qt GUI — use `cpp_qt_gui/` and link this crate as a staticlib

## Structure

```
rust_cxx_app/
├── Cargo.toml
├── build.rs                     ← cxx-build configuration
├── src/
│   ├── lib.rs                   ← #[cxx::bridge] module + Rust impls
│   └── main.rs                  ← CLI that exercises the bridge
├── cpp/
│   └── bridge.cc                ← C++ impl of `extern "C++"` declarations
├── tests/                       ← optional integration tests
├── AGENTS.md
└── README.md
```

## Build & run

The default is **pure-Rust only** — `cargo test` and `cargo run` work without
the MSVC toolchain. The CXX FFI layer is opt-in.

```powershell
# Default: pure-Rust, no C++. Works on any toolchain.
cargo test
cargo run --bin demo_cli -- 1 2 3 4 5

# Enable CXX FFI. Requires:
#   - MSVC toolchain (cl.exe, link.exe) — see shared/scripts/get-vcvars.ps1
#   - Set CC=cl.exe, CXX=cl.exe
#   - x86_64-pc-windows-msvc target (or compatible)
cargo test --features cxx-ffi
cargo build --features cxx-ffi
```

The `cxx-ffi` feature generates `cpp/bridge.h` and links C++ into the staticlib.
DO NOT commit `target/` or the generated `bridge.h`.

## How to extend

### Add a new C++ function callable from Rust

1. Declare it in the `extern "C++"` block in `src/lib.rs`:
   ```rust
   extern "C++" {
       include!("cpp/bridge.h");
       fn my_cpp_function(input: &str) -> String;
   }
   ```
2. Implement it in `cpp/bridge.cc`:
   ```cpp
   std::string my_cpp_function(const std::string& input) { ... }
   ```
3. `cargo build` regenerates `bridge.h` and links.

### Add a new Rust function callable from C++

1. Declare it in the `extern "Rust"` block in `src/lib.rs`:
   ```rust
   extern "Rust" {
       fn my_rust_function(x: f64) -> f64;
   }
   ```
2. Implement it as a public function in `src/lib.rs`:
   ```rust
   pub fn my_rust_function(x: f64) -> f64 { x * 2.0 }
   ```
3. From C++, call it as `my_rust_function(3.14)`.

### Add a new shared struct

```rust
struct Point3 { x: f64, y: f64, z: f64 }
```

CXX will generate the matching C++ struct in the auto-generated header.

## Common pitfalls

1. **MSVC + Qt6 link failures** — the build script's
   `/std:c++17 /EHsc` flags match MSVC defaults but may need adjustment if
   your C++ uses Qt. See the recipe in `cpp_qt_gui/AGENTS.md` for how
   `add_link_options("/LIBPATH:...")` solves the d3d11.lib problem.

2. **Function name collisions** — the `mod ffi` namespace + C++ namespace
   must match. Default is `crate_name`. Rename by changing `crate-type` in
   Cargo.toml and the `namespace` in cpp/bridge.cc.

3. **Forgetting `crate-type = ["staticlib"]`** — without this, the
   produced `.rlib` cannot be linked from C++.

## Related

- Sibling: `../rust_lib/` (no FFI)
- Consumer: `../cpp_qt_gui/` (Qt GUI that links to this as a staticlib)
- Workflow: `../../AGENTS.md`
