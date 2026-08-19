# Swarm Report 01: Memory Management Analysis
**Agent:** explore — memory-safety specialist
**Scope:** `libs/qCC_db/extern/CCCoreLib/src/` — all `.cpp` files
**Date:** 2026-08-19

---

## 1. Findings Summary

Of the 34 `.cpp` source files in CCCoreLib, **8 are high-severity memory management candidates** for Rust migration. The remainder are either purely mathematical (low risk) or so tightly coupled to the Qt/GUI layer that FFI complexity outweighs the benefit.

---

## 2. Priority 1 — Manual Allocation / Ownership Issues

### 2.1 `ccPointCloud.cpp` — Primary Cloud Data Structure
**Risk:** CRITICAL
**Lines:** ~1,800 (`.cpp`) + 1,066 (`.h`)
**File:** `libs/qCC_db/src/ccPointCloud.cpp`

**Key memory patterns:**
```cpp
// Pattern 1: Raw pointer ownership scattered across methods
class ccPointCloud {
    std::vector<CCVector3> m_points;         // owned: ok, std::vector handles it
    std::vector<unsigned> m_visibFlags;     // owned: ok
    ScalarField* m_currentScalarField;       // RAW POINTER — ownership unclear
    ccOctree* m_octree;                      // RAW POINTER — owned? borrowed?
    ccPointCloudLOD* m_lod;                   // RAW POINTER — owned?
    std::vector<ccMesh*> m_meshes;            // owned: ok, std::vector<unique_ptr> ideally
};
```

**Issues identified:**
- `m_octree`: deleted in destructor but built lazily — potential use-after-free if accessed after cloud modification
- `m_currentScalarField`: raw pointer with no clear ownership contract; cast from `std::vector<ScalarField>*` without lifetime annotation
- `m_meshes`: `std::vector<ccMesh*>` — classic ownership ambiguity; should be `std::vector<std::unique_ptr<ccMesh>>`
- No `std::unique_ptr` / `std::shared_ptr` for cloud-child relationships
- Chunking logic for >2B points uses manual offset arithmetic (`m_chunkOffset`) — error-prone

**Rust win:** Replace all raw pointers with `Option<Box<T>>` / `Arc<RwLock<T>>` for shared cloud access. The octree-lifetime issue is a textbook Rust borrow checker win.

### 2.2 `DgmOctree.cpp` — Spatial Index
**Risk:** HIGH
**Lines:** 4,054 (`.cpp`) + 1,258 (`.h`)
**File:** `libs/qCC_db/extern/CCCoreLib/src/DgmOctree.cpp`

**Key memory patterns:**
```cpp
// Pattern 1: Dangling pointer risk in PointDescriptor
struct PointDescriptor {
    const CCVector3* point;    // BORROWED from cloud — lifetime bound to cloud
    unsigned pointIndex;
    double squareDistd;
};

// Pattern 2: Cell code storage — manual memory management via std::vector
std::vector<CellCode> m_codes;     // owned by octree
std::vector<NeighboursSet> m_neighbors;  // dynamic, per-cell, allocated on demand

// Pattern 3: m_buildInProgress atomic — but no mutex on the actual build state
std::atomic<bool> m_buildInProgress;
```

**Issues identified:**
- `NeighboursSet` (`std::vector<PointDescriptor>`) is allocated per-octree-cell on demand. Memory fragmentation risk with millions of points.
- The `m_MT_wrapper` closure captures `this` — potential data race if octree is modified mid-build
- `cellCodesContainer` / `cellIndexesContainer` resize operations can throw `std::bad_alloc` (caught, but leaks partial state)

**Rust win:** `Vec<PointDescriptor>` with `SmallVec<[PointDescriptor; 8]>` for the common case. `Arc<DgmOctree>` for thread-safe sharing. Build state machine encoded in an enum (Idle | Building | Ready | Error) with exclusive access.

### 2.3 `ScalarField.cpp` — Per-Point Data
**Risk:** HIGH
**File:** `libs/qCC_db/extern/CCCoreLib/src/ScalarField.cpp`

**Key memory patterns:**
```cpp
// Pattern: inherits from std::vector<float>
class ScalarField : public std::vector<ScalarType> {
    std::string m_name;
    double m_offset;
    bool m_offsetHasBeenSet;
    double m_localMinVal, m_localMaxVal;
};
```

**Issues identified:**
- Inherits from `std::vector` — slicing risk if copied through base pointer
- `m_offset` arithmetic (`+=`, `-=`) on large fields can overflow (int32 on 32-bit, no guard)
- `resize()` without `reserve()` in batch operations causes repeated reallocations
- `countValidValues()` iterates full vector even when early-exit is possible
- No `NaN` sentinel for invalid values (uses `NAN_FLOAT` magic constant)

**Rust win:** `Vec<f32>` with a separate `ValidityBitmap` (small `Vec<bool>` or `u8` bitmap). `Offset<f32>` newtype with checked arithmetic. `HashMap<String, Box<ScalarField>>` for named fields.

---

## 3. Priority 2 — Ownership Ambiguity

### 3.1 `ReferenceCloud.cpp` — Cloud View / Subset
**Risk:** MEDIUM-HIGH
**File:** `libs/qCC_db/extern/CCCoreLib/src/ReferenceCloud.cpp`

**Key patterns:**
```cpp
// Pattern: points to another cloud's data (view semantics)
class ReferenceCloud : public GenericIndexedCloud {
    PointCloud* m_theCloud;    // NOT owned — view into m_theCloud's points
    std::vector<unsigned> m_pointIndexes;  // indexes into m_theCloud
};
```

**Issues identified:**
- `m_theCloud` is a raw pointer with no lifetime annotation. If the parent cloud is deleted while a `ReferenceCloud` exists, dangling pointer.
- Used extensively in ICP registration (`DataCloud::cloud`, `ModelCloud::cloud`) — see `RegistrationTools.cpp:128–144`
- No `std::weak_ptr` equivalent

**Rust win:** `Weak<ccPointCloud>` / `'a` lifetime on the cloud reference. Rust's lifetime system would prevent storing a `ReferenceCloud` after the parent cloud is dropped.

### 3.2 `KdTree.cpp` / `TrueKdTree.cpp` — Nearest Neighbour Search
**Risk:** MEDIUM
**Files:** `libs/qCC_db/extern/CCCoreLib/src/KdTree.cpp`, `TrueKdTree.cpp`

**Key patterns:**
```cpp
class KDTree {
    KDTreeCell* m_root;    // RAW POINTER — manually allocated/freed
    GenericIndexedCloud* m_associatedCloud;  // borrowed reference
    std::vector<unsigned> m_indexes;
};

// In buildSubTree: recursive allocation with no RAII
KDTreeCell* buildSubTree(...) {
    auto* cell = new KDTreeCell();  // manual new
    // ...
    if (failed) { deleteSubTree(cell); return nullptr; }  // exception-unsafe
    return cell;
}
```

**Issues identified:**
- `deleteSubTree` is the only cleanup mechanism — recursive deletion with no tail-call optimization
- `new`/`delete` pair not exception-safe (if `std::bad_alloc` throws during subtree build, partial tree leaks)
- `m_indexes` vector resize can invalidate the `m_root` pointer if a realloc happens (C++11 guarantees iterator stability for `vector::push_back` but raw pointers are not iterators)

**Rust win:** Recursive tree with `Box<KdTreeCell>` — drop is automatic and tail-call optimizable. `Rc<RefCell<KdTreeCell>>` for shared trees.

---

## 4. Priority 3 — Buffer / Array Safety

### 4.1 `GeometricalAnalysisTools.cpp`
**Risk:** MEDIUM
**File:** `libs/qCC_db/extern/CCCoreLib/src/GeometricalAnalysisTools.cpp`

**Key patterns:**
```cpp
// Pattern: C-style arrays passed to TBB parallel_for
void ComputeGeometricSaliency(..., void** userParams) {
    // userParams is void** — no type safety on parameter access
    // cast to StructureOprationSet* without validation
    auto* sos = static_cast<StructureOprationSet*>(userParams[0]);
}

// Pattern: raw float* buffers shared across threads
float* outputBuffer = static_cast<float*>(malloc(n * sizeof(float))); // no guard
```

### 4.2 `FastMarching.cpp` — Propagation Algorithms
**Risk:** MEDIUM
**File:** `libs/qCC_db/extern/CCCoreLib/src/FastMarching.cpp`

**Key patterns:**
```cpp
// Pattern: priority queue with manual reallocation
// m_distance is a std::vector<double> that grows during propagation
// if maxheap overflows, partial state is inconsistent
```

---

## 5. Migration Guidance

### 5.1 Rule of Thumb: When to Migrate

| Condition | Action |
|-----------|--------|
| Raw `new`/`delete` pair | **Migrate immediately** — textbook Rust win |
| `std::vector<T*>` (owned pointers) | **Migrate** — convert to `Vec<Box<T>>` |
| `std::vector<T*>` (borrowed indices) | **Keep in C++** — Rust would need lifetime `'a` |
| Qt6 GUI / OpenGL bound | **Never migrate** — Qt ABI incompatibility |
| Pure math, no allocation | **Keep in C++** — no Rust benefit |
| TBB / QtConcurrent parallel | **Audit FFI before migrating** |

### 5.2 Memory Migration Pattern Map

| C++ Pattern | Rust Equivalent | FFI Notes |
|------------|-----------------|-----------|
| `T* ptr = new T()` | `Box::new(T)` | `Box<T>` crosses FFI as `unique_ptr<T>` |
| `delete ptr` | `drop(Box::into_raw(...))` | Only for C++→Rust ownership transfer |
| `std::vector<T>` | `Vec<T>` | Zero-copy via `&mut [T]` slice |
| `T**` (output param) | `&mut Option<Box<T>>` | Explicit nullable |
| `std::atomic<T>` | `AtomicU32`, etc. | Direct equivalent |
| `std::unique_ptr<T>` | `Box<T>` | `cxx` converts automatically |
| `std::shared_ptr<T>` | `Rc<RefCell<T>>` or `Arc<RwLock<T>>` | Choose based on single/multi-thread |
| `std::vector<T*>` (owned) | `Vec<Box<T>>` | Unwrap/rewrap at FFI boundary |
| `T&` (reference) | `&T` | Must not outlive source |
| `void*` | `*mut std::ffi::c_void` | Requires unsafe block in Rust |
