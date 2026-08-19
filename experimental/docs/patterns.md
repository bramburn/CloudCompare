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

## P12. SVD rotation order — nalgebra's `svd.v_t` is V^T

**Pattern:** When computing the Horn 1987 rotation R = V · U^T from
an SVD of a 3×3 cross-covariance H, remember that nalgebra's
`svd.v_t` is the right-singular vectors **transposed** (V^T, not V).
The correct R is therefore:

```rust
let v_t = svd.v_t.ok_or(...)?;
let u = svd.u.ok_or(...)?;
let r = v_t.transpose() * u.transpose();  // R = V · U^T
```

**Why it matters:** using `v_t * u.transpose()` (without the inner
`.transpose()`) gives V^T · U^T = (U·V)^T, which is the wrong matrix.
For a full-rank H, the result is a mirror/reflection, not a rotation.
For a rank-deficient H (degenerate fixture, e.g. a translated cube),
the SVD has no unique answer for the missing axes and the result
becomes implementation-dependent.

**Symptom of the bug:** ICP converges in 1 iteration to a sensible
result, then diverges in iteration 2 because the reflection undoes
the alignment.

**Source:** `cc-rust/src/registration.rs` (D4, fix landed 2026-08-19).

---

## P13. ICP with in-place data must NOT apply cumulative transform in NN search

**Pattern:** Two styles of ICP — non-mutating (keep `data_initial`,
track cumulative `(R, t)`) and in-place (mutate `data` each iteration).
Pick **one**. The bug is mixing them:

```rust
// WRONG — double-transformation
for i in 0..n_data {
    let pt = data[i];                              // already transformed
    let transformed = rotation * pt + translation; // transforms AGAIN
    nearest_neighbour(model, &transformed);
}
```

```rust
// RIGHT — in-place style
for i in 0..n_data {
    let pt = data[i];  // data is already at the current pose
    nearest_neighbour(model, &pt);
}
```

**Why it matters:** the data array IS the current pose in the
in-place style. Applying the cumulative `(rotation, translation)` to
the already-transformed data double-rotates and double-translates,
which sends the correspondences into a region where the next ΔR
makes things worse, not better.

**Symptom of the bug:** ICP gets the right answer in iteration 1,
then the cumulative transform "skips ahead" of the actual data state
and iteration 2 finds nonsense correspondences. RMS diverges.

**Source:** `cc-rust/src/registration.rs` (D4, fix landed 2026-08-19).

---

## P14. Test fixtures for SVD-based algorithms must be non-symmetric

**Pattern:** When writing characterisation tests for any algorithm
that uses SVD (rotation estimation, point-set registration,
procrustes analysis), the input cloud must be **non-symmetric**
along all three axes. Pure-cube corners are a classic trap.

The 8 corners of the unit cube are at (0/1, 0/1, 0/1). Translating
this cube by any axis-aligned vector (e.g. (1, 0, 0)) gives a
matched-pair distribution where the cross-covariance H has a
zero column/row in the direction of the translation:

```text
H = Σ (data_i − c_data)(model_i − c_model)^T
  = diag(0, 2, 2)    for a +X translation of the cube
```

H is rank 2 in 3D. The SVD has infinitely many valid decompositions
because the missing axis can take any sign. nalgebra picks one
deterministically but it is, in general, a reflection rather than
a rotation.

**Fix:** add at least one off-axis point to break the symmetry. The
"asymmetric-9" fixture in `experimental/fixtures/synthetic/asymmetric-9.toml`
is the canonical example: 8 cube corners plus one point at
(1.5, 0.3, 0.7). The off-axis point makes H full-rank, so the SVD
returns a unique rotation.

**Symptom of the bug:** ICP tests pass on paper (the test "asserts"
the right answer) but the recovered transform has a +/−X flip that
the assertion doesn't catch because the test only checks magnitude
or RMS, not the sign of individual components.

**Source:** `experimental/fixtures/synthetic/asymmetric-9.toml`,
`cc-rust/src/registration.rs::tests::asymmetric_cloud` (D4, fixture
swap landed 2026-08-19).

---

## P15. PCA pre-alignment: degenerate on near-symmetric clouds

**Pattern:** Coarse pre-alignment via principal component analysis
(centroid + eigenvectors of the covariance matrix) gives ICP a
much better starting point when the initial rotation is large
(30°+). But PCA only works well when the cloud has **distinct
eigenvalues** — i.e. it must be clearly elongated along one
axis.

For a symmetric cloud (e.g. the unit cube), all three
eigenvalues are equal, and the principal axes are not unique.
The SVD-based axis alignment can land on a 180° rotation
around any axis. The coarse alignment gets you to RMS ≈
the cloud's "radius" — i.e. useless.

**Symptom of the bug:** the coarse-aligned cloud has RMS equal
to the cloud's diagonal, even after running ICP for many
iterations. The ICP loop is escaping the local minimum but
slowly.

**Fix:** use a deliberately asymmetric cloud (e.g. an "L" with
arms of clearly different lengths). For real-world data
(building scans, terrain), the cloud is usually asymmetric
and PCA works fine. For ICP on point clouds that *could* be
symmetric, prefer ICP variants that search over rotations
(e.g. Go-ICP, 4-point congruent set) over pure PCA.

**Source:** `cc-rust/src/coarse_align.rs` (test
`coarse_align_handles_rotation` uses an L-shaped cloud; the
`asymmetric-9` cloud used by ICP tests is too symmetric for
PCA to recover rotations beyond ~5°).

**Pattern:** When writing characterisation tests for any algorithm
that uses SVD (rotation estimation, point-set registration,
procrustes analysis), the input cloud must be **non-symmetric**
along all three axes. Pure-cube corners are a classic trap.

The 8 corners of the unit cube are at (0/1, 0/1, 0/1). Translating
this cube by any axis-aligned vector (e.g. (1, 0, 0)) gives a
matched-pair distribution where the cross-covariance H has a
zero column/row in the direction of the translation:

```text
H = Σ (data_i − c_data)(model_i − c_model)^T
  = diag(0, 2, 2)    for a +X translation of the cube
```

H is rank 2 in 3D. The SVD has infinitely many valid decompositions
because the missing axis can take any sign. nalgebra picks one
deterministically but it is, in general, a reflection rather than
a rotation.

**Fix:** add at least one off-axis point to break the symmetry. The
"asymmetric-9" fixture in `experimental/fixtures/synthetic/asymmetric-9.toml`
is the canonical example: 8 cube corners plus one point at
(1.5, 0.3, 0.7). The off-axis point makes H full-rank, so the SVD
returns a unique rotation.

**Symptom of the bug:** ICP tests pass on paper (the test "asserts"
the right answer) but the recovered transform has a +/−X flip that
the assertion doesn't catch because the test only checks magnitude
or RMS, not the sign of individual components.

**Source:** `experimental/fixtures/synthetic/asymmetric-9.toml`,
`cc-rust/src/registration.rs::tests::asymmetric_cloud` (D4, fixture
swap landed 2026-08-19).

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
