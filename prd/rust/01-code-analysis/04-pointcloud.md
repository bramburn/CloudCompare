# Code Analysis 04: ccPointCloud — Deferred Migration
**Subsystem:** `libs/qCC_db/src/ccPointCloud.cpp/.h`
**Decision: Do Not Migrate (Deferred Indefinitely)**
**Date:** 2026-08-19

---

## 1. Why Not to Migrate ccPointCloud

`ccPointCloud` is the fundamental data structure of CloudCompare — every plugin, every algorithm, every GUI component touches it. Attempting to migrate it to Rust would require:

1. **Rewriting every C++ algorithm** that calls `ccPointCloud` methods
2. **CXX bridge for every ccPointCloud method** — the class has ~150 public methods
3. **Qt6 ABI coupling** — `ccPointCloud` inherits from `ccHObject` which is deeply coupled to Qt's `QObject` meta-object system
4. **OpenGL buffer management** — `QOpenGLBuffer` references live inside `ccPointCloud`

**The FFI surface is too large and too deeply coupled to migrate.** This is a "never migrate" decision, not a "defer" decision.

---

## 2. What ccPointCloud Does

```cpp
class ccPointCloud : public GenericPointCloud, public ccHObject {
    // Geometric data
    std::vector<CCVector3> m_points;           // owned: 3D positions

    // Per-point features
    std::vector<RGB颜料> m_colors;              // owned (or shared)
    std::vector<CompressedNormals> m_normals;  // owned
    ScalarFieldSet m_scalarFields;             // owned

    // Spatial index (lazy-built)
    ccOctree* m_octree;                        // raw pointer, lazily built
    ccPointCloudLOD* m_lod;                    // owned, for display

    // Visibility
    std::vector<unsigned> m_visibFlags;        // owned

    // Child objects (meshes, images, etc.)
    std::vector<ccMesh*> m_meshes;             // SHOULD be unique_ptr
};
```

---

## 3. Specific Issues (For the Record)

### Issue 1: Octree Ownership — Classic Rust Win

```cpp
// ccPointCloud.cpp — octree is built lazily and deleted manually:
ccOctree* cloud = entity->getOctree();
// ...
// If cloud is modified: entity->deleteOctree();  ← manual call
// If entity is deleted: ~ccHObject() → delete m_octree  ← manual in destructor
```

If the user calls `cloud->resize(n)` after building an octree, the octree is invalidated. Accessing it after this point is a use-after-free.

**Rust approach:**
```rust
pub struct PointCloud {
    points: Vec<[f32; 3]>,
    octree: Mutex<Option<Octree>>,  // Invalidated when points change
}
```

The `Mutex` makes the `Option<Octree>` explicit: `None` means "needs rebuild." The type system enforces this — you cannot access the octree without going through the mutex.

### Issue 2: Child Object Ownership

```cpp
std::vector<ccMesh*> m_meshes;  // SHOULD be unique_ptr
// Currently:
m_meshes.push_back(newMesh);
delete m_meshes[i];  // manual — error-prone
```

**Rust approach:**
```rust
pub struct PointCloud {
    meshes: Vec<Mesh>,
}
// No pointer — Mesh values own their data
// Or with Arc for shared ownership:
meshes: Vec<Arc<Mesh>>
```

### Issue 3: Qt Coupling

`ccPointCloud` inherits from `ccHObject` which inherits from `QObject`. The Qt meta-object system (MOC) generates code that expects C++ vtables. This cannot be represented in Rust's type system.

**Decision: Keep all Qt-bound code in C++. This is not a Rust limitation — it's a fundamental architectural constraint.**

---

## 4. What CAN be Migrated Around ccPointCloud

While `ccPointCloud` itself cannot migrate, the **algorithms that work on point clouds** can:

| Algorithm | File | Can Migrate | Why |
|-----------|------|:-----------:|------|
| Octree build | `DgmOctree.cpp` | ✅ YES | Pure compute, no Qt |
| KD-tree build/query | `KdTree.cpp` | ✅ YES | Pure compute, no Qt |
| Scalar field stats | `ScalarField.cpp` | ✅ YES | No Qt |
| ICP registration | `RegistrationTools.cpp` | ✅ YES | No Qt |
| Normals computation | `ccPointCloudInterpolator.cpp` | ✅ YES | Pure compute |
| Distance computation | `DistanceComputationTools.cpp` | ✅ YES | Pure compute |
| Cloud sampling | `CloudSamplingTools.cpp` | ✅ YES | Pure compute |
| OpenGL rendering | `ccPointCloud.cpp` (display) | ❌ NO | Qt+OpenGL |
| Chunking (>2B pts) | `ccPointCloud.cpp` | ❌ NO | Qt-bound |
| File I/O | `ccPointCloud.cpp` (load/save) | ✅ YES (I/O layer only) | |

**The strategy is: migrate what ccPointCloud DOES, not what ccPointCloud IS.**

---

## 5. CXX Bridge for ccPointCloud Interactions

Even though `ccPointCloud` stays in C++, Rust algorithms can work on it via CXX:

```cpp
// In CXX bridge:
unsafe extern "C++" {
    type CxxPointCloud;
    fn CxxPointCloud_getPoint(self: &CxxPointCloud, i: usize) -> &[f32; 3];
    fn CxxPointCloud_size(self: &CxxPointCloud) -> usize;
    fn CxxPointCloud_getScalarField(self: &CxxPointCloud, i: usize) -> *mut CxxScalarField;
    fn CxxPointCloud_addPoint(self: &mut CxxPointCloud, p: &[f32; 3]) -> bool;
}
```

```rust
// Rust algorithms work on the CXX wrapper:
fn compute_cloud_mean(cloud: &CxxPointCloud) -> f64 {
    let n = cloud.size();
    let mut sum = 0.0f64;
    for i in 0..n {
        let p = cloud.getPoint(i);
        sum += (p[0] + p[1] + p[2]) as f64;
    }
    sum / (n as f64 * 3.0)
}
```

**The boundary is clean: Rust sees point data, C++ owns the ccPointCloud.**
