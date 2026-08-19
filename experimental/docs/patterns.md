# Patterns — "How we do X"

> Reusable patterns. If a session teaches us a new pattern, append it here.
> One page per pattern. Link out to the originating session.

---

## P1. Characterisation tests against CCCoreLib

**Pattern:** When porting a C++ algorithm to Rust, write characterisation tests
that pin Rust's behaviour to the C++ source. The "right answer" is what
CCCoreLib does, not what statistical folklore says.

**Steps:**

1. Read the C++ source for the function. Note the exact formula and any
   special-case handling (NaN, infinity, sentinel values).
2. Implement the function in Rust.
3. Write tests that exercise:
   - The basic case
   - Edge cases (empty, single, all-invalid)
   - The cases that caught you out the first time (e.g. NaN)
4. Document the C++ formula in a comment above each Rust function.
5. If a Rust test fails because the formula is different, the **Rust code is
   wrong** until proven otherwise — investigate, don't relax the test.

**Example from this repo:**

- `sessions/2026-08-19-rust-migration-icp-scalarfield/src/scalar_field.rs`
- `docs/decisions/2026-08-19-rust-scalarfield-formulas.md`

**Anti-pattern:** writing `assert!((rust - expected).abs() < 1e-6)` and
"tuning" the test until it passes. The expected value comes from the C++
source. If the test fails, the formula is wrong.

---

## P2. vcvars64 environment capture

**Pattern:** Capturing MSVC environment variables from PowerShell so
Rust/CXX builds work.

**Why:** Rust's GNU toolchain (x86_64-pc-windows-gnu) is the default; it
expects `g++.exe`. CCCoreLib and Qt require `cl.exe`. Without the right
environment, `link-cplusplus` (used by `cxx-build`) fails with cryptic
errors.

**How:** Source `experimental/shared/scripts/get-vcvars.ps1` at the top of
your build script. Then set `CC=cl.exe` and `CXX=cl.exe` (and
`CXXFLAGS=/std:c++17 /EHsc` for CXX).

**Source:** `experimental/shared/scripts/get-vcvars.ps1`

**Verified on:** Windows Server 2019, VS 2022 Community 17.14, MSVC 14.44,
Windows SDK 10.0.26100.0.

---

## P3. Windows SDK lib path for Qt OpenGLWidgets

**Pattern:** When linking `Qt6::OpenGLWidgets` with MSVC, the linker
needs to find `d3d11.lib`, `dxgi.lib`, `d3d12.lib` from the Windows SDK.

**Why:** The MSVC linker's default library search path doesn't include
the Windows SDK. vcpkg's toolchain file interferes with
`CMAKE_SYSTEM_LIBRARY_PATH`, so the usual CMake workarounds don't apply.

**How:** In your `CMakeLists.txt`, before the `target_link_libraries`
call:

```cmake
if(MSVC AND EXISTS "$ENV{WindowsSdkDir}/Lib/$ENV{WindowsSDKVersion}/um/x64/d3d11.lib")
    add_link_options("/LIBPATH:\"$ENV{WindowsSdkDir}/Lib/$ENV{WindowsSDKVersion}/um/x64\"")
endif()
```

Requires that `$env:WindowsSdkDir` and `$env:WindowsSDKVersion` are set —
sourcing `get-vcvars.ps1` does this.

**Source:** `qCC/test/CMakeLists.txt`, `experimental/templates/cpp_qt_gui/CMakeLists.txt`

---

## P4. CXX FFI — `unsafe extern "C++"` for safe-to-call functions

**Pattern:** In a `#[cxx::bridge]` module, the `extern "C++"` block must
be marked `unsafe extern "C++"` if it contains any function declarations,
even "safe" ones.

**Why:** CXX 1.0 requires this for all C++ blocks. Without it, cxx-build
fails with "block must be declared `unsafe extern "C++"`".

**How:**

```rust
#[cxx::bridge]
pub mod ffi {
    unsafe extern "C++" {
        include!("cpp/bridge.h");
        fn my_cpp_function(x: i32) -> i32;
    }
}
```

**Source:** `experimental/templates/rust_cxx_app/src/lib.rs`

---

## P5. CXX header location — the auto-generated `lib.rs.h`

**Pattern:** CXX 1.0 generates the FFI header at
`target/.../cxxbridge/include/<crate>/src/lib.rs.h`. Your `cpp/bridge.h`
must include this with that exact path (note the `src/` between crate
name and the file).

**How:**

```cpp
// cpp/bridge.h
#pragma once
#include "<crate>/src/lib.rs.h"  // brings in ::rust::cxxbridge1::String, ::rust::cxxbridge1::Str

::rust::cxxbridge1::String my_func(::rust::cxxbridge1::Str input);
```

**Source:** `experimental/templates/rust_cxx_app/cpp/bridge.h`

---

## P6. Staticlib + rlib for hybrid Rust crates

**Pattern:** When you want a Rust crate to be both linkable from C++
(via CXX) and usable from a Rust binary in the same crate, use
`crate-type = ["staticlib", "rlib"]`.

**Why:** Without `rlib`, the bin target can't `use crate_name::...` —
the crate is only available as a static library.

**How:**

```toml
[lib]
name = "my_crate"
path = "src/lib.rs"
crate-type = ["staticlib", "rlib"]
```

**Source:** `experimental/templates/rust_cxx_app/Cargo.toml`

---

## P7. CXX FFI is opt-in (feature flag) for portability

**Pattern:** When a template's default build doesn't need CXX, gate
CXX behind a feature flag. The default `cargo test` then works on any
toolchain; CXX is opt-in for when you actually need FFI.

**Why:** CXX requires the MSVC toolchain. If the template's default
build needs CXX, every session that uses the template needs MSVC, even
for pure-Rust experiments.

**How:**

```toml
[features]
default = []
cxx-ffi = ["dep:cxx"]

[dependencies]
cxx = { version = "1.0", optional = true }

[build-dependencies]
cxx-build = { version = "1.0", optional = true }
```

```rust
// build.rs
#[cfg(feature = "cxx-ffi")]
fn build_cxx() { /* ... */ }

fn main() {
    #[cfg(feature = "cxx-ffi")]
    build_cxx();
}
```

**Source:** `experimental/templates/rust_cxx_app/`

---

## P9. `glEnable(GL_PROGRAM_POINT_SIZE)` for point clouds in OpenGL Core

**Pattern:** When rendering `GL_POINTS` in OpenGL Core profile (Qt 6's
`QOpenGLWidget` defaults to Core on Windows), `gl_PointSize` set in the
vertex shader is **silently ignored** unless you call
`glEnable(GL_PROGRAM_POINT_SIZE)` in `initializeGL()`.

**Symptom:** the points render at the hardware's minimum size (typically
1.0 pixel). A 2000-point cloud looks like a faint dotted outline instead
of visible dots.

**Why:** OpenGL Core profile (vs. Compatibility profile) removed the
fixed-function point-size state. Without `GL_PROGRAM_POINT_SIZE`, drivers
clamp the size to 1.0 regardless of the shader.

**How:** in your `QOpenGLWidget::initializeGL()` (or wherever you set up
GL state):

```cpp
glEnable(GL_PROGRAM_POINT_SIZE);  // ← required for vertex-shader gl_PointSize
```

Optional: also set the value dynamically via a uniform so you can scale
point size with camera distance:

```glsl
// vertex shader
uniform float uPointSize;
void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    gl_PointSize = uPointSize;  // 2.0 - 12.0 pixels feels right
}
```

```cpp
// in C++
glUniform1f(uPointSize_loc, std::clamp(80.0f / std::max(0.5f, distance_), 2.0f, 12.0f));
```

**Source:** `experimental/templates/cpp_qt_gui/src/pointcloudview.cpp`

---

## P10. QOpenGLWidget mouse event delivery

**Pattern:** `QOpenGLWidget` doesn't always receive mouse events out of
the box. Three things to set in the constructor:

1. `setMouseTracking(true)` — needed for hover events (less critical for
   press/drag but still good practice)
2. `setFocusPolicy(Qt::StrongFocus)` — without this, keyboard and wheel
   events may go elsewhere after a click
3. `setFocus()` after adding to layout — give the widget initial focus so
   the first wheel/key event reaches it
4. In `mousePressEvent`, call `setFocus(Qt::MouseFocusReason)` — keeps
   focus locked even if the user clicks on the widget after a non-focus
   action

**Symptom of missing these:** the viewport renders correctly but
left/right/middle drag, wheel, and keyboard shortcuts do nothing. The
status bar still shows "View reset" because the button works.

**Source:** `experimental/templates/cpp_qt_gui/src/pointcloudview.cpp`

---

## P11. Pan math: use camera right/up vectors, not MVP inversion

**Pattern:** Converting a 2D pixel delta to a 3D world-space pan offset
should use the camera's local right and up vectors, not an inverse MVP
multiply. The MVP inversion trick gives weird results because it's
projecting through perspective division.

**How:**

```cpp
const float yaw_rad = qDegreesToRadians(yaw_);
const float pitch_rad = qDegreesToRadians(pitch_);
const QVector3D forward(std::cos(pitch_rad) * std::sin(yaw_rad),
                        -std::sin(pitch_rad),
                        -std::cos(pitch_rad) * std::cos(yaw_rad));
const QVector3D world_up(0, 1, 0);
const QVector3D right = QVector3D::crossProduct(forward, world_up).normalized();
const QVector3D up = QVector3D::crossProduct(right, forward).normalized();

const float pan_amount = distance_ * 0.002f;  // scale with zoom
pan_offset_ += right * (float)delta.x() * pan_amount;
pan_offset_ -= up * (float)delta.y() * pan_amount;
```

**Source:** `experimental/templates/cpp_qt_gui/src/pointcloudview.cpp`

---

## P8. Experimental session lifecycle

**Pattern:** Every experiment follows the same lifecycle. Don't skip steps.

1. **Search first.** Read `docs/index.md`, `docs/patterns.md`, `docs/decisions.md`.
2. **Pick a template.** `rust_lib`, `rust_cxx_app`, `cpp_qt_console`, `cpp_qt_gui`.
3. **Scaffold:** `cp -r templates/<chosen> sessions/<YYYY-MM-DD>-<topic>/`
4. **Implement minimally.** Throwaway but buildable.
5. **Build & test.** The session must end green.
6. **Document.** Update `docs/index.md` and either `docs/patterns.md` or
   `docs/decisions.md` (or both).

**Source:** `experimental/AGENTS.md`

---

## Adding a new pattern

When you find a pattern that:

- Solves a recurring problem
- Took more than 30 minutes to figure out
- Has a "right way" to do it (rather than "it works on my machine")

…add it here. One page, link to the originating session, link to the
code that uses it.

Don't add:

- Trivial things (one-line answers)
- Things that are well-documented elsewhere (link to the official docs)
- Hypothetical patterns ("we might want to do X someday")
