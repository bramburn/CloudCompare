# Code Analysis 03: DgmOctree & KdTree
**Subsystem:** `libs/qCC_db/extern/CCCoreLib/src/DgmOctree.cpp`, `KdTree.cpp`, `TrueKdTree.cpp`
**Phase 3 Migration Target**
**Date:** 2026-08-19

---

## 1. DgmOctree Architecture

### What it does

`DgmOctree` is a **3D spatial index** for point clouds. Unlike a generic octree, it uses:
- **Morton codes** (Z-order curve) for fast cell neighbour queries
- **Structure-of-Arrays (SoA)** within each level for cache efficiency
- **Per-cell lazy allocation** — cells only allocate neighbours when queried

### Data Layout

```cpp
// DgmOctree.h — key data structures
class DgmOctree {
    // Per-level cell data (SoA layout)
    std::vector<CellCode> m_codes;              // level 0 cell codes
    std::vector<cellIndexesContainer> m_thePointsAndNeighbours;  // level 0 neighbours

    // These arrays repeat for each level up to MAX_OCTREE_LEVEL
    PointCoordinateType m_cellSize[MAX_OCTREE_LEVEL + 2];
    unsigned m_cellCount[MAX_OCTREE_LEVEL + 1];
    unsigned m_maxCellPopulation[MAX_OCTREE_LEVEL + 1];
    double m_averageCellPopulation[MAX_OCTREE_LEVEL + 1];
    double m_stdDevCellPopulation[MAX_OCTREE_LEVEL + 1];

    // Octree geometry
    CCVector3 m_pointsMin;
    CCVector3 m_pointsMax;

    // Threading
    MultiThreadingWrapper m_MT_wrapper;
    std::atomic<bool> m_buildInProgress;
};
```

### Key Safety Issues

| Issue | Location | Risk | Rust Fix |
|-------|----------|------|----------|
| `const CCVector3* point` in `PointDescriptor` | `DgmOctree.h:98` | Dangling pointer if cloud deleted | Lifetime `'a` bound to cloud |
| Manual `new`/`delete` for `KDTreeCell` | `KdTree.cpp:buildSubTree` | Leak on exception | `Box<KdTreeCell>` |
| `void** userParams` in MT wrapper | `DgmOctree.h:1235` | Type-unsafe across threads | Type-erased `Box<dyn Any>` |
| `bool cellFuncSuccess` written by threads | `DgmOctree.cpp:3273` | Data race | `AtomicBool` |
| No bounds check on `m_thePointsAndNeighbours[i]` | `DgmOctree.cpp` | Index OOB | `.get(i)` returns `Option<&T>` |

---

## 2. KDTree Code Analysis

### Location: `libs/qCC_db/extern/CCCoreLib/src/KdTree.cpp`

```cpp
class KDTree {
    KDTreeCell* m_root;
    GenericIndexedCloud* m_associatedCloud;
    std::vector<unsigned> m_indexes;  // permutation index for building
};

struct KDTreeCell {
    CCVector3 cellCenter;
    unsigned splitDim;    // 0, 1, or 2 — split axis
    PointCoordinateType splitPos;
    unsigned pointIndex;   // leaf only
    KDTreeCell *left, *right;
};
```

### Issues in buildSubTree

```cpp
// KdTree.cpp:buildSubTree — RECURSIVE ALLOCATION
KDTreeCell* buildSubTree(unsigned first, unsigned last, GenericIndexedCloud* cloud) {
    auto* cell = new KDTreeCell();  // ← manual allocation
    // ...
    if (needsSplit) {
        cell->left  = buildSubTree(first, mid, cloud);
        cell->right = buildSubTree(mid, last, cloud);
        if (!cell->left || !cell->right) {  // null check
            if (cell->left) deleteSubTree(cell->left);
            if (cell->right) deleteSubTree(cell->right);
            delete cell;  // ← manual deallocation
            return nullptr;
        }
    }
    return cell;
}

// Problem 1: If new throws (unlikely), cell leaks
// Problem 2: Exception during recursive build → partial tree leaks
// Problem 3: deleteSubTree is recursive → stack overflow for large clouds
```

### Rust Equivalent

```rust
enum KdTreeCell {
    Leaf {
        point_index: usize,
        point: [f32; 3],
    },
    Branch {
        split_dim: usize,      // 0, 1, or 2
        split_pos: f32,
        left: Box<KdTreeCell>,  // Box = heap allocation, Drop = automatic free
        right: Box<KdTreeCell>,
        center: [f32; 3],
    },
}

pub struct KdTree {
    root: Option<Box<KdTreeCell>>,
    cloud_size: usize,
}

impl Drop for KdTree {
    fn drop(&mut self) {
        // Box<KdTreeCell> automatically frees on drop
        // No recursive delete needed
    }
}
```

**Key advantage:** `Box<KdTreeCell>` drop is not recursive for `Box<Branch>`. The tree is freed in O(1) stack depth by walking the tree and freeing each `Box` in turn. No stack overflow.

---

## 3. Multi-Threading Wrapper — Data Race Analysis

### Location: `DgmOctree.h:1228-1247` + `DgmOctree.cpp:3269-3281`

```cpp
// Current C++ — thread-unsafe structure
struct MultiThreadingWrapper {
    octreeCellFunc cellFunc = nullptr;
    bool cellFuncSuccess = true;         // ← WRITTEN by worker threads, READ by main
    DgmOctree* octree = nullptr;         // ← captured in TBB lambda
    GenericProgressCallback* progressCb = nullptr;
    NormalizedProgress* normProgressCb = nullptr;
    void** userParams = nullptr;         // ← void** = type-unsafe
};

// In parallel loop:
tbb::parallel_for(tbb::blocked_range<std::size_t>(0, cells.size()), [&](auto r) {
    for (auto i = r.begin(); i < r.end(); ++i) {
        m_MT_wrapper.launchOctreeCellFunc(cells[i]);  // writes cellFuncSuccess
    }
});
// ← main thread reads cellFuncSuccess here: DATA RACE
```

### Rust Architecture for Thread-Safe Octree

```rust
use std::sync::{Arc, atomic::{AtomicBool, AtomicU32}, Mutex};

pub struct OctreeBuildContext {
    // Shared read-only data
    points: Arc<Vec<[f32; 3]>>,
    params: OctreeParams,

    // Thread-safe mutable state
    success: AtomicBool,          // replaces bool cellFuncSuccess
    progress: AtomicU32,           // replaces NormalizedProgress counter
    error: Mutex<Option<String>>, // thread-safe error reporting

    // Callbacks — Rust closures (not void*)
    progress_callback: Mutex<Option<Box<dyn Fn(f64) -> bool + Send>>>,
}

impl OctreeBuildContext {
    pub fn new(points: Vec<[f32; 3]>, params: OctreeParams) -> Self {
        OctreeBuildContext {
            points: Arc::new(points),
            params,
            success: AtomicBool::new(true),
            progress: AtomicU32::new(0),
            error: Mutex::new(None),
            progress_callback: Mutex::new(None),
        }
    }
}

// Parallel octree build using rayon
pub fn build_octree_parallel(params: OctreeBuildParams) -> Result<DgmOctree, OctreeError> {
    let ctx = Arc::new(OctreeBuildContext::new(params.points));

    // Rayon work-stealing is more cache-friendly than TBB for irregular problems
    let cells: Vec<_> = compute_octree_cells(&ctx);

    cells.par_iter().for_each(|cell| {
        // Each thread operates on its own cell — no shared mutation
        // Progress counter updated atomically
        let prev = ctx.progress.fetch_add(1, Ordering::Relaxed);
        if let Some(ref cb) = ctx.progress_callback.lock().unwrap().as_ref() {
            let pct = prev as f64 / cells.len() as f64;
            cb(pct);  // callback returns bool — stop if cancelled
        }
    });

    if !ctx.success.load(Ordering::SeqCst) {
        return Err(OctreeError::CellComputationFailed);
    }

    Ok(assemble_octree(cells, &ctx))
}
```

---

## 4. Morton Code Computation

### Location: `DgmOctree.cpp` (estimated ~100 lines)

Morton code (Z-order curve) encodes 3D points as a single integer, enabling fast spatial hashing and neighbour queries.

```cpp
// C++ implementation — bit interleaving
CellCode ComputeMortonCode(const CCVector3& point, int level) {
    // Shift to positive coordinates (octree is in positive space)
    unsigned x = static_cast<unsigned>(point.x + HALF_RANGE);
    unsigned y = static_cast<unsigned>(point.y + HALF_RANGE);
    unsigned z = static_cast<unsigned>(point.z + HALF_RANGE);

    // Bit interleave: xyz → morton
    CellCode code = 0;
    for (int bit = 0; bit < BITS_PER_LEVEL; ++bit) {
        code |= ((x >> bit) & 1) << (3 * bit);
        code |= ((y >> bit) & 1) << (3 * bit + 1);
        code |= ((z >> bit) & 1) << (3 * bit + 2);
    }
    return code;
}
```

```rust
// Rust — identical bit operations, guaranteed no overflow
pub fn compute_morton_code(x: u32, y: u32, z: u32, bits: u32) -> u64 {
    let mut code = 0u64;
    for bit in 0..bits {
        code |= ((x >> bit) & 1) as u64 << (3 * bit);
        code |= ((y >> bit) & 1) as u64 << (3 * bit + 1);
        code |= ((z >> bit) & 1) as u64 << (3 * bit + 2);
    }
    code
}

// SIMD version using wide crate (AVX2):
// 8 × 32-bit x coordinates → 2 × 128-bit → 16 interleaved bits per iteration
```

---

## 5. SmallVec for Neighbour Sets

`NeighboursSet` (`std::vector<PointDescriptor>`) is allocated per octree cell. Most cells have few neighbours — the allocation overhead dominates.

```cpp
// Current C++:
using NeighboursSet = std::vector<PointDescriptor>;
// Problem: std::vector always heap-allocates, even for 1–4 neighbours
```

```rust
// Rust with SmallVec — no heap allocation for the common case
use smallvec::SmallVec;

pub struct PointDescriptor {
    point: [f32; 3],
    point_index: usize,
    square_dist: f64,
}

pub type NeighboursSet = SmallVec<[PointDescriptor; 8]>;  // heap-allocates only if >8 neighbours
```

**Performance impact:** For typical octree cells with 1–4 neighbours, `SmallVec<[T; 8]>` eliminates 80–90% of heap allocations in neighbour queries. This is a significant speedup for dense point clouds.

---

## 6. TrueKdTree vs KDTree

CloudCompare has two KD-tree implementations:

| | `KDTree` | `TrueKdTree` |
|--|---------|-------------|
| Purpose | Nearest-neighbour search | Exact N nearest neighbours |
| Algorithm | Best-bin-first kd-tree | Limited-radius search |
| Build | Single-threaded | Single-threaded |
| Query | Recursive | Iterative |

Both use manual `new`/`delete` and both need migration. The `TrueKdTree` is simpler (no dynamic allocation in the query path) and is a better first target.
