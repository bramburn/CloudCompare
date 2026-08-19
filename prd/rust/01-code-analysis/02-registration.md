# Code Analysis 02: Registration Tools
**Subsystem:** `libs/qCC_db/extern/CCCoreLib/src/RegistrationTools.cpp`, `PointProjectionTools.cpp`
**Phase 2 Migration Target**
**Date:** 2026-08-19

---

## 1. Three Registration Algorithms

CloudCompare implements three registration algorithms, all in `RegistrationTools.cpp`:

| Algorithm | Class | Use Case | Complexity |
|----------|-------|---------|-----------|
| **Horn** (closed-form) | `HornRegistrationTools` | Global alignment, 3+3 point pairs | MEDIUM |
| **ICP** (iterative) | `ICPRegistrationTools` | Fine registration, noisy data | HIGH |
| **4PCS** (feature-based) | `FPCSRegistrationTools` | Coarse registration, partial overlap | HIGH |

---

## 2. Horn Registration — Code Analysis

### Location: `HornRegistrationTools::FindAbsoluteOrientation`

Implements: B. K. P. Horn, "Closed-form solution of absolute orientation using unit quaternions" (1987).

**Algorithm:**
1. Compute centroids of both point sets
2. Compute cross-covariance matrix H
3. Extract quaternion from largest eigenvector of 4×4 symmetric matrix N
4. Extract scale from centroids and covariance trace

**C++ Implementation concerns:**

```cpp
// RegistrationTools.cpp — FindAbsoluteOrientation
// Lines ~1800–1900 (estimated)
SquareMatrix3x3 R = /* cross-covariance matrix */;
// Jacobi eigenvalue decomposition — manual implementation
for (int iter = 0; iter < max_iter; ++iter) {
    // off-diagonal max → Givens rotation → update matrix
}
// Extract quaternion from eigenvector
```

**Rust implementation using nalgebra:**

```rust
use nalgebra::{Matrix4, SMatrix, Vector3};

pub fn horn_registration(
    to_align: &[[f64; 3]],
    reference: &[[f64; 3]],
) -> Result<Transform3d, &'static str> {
    assert_eq!(to_align.len(), reference.len());

    let n = to_align.len() as f64;

    // Step 1: Centroids
    let c1 = centroid(to_align);
    let c2 = centroid(reference);

    // Step 2: Cross-covariance H = sum((p_i - c1) * (q_i - c2)^T)
    let mut H = SMatrix::<f64, 3, 3>::zeros();
    for (p, q) in to_align.iter().zip(reference.iter()) {
        let dp = [p.0 - c1.x, p.1 - c1.y, p.2 - c1.z];
        let dq = [q.0 - c2.x, q.1 - c2.y, q.2 - c2.z];
        H += SMatrix::<f64, 3, 3>::from_column_slice(&[
            dp[0]*dq[0], dp[0]*dq[1], dp[0]*dq[2],
            dp[1]*dq[0], dp[1]*dq[1], dp[1]*dq[2],
            dp[2]*dq[0], dp[2]*dq[1], dp[2]*dq[2],
        ]);
    }

    // Step 3: Build 4×4 symmetric matrix N (Sab-Rodriguez algorithm)
    let tr_H = H.trace();
    let mut N = Matrix4::new(
        tr_H + tr_H, H[(2,1)] - H[(1,2)], H[(0,2)] - H[(2,0)], H[(1,0)] - H[(0,1)],
        H[(2,1)] - H[(1,2)], tr_H, 0.0, 0.0, 0.0,
        H[(0,2)] - H[(2,0)], 0.0, tr_H, 0.0,
        H[(1,0)] - H[(0,1)], 0.0, 0.0, tr_H,
    );

    // Step 4: Largest eigenvector = optimal quaternion
    // Use nalgebra's eigendecomposition or power iteration
    let (eigenvalues, eigenvectors) = N.symmetric_eigen();
    let q = eigenvectors.column(3);  // largest eigenvalue's eigenvector

    let quaternion = nalgebra::Quaternion::new(q[3], q[0], q[1], q[2]);
    let rotation = nalgebra::UnitQuaternion::from_quaternion(quaternion);
    let scale = compute_scale(to_align, reference, &rotation, &c1, &c2);

    Ok(Transform3d { rotation, scale, translation: c2 - scale * rotation * c1 })
}
```

### Safety guarantee: `UnitQuaternion`

In C++, `Quaternion::normalize()` may return a zero quaternion if the input is near-zero. In Rust with `nalgebra`:

```rust
let rotation = nalgebra::UnitQuaternion::<f64>::from_quaternion(q);
if rotation.is_none() {
    return Err("Degenerate quaternion — points may be coincident");
}
// rotation.unwrap() is guaranteed non-zero
```

---

## 3. ICP Registration — Code Analysis

### Location: `ICPRegistrationTools::Register`

**C++ data flow:**
```cpp
ICPRegistrationTools::RESULT_TYPE ICPRegistrationTools::Register(
    GenericIndexedCloudPersist* inputModelCloud,
    GenericIndexedMesh* inputModelMesh,
    GenericIndexedCloudPersist* inputDataCloud,
    const Parameters& params,
    ScaledTransformation& transform,
    double& finalRMS,
    unsigned& finalPointCount,
    GenericProgressCallback* progressCb) {

    Garbage<GenericIndexedCloudPersist> cloudGarbage;  // ← RAII cleanup
    Garbage<ScalarField> sfGarbage;

    DataCloud data;  // stack-allocated, mutated across iterations
    ModelCloud model;

    while (!converged) {
        // 1. Build or update KD-tree of model
        // 2. For each data point: find nearest neighbour in model
        // 3. Compute optimal transform (Horn sub-step)
        // 4. Apply transform to data cloud (in-place mutation)
        // 5. Compute RMS
        // 6. Check convergence
        // 7. Update progress callback
    }
}
```

**Memory safety issues in C++ implementation:**

1. **`Garbage<>` collector** — manual RAII replacement for exception safety. Catches exceptions during ICP and cleans up temporary clouds. Rust's `Drop` makes this automatic.

2. **In-place mutation of data cloud** — `data.rotatedCloud` is a `PointCloud*` that gets transformed each iteration. If the ICP diverges, the cloud is in an inconsistent state with no rollback.

3. **`GenericProgressCallback*`** — raw pointer passed through parallel regions. If the GUI is closed mid-ICP, the callback becomes a dangling pointer.

**Rust ICP architecture:**

```rust
pub struct IcprEngine {
    model_cloud: Arc<kdtree::KdTree<3, f32, ()>>,  // read-only, shared
    data_points: Vec<[f32; 3]>,                     // exclusive write access
    params: IcprParams,
    state: IcprState,
}

enum IcprState {
    Idle,
    Running { iteration: u32, last_rms: f64 },
    Converged { final_rms: f64, transform: Transform3d },
    Failed(IcprError),
}

impl IcprEngine {
    pub fn step(&mut self) -> IcprStepResult {
        // Stateful ICP iteration — compiler enforces linear progression
        // Cannot skip to Converged without going through Running
        // Transform history tracked automatically (no rollback needed)
        match self.state {
            IcprState::Idle => { self.state = IcprState::Running { iteration: 0, last_rms: f64::MAX }; }
            IcprState::Running { iteration, last_rms } => {
                // iteration logic
                if converged {
                    self.state = IcprState::Converged { final_rms, transform };
                } else {
                    self.state = IcprState::Running { iteration: iteration+1, last_rms };
                }
            }
            _ => {}
        }
    }
}
```

---

## 4. Data Race in ICP Parameters

**Location:** `ICPRegistrationTools::Parameters` and usage in TBB loops

```cpp
// Parameters contains:
struct Parameters {
    // ...
    ScalarField* modelWeights;   // ← WRITTEN by ICP loop
    ScalarField* dataWeights;    // ← WRITTEN by ICP loop
    int maxThreadCount;           // ← READ by parallel_for
    NORMALS_MATCHING normalsMatching;
    CONVERGENCE_TYPE convType;
};
```

**In ICP loop:**
```cpp
// modelWeights->at(i) is written by ICP iteration
// Then the ICP loop may spawn TBB parallel_for
// modelWeights pointer is passed to the parallel region
// If the weights are also being read by another thread → DATA RACE
```

**Rust fix:** `IcprParams` contains `Option<Arc<RwLock<ScalarField>>>` or `Arc<Vec<f32>>`. Shared read access is explicit; exclusive write access is enforced by the type system.

---

## 5. 4PCS Registration — Code Analysis

**Location:** `FPCSRegistrationTools::RegisterClouds`

**Algorithm:** Select 4 coplanar points from data cloud → find congruent 4-point bases in model cloud → compute rigid transform → score.

**Memory issues:**
```cpp
// FPCSRegistrationTools.cpp:
std::vector<Base> candidates;  // potentially millions of candidates
std::vector<ScaledTransformation> transforms;  // one per candidate
// Memory grows with O(n^2) where n = number of candidate bases
// No pre-allocation — repeated push_back causes reallocation
```

**Rust fix:**
```rust
struct FpcsContext {
    candidates: Vec<Base>,
    transforms: Vec<Transform3d>,
    // Pre-allocated capacity hints based on overlap estimate
    max_candidates: usize,
}

impl FpcsContext {
    pub fn with_capacity(estimated_candidates: usize) -> Self {
        FpcsContext {
            candidates: Vec::with_capacity(estimated_candidates),
            transforms: Vec::with_capacity(estimated_candidates),
            max_candidates: estimated_candidates,
        }
    }
}
```

---

## 6. PointProjectionTools — Transformation Math

**Location:** `PointProjectionTools.cpp`

**What it does:** 3D transformation matrices, SVD-based registration sub-step, Procrustes analysis.

**Rust equivalent:** Pure nalgebra — no FFI needed for the math itself.

```rust
use nalgebra::{Isometry3, Matrix3, Vector3, Matrix4};

pub fn apply_transform(points: &[[f32; 3]], t: &Isometry3<f64>) -> Vec<[f32; 3]> {
    points.iter()
        .map(|p| {
            let transformed = t.transform_point(&nalgebra::Point3::new(p[0] as f64, p[1] as f64, p[2] as f64));
            [transformed.x as f32, transformed.y as f32, transformed.z as f32]
        })
        .collect()
}
```

**This file is entirely self-contained math — best candidate for direct Rust rewrite without FFI at all.**
