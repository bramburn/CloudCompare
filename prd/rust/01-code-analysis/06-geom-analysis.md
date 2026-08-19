# Code Analysis 06: GeometricalAnalysisTools & DistanceComputationTools
**Subsystem:** `libs/qCC_db/extern/CCCoreLib/src/GeometricalAnalysisTools.cpp`, `DistanceComputationTools.cpp`
**Phase 3–4 Migration Target**
**Date:** 2026-08-19

---

## 1. GeometricalAnalysisTools — Code Analysis

### Location: `libs/qCC_db/extern/CCCoreLib/src/GeometricalAnalysisTools.cpp`

**What it does:**
- **Curvature estimation** — principal curvatures via quadric fitting
- **Local roughness** — neighbourhood variance
- **Geometric saliency** — multi-scale curvature analysis (for feature extraction)
- **Density estimation** — neighbourhood point count

**Key data flow:**
```cpp
// GeometricalAnalysisTools.cpp — structure passed to TBB parallel_for
struct StructureOprationSet {
    GenericOctree* theOctree;
    GenericCloud* theCloud;
    unsigned radius;  // neighbourhood radius in grid units
    CCCoreLib::DgmOctree::CELL_TYPE cellType;
    // ...
};

void ComputeGeometricSaliency(...) {
    // Creates vector<StructureOprationSet> — one per scale
    // Passes void** to TBB — type-unsafe across threads
}
```

### Safety Issues

1. **`void**` parameter passing in parallel regions** — same issue as `DgmOctree::MultiThreadingWrapper`:
   - C++ casts `void**` to `StructureOprationSet*` inside the parallel loop
   - If the cast is wrong, type confusion occurs
   - No compile-time check

2. **Memory-mapped grid operations** — allocates large 2D arrays for per-cell results:
   ```cpp
   float* saliencyGrid = static_cast<float*>(malloc(w * h * sizeof(float)));
   // No guard: w * h could overflow
   // No zero-initialisation: uninitialized values if malloc succeeds partially
   ```

**Rust fix:**

```rust
use std::collections::HashMap;

pub struct GeometricAnalysisContext {
    octree: Arc<DgmOctree>,
    cloud: Arc<kdtree::KdTree<3, f32, ()>>,
    radius: u32,
    cell_type: CellType,
}

pub fn compute_curvature_saliency(
    ctx: &GeometricAnalysisContext,
    scales: &[u32],
) -> HashMap<u32, Vec<f32>> {  // scale → per-cell saliency
    let mut results = HashMap::new();

    for &scale in scales {
        let mut saliency = Vec::with_capacity(ctx.octree.cell_count(scale));

        // Parallel over cells
        let cells: Vec<_> = ctx.octree.cells_at_level(scale).collect();
        let saliency_chunk: Vec<_> = cells.par_iter()
            .map(|cell| compute_cell_curvature(cell, scale, &ctx.params))
            .collect();
        saliency.extend(saliency_chunk);

        results.insert(scale, saliency);
    }

    results
}
```

---

## 2. DistanceComputationTools — Code Analysis

### Location: `libs/qCC_db/extern/CCCoreLib/src/DistanceComputationTools.cpp`

**What it does:**
- `computeCloud2CloudDistance` — point-to-point
- `computeCloud2MeshDistance` — point-to-triangle (slower, more accurate)
- `computeHausdorffDistance` — maximum distance
- `computeMeanDistance` — L1 mean
- `computeRMS` — L2 norm
- `computeApproxCloud2MeshDistance` — fast approximation using octree cells

**Concurrency pattern:**

```cpp
// DistanceComputationTools.cpp:930-944 — mutex per-thread (unusual)
struct LocalContext {
    QMutex currentBitMaskMutex;   // Qt path
    tbb::mutex currentBitMaskMutex;  // TBB path
};

// In parallel loop:
LocalContext ctx;  // stack-allocated per thread — each thread has its own mutex
```

This is a code smell — the mutex suggests the code is doing something that *should* be thread-safe but the author wasn't confident. The mutex-per-thread pattern is essentially a no-op for thread safety.

**Rust equivalent — no mutex needed:**

```rust
// Rust: no shared mutation in the parallel region
// Each thread operates on its own slice of the output
let distance_results: Vec<f64> = (0..n_points)
    .into_par_iter()
    .map(|i| {
        let nearest = kdtree.nearest(&points[i]).unwrap();
        let dist = euclidean_distance(&points[i], nearest.0);
        dist  // Each thread writes to its own output index — no mutex needed
    })
    .collect();
```

---

## 3. Scalar Field Arithmetic Tools

### Location: `libs/qCC_db/extern/CCCoreLib/src/ScalarFieldTools.cpp`

**What it does:**
- Arithmetic on scalar fields (`+`, `-`, `*`, `/` scalar)
- Scalar field combination (pointwise `sf1 * sf2`)
- Thresholding, filtering, resampling

**C++ safety issues:**

```cpp
// ScalarFieldTools.cpp — arithmetic without bounds check
void ScalarFieldTools::MultiplyScalarField(ScalarField* sf1, ScalarField* sf2, ScalarField* out) {
    // ASSUMES: sf1->size() == sf2->size() == out->size()
    // If not: out-of-bounds write → heap corruption
    for (size_t i = 0; i < sf1->size(); ++i) {
        out->setValue(i, sf1->at(i) * sf2->at(i));  // no size check
    }
}
```

**Rust fix:**
```rust
impl ScalarField {
    pub fn pointwise_mul(&self, other: &ScalarField) -> Result<ScalarField, SFError> {
        if self.values.len() != other.values.len() {
            return Err(SFError::SizeMismatch {
                size_a: self.values.len(),
                size_b: other.values.len(),
            });
        }
        let result: Vec<f32> = self.values.iter()
            .zip(other.values.iter())
            .map(|(a, b)| a * b)
            .collect();
        Ok(ScalarField { values: result, ..Default::default() })
    }
}
```

**The Rust compiler enforces size equality at runtime — and with `assert!` in tests, it's enforced at development time too.**
