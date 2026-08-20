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

---

## P16. 2026-08-20 — Pluggable NN trait + adapter pattern (D8)

**Problem:** The ICP loop needs a nearest-neighbour search, and
the right NN structure depends on the data: brute force is
simple and correct, kiddo is a fast pure-Rust KD-tree, and the
DgmOctree port will eventually need its own cell-code-ordered
descent. The original `icp_iterate` hard-coded brute force, so
variants could not plug in their own NN — they built a tree for
timing and then fell back to cc-rust's brute force, which made
cross-variant bench numbers meaningless (the wall time was
dominated by cc-rust's brute force, not the variant's NN).

**Pattern:** Define a minimal, object-safe trait and a default
adapter. The trait is the contract; the adapter is the
fallback.

```rust
pub trait NearestNeighbour {
    fn nearest(&self, query: &[f32; 3]) -> (usize, f32);
}

pub struct BruteForceNN<'a> { model: &'a [f32] }
impl<'a> NearestNeighbour for BruteForceNN<'a> {
    fn nearest(&self, query: &[f32; 3]) -> (usize, f32) {
        // O(n) scan; identical math to the old
        // `nearest_neighbour_slow` helper.
    }
}

pub fn icp_with_nn<N: NearestNeighbour + ?Sized>(
    data: &mut [f32], model: &[f32], nn: &N, params: &IcprParamsRust,
) -> Result<IcprResultRust, IcprErrorRust> { /* full ICP loop */ }

// Backward-compatible wrapper.
pub fn icp_iterate(data: &mut [f32], model: &[f32], params: &IcprParamsRust)
    -> Result<IcprResultRust, IcprErrorRust>
{
    let nn = BruteForceNN::new(model);
    icp_with_nn(data, model, &nn, params)
}
```

Each variant implements the trait by wrapping its own structure:

- `01-naive-on2` — `pub type NaiveNN<'a> = BruteForceNN<'a>;`
  (zero-cost alias).
- `02-kiddo-kdtree` — `KiddoNN` wraps the kiddo `KdTree` and
  adapts the f32 trait contract to the f64 kiddo internals.
- `03-handrolled-octree` — `OctreeNN` wraps the `Octree` and
  delegates `nearest()` to `Octree::nearest`.

Each variant then calls `icp_with_nn` with its own adapter:

```rust
let nn = build_nn(model_points);
icp_with_nn(data_points, model_points, &nn, params)
```

**Why this works:**

- **Single-method trait is object-safe** (`&dyn NearestNeighbour`
  works, no `where Self: Sized` issue). The ICP loop can be
  generic over `N: NearestNeighbour + ?Sized` and accept both
  concrete types and trait objects.
- **The default `BruteForceNN` adapter is the fallback.** It
  is identical to the old hard-coded scan, so legacy code
  that calls `icp_iterate` keeps working unchanged. The 40
  pre-D8 tests pass without modification.
- **Variants are zero-cost wrappers.** `NaiveNN` is a type
  alias, `KiddoNN` adds one f32→f64 cast per query, `OctreeNN`
  adds one trait dispatch per query. The trait itself is
  monomorphised, so there's no vtable cost in release.

**Symptom of getting it wrong:** if the trait is generic over
a lifetime and you try to make it object-safe, the borrow
checker will reject it. If the trait takes the model slice
by value, every call clones the model. If the adapter does
anything fancy (locks, allocations) inside `nearest()`, the
per-query cost goes from µs to ms. Keep it simple: the trait
is a thin contract, the adapter is a thin wrapper.

**What you do NOT do:** don't make the trait return a borrowed
slice — the model might be local to the calling function. Don't
make the trait take a `&mut self` — ICP queries the model
concurrently across iterations and a `&mut` would prevent
that. Don't use generic associated types for the return —
keep it `(usize, f32)`.

**Source:** `cc-rust/src/registration.rs::NearestNeighbour`,
`cc-rust/src/registration.rs::BruteForceNN`,
`cc-rust/src/registration.rs::icp_with_nn`,
`scenarios/2026-08-19-icp-variants/*/src/lib.rs` (the three
adapter implementations), D8 in
`experimental/docs/decisions.md` (the trait + dispatch
decision), and the `2026-08-20-icp-nn-comparison` scenario
(the cross-variant end-to-end bench).

---


---

## P17. 2026-08-20 — Clone the input when benchmarking N variants side-by-side

**Problem:** ICP mutates the data array in place — the recovered
transform is applied to data_points before returning. When you
run the same ICP algorithm with different NNs (the D8 trait
bench), the second variant sees data that the first variant
has *already* moved to the model. On a 49k-point real-data
test, this manifested as:

`
=== 01-naive-on2 (brute force NN) ===   iterations=13 wall=46s rms=0.000001  tx=(-0.5000) OK
=== 02-kiddo-kdtree (KD-tree NN) ===   iterations=2  wall=0.02s rms=0.000001 tx=(-0.0000) BUG
`

The kiddo ICP reported converged=true after 2 iterations with
RMS=0.000001. The recovered translation was (0, 0, 0). The
numbers *looked* correct (RMS matches naive exactly, only 2
iterations, "converged"). But the translation was wrong.

**Root cause:** between the two icp_iterate calls, the
data slice was shared. The naive ICP had already moved the
data to the model — so the kiddo ICP saw data == model,
RMS was 0 immediately, and SVD returned the identity transform
("data is already at model, no transform needed").

**Pattern:** when benchmarking N variants on the same data
input, snapshot the input before each run. Either:

`
ust
// Option A: clone the template into a fresh buffer per variant.
let data_template = data.clone();
for icp_fn in [icp_naive, icp_kiddo, icp_octree] {
    let mut data = data_template.clone();
    icp_fn(&mut data, &model, &params);
}

// Option B: refactor ICP to return the transform without
// mutating the input (then the bench doesn't need a clone).
pub fn icp_iterate_pure(data: &[f32], model: &[f32], params: &IcprParamsRust)
    -> Result<IcprResultRust, IcprErrorRust>;
// Returns the transform; caller applies it if needed.
`

Option A is the smaller change and is what the
2026-08-20-d8-realdata-all-nns session does. Option B is the
right long-term refactor for a public API (the in-place
mutation is convenient for one-shot ICP but awkward for
benchmarks and undo-able editors).

**Symptom of getting it wrong:** RMS values are identical
across all variants (the variants agree on the correspondences
in the *final* pose, because the data is already at the model
when they run). The recovered transforms are wildly different
across variants. The "converged" flag is 	rue after only 2
iterations on every variant.

**What would have caught it sooner:** an assertion in the test
that the recovered translation has the expected magnitude and
direction. The existing RMS assertion alone is not enough —
RMS can be 0 for both a correct alignment and a "data == model"
trivial alignment.

**Source:** the test-bug fix in
experimental/sessions/2026-08-20-d8-realdata-all-nns/src/main.rs
(2026-08-20). The session's AGENTS.md and decisions.md
also document this gotcha.

---

## P18. 2026-08-20 — Cell-code NN: early-termination check must use `minDistToBorder` (D9)

**Problem:** When implementing a cell-code-ordered NN search
(Chebyshev shell expansion with AABB pruning), the natural early-
termination check is `(d * cell_max_dim)² > best_d2`: if the
minimum distance from the query to any cell in shell d (≈ d
cells × cell_size) exceeds the current best squared distance,
no further cell can contain a closer point. This is the same
form as the per-cell AABB check, just on the whole shell at
once.

But the check is **wrong when the query is near a cell face**.
The per-axis AABB min distance to a d=1 cell is just
`min_dist_to_border` (the distance from the query to the shared
face), not `cell_max_dim`. The naive check uses the cell-size
distance from the cell *center*, which is only correct when
the query IS at the cell center. When the query is anywhere
else, the closest d=1 cell can be much nearer than
`cell_max_dim`, and the naive check incorrectly terminates
before visiting it.

**Concrete failure case (5k Gaussian, level 6, seed 99):**
query at `(-0.28, -0.15, -0.48)`. Cell (26, 27, 25) is the
query's cell; the NN is in cell (27, 27, 25) (Chebyshev
distance 1, just across the +X face). The query is at
distance 0.001 from the +X face, so `min_dist_to_border = 0.001`.
The naive check `(1 * 0.064)² > 0.0024` is `0.0041 > 0.0024`
= true, so the search terminates at d=1 without ever visiting
the d=1 cells. The returned NN is a point in the query's own
cell at d²=0.0024, but the actual NN is at d²=0.0023 in the
adjacent cell.

**Pattern:** the correct lower bound on the AABB min distance
to the most-aligned cell in shell d is:

- **Inside the bbox:** `min_dist_to_border + (d - 1) * cell_max_dim`
  for d ≥ 1. The (d - 1) captures the fact that the d=1 cells
  share a face with the query's cell, so the per-axis distance
  is just `min_dist_to_border` (the query is already partway
  into the cell).
- **Outside the bbox** (after the C++-style jump
  optimisation): `min_dist_to_border + d * cell_max_dim` for
  d > start_d. The first valid shell (d=start_d) is one cell
  further out from the query than the cell *index* distance
  suggests, because the query is on the wrong side of the
  cell border.

The check is then: if `lower_bound² > best_d2`, terminate.

**Why this works:** the per-axis AABB min distance to the
"most-aligned" cell in shell d is the AABB min distance from
the query to the cell's AABB on its dominant axis. Going
through the cells in the dominant direction, the AABB min
distance grows by one cell per shell (after the first
d=1 cell, which shares a face). The formula above captures
this.

**Symptom of the bug:** the D9 test on 5k Gaussian (200
queries) showed D9 returning a slightly non-optimal NN for
query 6 specifically — the NN was in an adjacent cell, but
the search terminated at d=1 because of the bad check. The
brute force on the same fixture returned a different index
with a 0.0001 lower d². The 500-pt Gaussian test didn't
catch it because the larger cell size at level 5 made the
naive check less likely to fail by accident.

**What would have caught it sooner:** a correctness
assertion in the perf test (compare the D9 NN to the brute
force NN on the same queries). The original test only
checked timing, so a slightly-wrong NN went unnoticed.

**What you do NOT do:** don't use `(d * cell_max_dim)² > best_d2`
as the early-termination check. The per-axis distance
accounts for `min_dist_to_border`; the shell-distance check
does not. The C++ algorithm in `DgmOctree.cpp` uses
`ComputeMinDistanceToCellBorder` for exactly this reason.

**Source:** `cc-rust/src/dgm_octree.rs::nearest_neighbor`
(D9, fix landed 2026-08-20). The 5k Gaussian correctness
test in the same file is the regression test; the bug
existed in the previous version of the function.

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
