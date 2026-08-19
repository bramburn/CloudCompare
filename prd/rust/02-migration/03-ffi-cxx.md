# CXX FFI Deep-Dive — CloudCompare Integration
**Date:** 2026-08-19
**Source:** CXX official docs (cxx.rs) + Perplexity 2026 research

---

## 1. CXX at a Glance

CXX (by David Tolnay, `cxx.rs`) is the production-standard FFI crate for C++/Rust interop. Key facts:

- **License:** Apache 2.0 / MIT
- **Minimum Rust:** 1.73+
- **Minimum C++:** C++11
- **Performance:** Zero-copy — no serialization, no memory allocation on the bridge
- **Maintenance:** Active (last release: 2025), used in production at JetBrains, Cloudflare, Discord, Google
- **C++ build:** CMake integration via `cxx-build` crate

---

## 2. CXX vs Bindgen vs Manual FFI

| Approach | Safety | Boilerplate | C++ idioms | Performance |
|----------|--------|-------------|-----------|-------------|
| **CXX** | ✅ Safe (checked at compile time) | Low | ✅ Full support | Zero-copy |
| **Bindgen** | ⚠️ Raw `unsafe` | Medium | ⚠️ Limited to C types | Zero-copy |
| **Manual FFI** | ❌ Manual discipline | High | ❌ C-style only | Zero-copy |
| **cbindgen** | ⚠️ Raw `unsafe` | Medium | ❌ C-only output | Zero-copy |

**CXX is the only approach that provides compile-time safety guarantees at the language boundary.** This is why all major production migrations use it.

---

## 3. CXX Bridge Declaration — Full Example

```rust
// cc-rust/src/lib.rs

#[cxx::bridge]
mod ffi {
    // ============================================================
    // RUST → C++: Rust types exposed to C++
    // ============================================================
    extern "Rust" {
        // ScalarField computation
        fn rust_compute_mean(values: &[f32]) -> f64;
        fn rust_compute_std(values: &[f32], mean: f64) -> f64;
        fn rust_count_valid(values: &[f32]) -> usize;
        fn rust_apply_offset(values: &mut [f32], offset: f64);

        // ICP registration
        fn rust_icp_register(
            model_points: &[f32],   // flattened: [x0,y0,z0,x1,y1,z1,...]
            data_points: &mut [f32],
            params: &IcprParams,
        ) -> Result<IcprResult, IcprError>;

        // Octree build
        fn rust_build_octree(
            points: &[f32],
            params: &OctreeParams,
            progress_cb: *mut c_void,  // raw pointer for callback
        ) -> Result<OctreeData, OctreeError>;

        // Type declarations
        type IcprParams;
        type IcprResult;
        type OctreeParams;
        type OctreeData;
    }

    // ============================================================
    // C++ → RUST: C++ types exposed to Rust
    // ============================================================
    unsafe extern "C++" {
        include!("CCCoreTypes.h");
        include!("DgmOctree.h");
        include!("ScalarField.h");

        type CxxScalarField;          // wraps ScalarField*
        type CxxGenericCloud;         // wraps GenericIndexedCloud*
        type CxxProgressCallback;      // wraps GenericProgressCallback*

        // C++ functions callable from Rust
        fn CxxScalarField_getSize(self: &CxxScalarField) -> usize;
        fn CxxScalarField_getAt(self: &CxxScalarField, i: usize) -> f32;
        fn CxxGenericCloud_getPoint(self: &CxxGenericCloud, i: usize) -> &[f32; 3];
        fn CxxProgressCallback_update(cb: &mut CxxProgressCallback, progress: f64) -> bool;
    }
}
```

---

## 4. CXX Type Support Matrix

| C++ Type | CXX Support | Rust Type | Notes |
|----------|-------------|-----------|-------|
| `bool` | ✅ | `bool` | |
| `int32_t`, `uint32_t` | ✅ | `i32`, `u32` | |
| `int64_t`, `uint64_t` | ✅ | `i64`, `u64` | |
| `float`, `double` | ✅ | `f32`, `f64` | |
| `std::string` | ✅ | `String` | CXX converts automatically |
| `std::vector<T>` | ✅ | `Vec<T>` | Zero-copy slice (`&[T]`) |
| `std::array<T,N>` | ✅ | `[T; N]` | |
| `std::unique_ptr<T>` | ✅ | `Box<T>` | |
| `std::shared_ptr<T>` | ✅ | `Arc<T>` | |
| `std::map<K,V>` | ❌ | | Serialize or use custom |
| `std::set<T>` | ❌ | | Serialize or use custom |
| `std::list<T>` | ❌ | | Not idiomatic in either language |
| Raw `T*` | ✅ | `*mut T` | `unsafe` on Rust side |
| Raw `const T*` | ✅ | `*const T` | `unsafe` on Rust side |
| C++ class (opaque) | ✅ | `type Foo;` | Opaque handle |
| C++ struct (shared) | ✅ | `#[repr(C)]` struct | Must be trivially copyable |
| C++ enum | ✅ | Rust enum | With `#[repr(C)]` |
| Virtual dispatch | ⚠️ | Not directly | Use extern "C" shim |
| Qt classes | ❌ | | Cannot cross FFI boundary |

---

## 5. Opaque Types — The Key Pattern for CCCoreLib

Most CCCoreLib classes are **opaque** to Rust — Rust sees them as handles, never the internal structure:

```cpp
// C++ side — GenericCloud.h
class GenericCloud {
public:
    virtual unsigned size() const = 0;
    virtual CCVector3 getPoint(unsigned i) const = 0;
    // ... ~20 more methods
};
```

```rust
// Rust side
#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        type GenericCloud;  // Opaque — Rust can't see inside

        fn getPoint(self: &GenericCloud, i: usize) -> &[f32; 3];
        fn size(self: &GenericCloud) -> usize;
    }
}
```

**Why opaque:** Rust doesn't need to know `GenericCloud`'s internal layout. It only calls the methods. The C++ side is responsible for the vtable and lifetime.

---

## 6. Pin<&mut T> for C++ Objects with Internal State

CXX requires `Pin<&mut T>` for C++ objects that may not support being moved in memory:

```rust
// Required when C++ type has internal pointers or virtual methods
fn mutate_progress_cb(cb: Pin<&mut CxxProgressCallback>, progress: f64) -> bool {
    // Pin<&mut T> guarantees the object won't be moved
    // CXX enforces this at compile time
}
```

This is required for `GenericProgressCallback` since it has virtual methods.

---

## 7. Ownership Transfer Patterns

### 7.1 C++ → Rust (Rust owns the data)

```cpp
// C++: creates a buffer, transfers ownership to Rust
std::vector<float> buffer = ComputeBuffer();
rust_process_buffer(buffer);  // CXX converts std::vector → Vec
// Rust now owns buffer; C++ buffer is moved-from (valid but empty)
```

```rust
// Rust: Vec is moved in
fn rust_process_buffer(buffer: Vec<f32>) {
    // buffer is owned by this function
    // automatically freed when function returns (Drop)
}
```

### 7.2 Rust → C++ (C++ owns the data)

```rust
// Rust: creates data, transfers ownership to C++
fn create_octree_data() -> *mut OctreeData {
    let data = Box::new(OctreeData::new());
    Box::into_raw(data)  // leaks from Rust's perspective; C++ now owns it
}
```

```cpp
// C++: receives raw pointer, takes ownership
OctreeData* raw = rust_create_octree_data();
std::unique_ptr<OctreeData> owned(raw);  // C++ manages lifetime now
// When unique_ptr goes out of scope, Rust's Box::drop is called (via extern "C" shim)
```

### 7.3 Shared Ownership (Most Common)

```rust
// Rust: creates data shared with C++
fn create_shared_octree() -> Arc<OctreeData> {
    Arc::new(OctreeData::new())
}
```

```cpp
// C++: receives Arc (CXX converts Arc → std::shared_ptr)
auto octree = rust_create_shared_octree();
// C++ and Rust share ownership via shared_ptr
// Last one to drop frees
```

---

## 8. Panic Handling Across FFI

**Critical:** Unwinding across FFI boundaries is **Undefined Behaviour** in both C++ and Rust.

```rust
#[no_mangle]
extern "C" fn safe_scalar_compute_mean(
    values_ptr: *const f32,
    len: usize,
) -> f64 {
    // Catch panics — prevents UB if Rust code panics
    let result = std::panic::catch_unwind(|| {
        let values = unsafe { std::slice::from_raw_parts(values_ptr, len) };
        compute_mean(values)
    });

    match result {
        Ok(v) => v,
        Err(_) => {
            // Log the panic
            eprintln!("Rust panic in scalar_compute_mean");
            // Return NaN to signal error — caller checks for NaN
            std::f64::NAN
        }
    }
}
```

**CXX auto-handles this** for safe signatures (no manual `catch_unwind` needed). You only need it for `#[no_mangle] extern "C"` functions.

---

## 9. Thread Safety with CXX

CXX does **not** assume `Send`/`Sync` on `extern "C++"` types. Explicit opt-in:

```rust
// In the CXX bridge:
unsafe extern "C++" {
    type GenericProgressCallback;
    type DgmOctree;  // NOT implicitly Send/Sync
}

// In src/threading.rs:
unsafe impl Send for ffi::DgmOctree {}
unsafe impl Sync for ffi::DgmOctree {}

// Only do this if you're CERTAIN the C++ type is thread-safe
// For ICP context: DataCloud contains raw pointers — DO NOT mark as Sync
```

**Rule:** Default to NOT marking CXX types as `Send`/`Sync`. Only opt in after auditing the C++ implementation.

---

## 10. CMake Integration for CloudCompare

```cmake
# CCCoreLib/CMakeLists.txt additions

# FetchContent for CXX
include(FetchContent)
FetchContent_Declare(cxx
    GIT_REPOSITORY https://github.com/dtolnay/cxx.git
    GIT_TAG        v1.0.186
    GIT_SHALLOW    TRUE
    FIND_PACKAGE_ARGS NAMES cxx
)
FetchContent_MakeAvailable(cxx)

# The CXX build script generates C++ glue code
# Add to cc-rust/build.rs:
#   fn main() {
#       cxx_build::bridge("src/lib.rs")
#           .file("src/cc_rust.cc")   # CXX-generated C++ glue
#           .compile("cc_rust");
#   }

# Build Rust library
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/cc_rust/src/lib.rs.gen
    COMMAND cargo generate-lockfile --manifest-path ${CMAKE_SOURCE_DIR}/cc-rust/Cargo.toml
    COMMAND cargo build --release
        --manifest-path ${CMAKE_SOURCE_DIR}/cc-rust/Cargo.toml
        --target-dir    ${CMAKE_BINARY_DIR}/cc_rust
        --target        x86_64-pc-windows-msvc
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/cc_rust/src/lib.rs.gen
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/cc-rust
    COMMENT "Building cc-rust library"
    VERBATIM
)
add_custom_target(cc_rust_build ALL DEPENDS ${CMAKE_BINARY_DIR}/cc_rust/src/lib.rs.gen)

# Link into CCCoreLib
add_dependencies(CCCoreLib cc_rust_build)
target_link_libraries(CCCoreLib PRIVATE
    ${CMAKE_BINARY_DIR}/cc_rust/x86_64-pc-windows-msvc/release/cc_rust.lib
)
```

---

## 11. Windows MSVC Toolchain Setup

```powershell
# Install Rust for MSVC on Windows
rustup target add x86_64-pc-windows-msvc
rustup default stable-x86_64-pc-windows-msvc

# Verify:
cargo build --target x86_64-pc-windows-msvc --release

# In cc-configure.cmd, add:
#   set RUST_TARGET=x86_64-pc-windows-msvc
#   set CC_RUST_TARGET_DIR=%BUILD_DIR%\cc_rust
```

---

## 12. CXX Gotchas for CloudCompare

### 12.1 Qt Virtual Methods
Qt classes with virtual methods cannot be exposed through CXX directly. Solution: create a C-only adapter class that wraps the Qt virtual calls.

### 12.2 `ccHObject` Tree
`ccHObject` is deeply tied to Qt's object model. It cannot cross the FFI boundary. Solution: copy data across, never share Qt object references.

### 12.3 Eigen Matrices
CXX does not natively support Eigen. Use `nalgebra` for Rust-side math, and convert at the FFI boundary:
```cpp
// C++ side
Eigen::Matrix3d matrix = /* ... */;
rust_process_matrix(matrix.data(), matrix.rows(), matrix.cols());
```
```rust
fn rust_process_matrix(data: &[f64], rows: usize, cols: usize) {
    let mat = nalgebra::DMatrix::from_slice(data, rows, cols);
    // use nalgebra here
}
```

### 12.4 `std::function` / `std::bind` Callbacks
CXX doesn't support `std::function` across the bridge. Use raw function pointers + user data instead.
