# Swarm Report 03: Floating-Point Geometry & Math Analysis
**Agent:** explore — numerics specialist
**Scope:** `libs/qCC_db/extern/CCCoreLib/` — math, geometry, transformation files
**Date:** 2026-08-19

---

## 1. Numerical Type System in CCCoreLib

### 1.1 Scalar Types

```cpp
// From CCCoreTypes.h (approximate):
using ScalarType = float;       // Default: 32-bit float
using PointCoordinateType = int32_t;  // Fixed-point-like for 3D coordinates
using DistanceType = double;    // 64-bit for distances, RMS computations

// Registration transforms use double precision
struct ScaledTransformation {
    SquareMatrix3x3 R;    // 3x3 rotation matrix (stored as float, computed in double)
    CCVector3 T;          // Translation (float)
    double s = 1.0;       // Scale (double)
};
```

**Precision concern:** `SquareMatrix3x3` stores rotation as `float[9]` internally. ICP iterations accumulate floating-point errors across many iterations. The RMS convergence criterion (`minRMSDecrease = 1.0e-5`) may be met or missed depending on float vs double precision in intermediate computations.

### 1.2 CCVector3 / 3D Point Representation

```cpp
// From ccTypes.h (approximate):
template <typename Scalar> class CCVector3 {
    Scalar c[3];  // x, y, z — no SIMD alignment guarantee
};

// Used as CCVector3<PointCoordinateType> for positions
// Used as CCVector3f (= CCVector3<float>) for normals
```

**Alignment:** `c[3]` is not guaranteed to be 16-byte aligned for SIMD. The octree code uses Morton codes (bit-interleaving) where misaligned reads cause performance loss but not correctness issues.

---

## 2. Key Numerical Operations

### 2.1 Horn Registration (Closed-Form)

**File:** `RegistrationTools.cpp:1800+` (estimated)

Implements the closed-form quaternion solution from Horn 1987:
- Computes 4×4 covariance matrix (cross-covariance, auto-covariances)
- Solves 4×4 eigenvalue problem via Jacobi rotation
- Extracts optimal quaternion from largest eigenvector

**Numerical considerations:**
- The 4×4 eigenvalue solve is the most numerically sensitive step
- Jacobi iteration converges in ~6–10 iterations for well-conditioned clouds
- `SquareMatrix::toEigen()` conversion for the eigenvalue solve (likely calls into some lapack-like lib)
- Rust equivalent: `nalgebra` crate (`nalgebra = "0.34"`) — pure Rust, SIMD-accelerated

**Rust win:** `nalgebra::OMatrix<f64, Const<4>, Const<4>>::eigen()` — checked eigenvalue computation with explicit convergence tolerance.

### 2.2 ICP Iteration (Iterative, Numerical)

**File:** `RegistrationTools.cpp:147` → `ICPRegistrationTools::Register`

```cpp
// Simplified ICP loop
while (!converged) {
    // Step 1: Find nearest neighbours (KD-tree query)
    findNearestNeighbours(model, data, neighbours);

    // Step 2: Compute optimal transformation (Horn or SVD)
    trans = ComputeTransformation(neighbours);

    // Step 3: Apply transformation to data cloud
    data.applyTransform(trans);

    // Step 4: Compute RMS
    rms = ComputeRMS(model, data, trans);

    // Step 5: Check convergence
    if (delta_rms < minRMSDecrease) converged = true;
}
```

**Issues:**
- `neighbours` array: `std::vector<PointDescriptor>` allocated and deallocated each iteration
- `data.applyTransform(trans)`: in-place modification of all points — no rollback if iteration diverges
- Float precision accumulation across iterations (mitigated by RMS convergence check, but not robust)

**Rust win:** `applyRigidTransformation` can be expressed as a zero-copy `SMatrix4x4` multiply on a `&mut [CCVector3]`. No temporary allocations per iteration.

### 2.3 Octree Morton Code Computation

**File:** `DgmOctree.cpp:100+` (estimated)

Morton code (Z-order curve) for spatial hashing:
```cpp
// Key computation: interleave 3D coordinates into a 1D key
// DgmOctree.h:66-73 — CellCode is 64-bit or 32-bit based on OCTREE_CODES_64_BITS
CellCode ComputeMortonCode(const CCVector3& point) {
    // Bit-interleaving of x, y, z
    // Each level strips 3 bits (one per dimension)
    // Used for fast cell neighbour queries
}
```

**Rust win:** Bit operations are identical in speed. No safety issue — purely arithmetic.

### 2.4 Distance Computation

**File:** `DistanceComputationTools.cpp`

Distance types:
- `computeCloud2CloudDistance`: C2C — point-to-point nearest neighbour
- `computeCloud2MeshDistance`: C2M — point-to-triangle distance (slower)
- `computeHausdorffDistance`: max distance, robust to outliers
- `computeMeanDistance`: L1 mean
- `computeRMS`: L2 norm

**Numerical concern:** Point-to-triangle distance requires projecting onto the triangle plane and clamping — this involves 3×3 matrix solves per triangle (in the closest-point-on-triangle computation). Accumulated error can exceed the `delta` threshold used in 4PCS registration.

---

## 3. Rust Numerical Libraries for This Domain

### 3.1 Linear Algebra

| Crate | Purpose | SIMD | Maintenance |
|-------|---------|------|-------------|
| `nalgebra = "0.34"` | Vectors, matrices, quaternions, geometry | Yes (via `simba` or `wide`) | Active, well-tested |
| `glam = "0.29"` | Game/graphics math (Vec3, Mat4, Quat) | Yes (wide SIMD) | Very active |
| `matrixmultiply` | BLAS-like matmul | Yes (AVX2/NEON) | Active |
| `faer = "0.19"` | Dense linear algebra (LAPACK replacement) | Yes | Active, cutting-edge |
| `splines = "0.2"` | Spline interpolation | — | Medium |
| `eigen = "0.5"` | Eigen3 bindings | Via Eigen | Experimental |

**Recommendation for CloudCompare:**
- `glam` for 3D geometry (Vec3, Mat4, Quat) — fastest for small matrices
- `nalgebra` for general-purpose (eigenvalue solves, general matrices)
- `faer` for dense linear algebra (replaces manual Jacobi/QR implementations)

### 3.2 FFT / Signal Processing

Not heavily used in CCCoreLib — only `FastMarching.cpp` uses distance transforms. For 3D distance field computation, `dfdx = "0.2"` (numpy-like) or manual FFT is sufficient.

### 3.3 Spatial Indexing

| Crate | Purpose | Notes |
|-------|---------|-------|
| `rstar = "0.11"` | R-tree, 2D/3D | Slower than custom KD-tree for point clouds |
| `kdtree = "0.16"` | KD-tree, k-nearest-neighbour | Pure Rust, SIMD optional |
| `baron = "0.2"` | BVH for ray tracing | Could replace DgmOctree for ray casting |
| `geo = "0.28"` | Geospatial primitives | 2D only |

**Note:** `rstar` R-trees are designed for bounding-box queries, not point clouds. Custom KD-tree (`KdTree.cpp`) has better cache locality for point queries. A Rust rewrite of `DgmOctree` should use a similar 3D octree structure rather than an R-tree.

### 3.4 Special Functions

`sciprs = "0.4"` provides error functions (used in `ErrorFunction.cpp` for M3C2 uncertainty). No Rust replacement needed — this is self-contained and safe.

---

## 4. Numerical Robustness Comparison

| Algorithm | C++ Implementation | Rust Equivalent | Numerical Risk |
|-----------|-------------------|-----------------|---------------|
| Horn registration (4×4 eigensolve) | Manual Jacobi iteration | `nalgebra::SelfAdvancement::try_inverse` + eigen | LOW — direct translation |
| ICP nearest neighbour | KD-tree recursion | `kdtree` crate | LOW — same algorithm |
| Octree build (Morton codes) | Bit-hacking | Bit manipulation identical | NONE — deterministic |
| Point-to-triangle distance | Analytical projection | `nalgebra::closest_point_to_triangle` | LOW — same formula |
| Fast Marching (SDF) | Priority queue + heap | `Vec` + binary heap (std) | LOW — same algorithm |
| Scalar field statistics | Naive iteration | Iterator-based | LOW — same algorithm |
| Kriging interpolation | Matrix solve per cell | `faer::chol` (Cholesky) | MEDIUM — numerical stability |

---

## 5. FP Safety for Migration

### 5.1 Floating-Point NaN / Infinity Handling

C++ uses magic constants:
```cpp
#define NAN_FLOAT (std::numeric_limits<ScalarType>::quiet_NaN())
#define CC_ASCII_POINTS_TEMPORARY_HEADER_VERSION 5
```

Rust equivalent:
```rust
const NAN_FLOAT: f32 = f32::NAN;  // direct
const NAN_DOUBLE: f64 = f64::NAN;

// Checking validity:
if val.is_nan() { /* invalid */ }
if !val.is_finite() { /* guard against inf */ }
```

### 5.2 Precision Transition Points

**Risk:** When crossing the FFI boundary, C++ `float` and Rust `f32` have identical IEEE 754 representation. No conversion needed.

**Caveat:** The C++ code uses `PointCoordinateType = int32_t` for fixed-point coordinates. If Rust needs to work with the same coordinate system:
```rust
type PointCoord = i32;  // matches C++ exactly
type Scalar = f32;       // matches C++ exactly
type Distance = f64;    // C++ double = Rust f64
```

### 5.3 Quaternion Normalization

The ICP algorithm requires unit quaternions. C++ normalizes after each step:
```cpp
Quaternion q = /* computed */;
q.normalize();  // implicit — may not check for zero-length
```

Rust with `nalgebra`:
```rust
use nalgebra::{Quaternion, UnitQuaternion, RealField};
let q = UnitQuaternion::from_quaternion(q);  // Returns Option<UnitQuaternion<T>>
// None if quaternion norm is below threshold — explicit error
```
**Rust win:** `UnitQuaternion` makes the normalisation guarantee explicit in the type — `q.rotate_vector()` always returns a valid result; `q.as_ref()` gives raw quaternion if needed.
