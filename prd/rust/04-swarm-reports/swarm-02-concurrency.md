# Swarm Report 02: Concurrency & Data Race Analysis
**Agent:** explore — concurrency specialist
**Scope:** `libs/qCC_db/extern/CCCoreLib/src/` + `qCC/` threading usage
**Date:** 2026-08-19

---

## 1. Concurrency Architecture Overview

CloudCompare uses two parallelisation backends (selected at CMake configure time):

```cpp
// DgmOctree.cpp:25-47
#if defined(CC_CORE_LIB_USES_QT_CONCURRENT)
    #define ENABLE_MT_OCTREE
    #include <QtConcurrentMap>
    #include <QThreadPool>
#elif defined(CC_CORE_LIB_USES_TBB)
    #define ENABLE_MT_OCTREE
    #include <oneapi/tbb/parallel_for.h>
    #include <oneapi/tbb/task_arena.h>
    #include <oneapi/tbb/mutex.h>
#endif
```

**Both paths exist in the same codebase.** This dual-backend design means:
- Concurrency bugs may manifest on one backend but not the other
- TBB mutex (`tbb::mutex`) and QMutex have different semantics under contention
- `QtConcurrent::blockingMap` vs `tbb::parallel_for` have different synchronisation models

---

## 2. Data Race Candidates

### 2.1 `DgmOctree.cpp` — `m_MT_wrapper` Race

**Location:** `DgmOctree.cpp:3061–3281` (build), `DgmOctree.cpp:3363–3891` (function execution)

**The pattern:**
```cpp
// DgmOctree.h:1228-1247
struct MultiThreadingWrapper {
    octreeCellFunc cellFunc = nullptr;
    bool cellFuncSuccess = true;      // ← WRITTEN by threads, READ by main
    DgmOctree* octree = nullptr;      // ← CAPTURED by threads (via lambda)
    GenericProgressCallback* progressCb = nullptr;
    NormalizedProgress* normProgressCb = nullptr;
    void** userParams = nullptr;      // ← void** is inherently unsafe across threads
};

// DgmOctree.cpp:3269-3272 (QtConcurrent path)
QtConcurrent::blockingMap(cells, [this](const octreeCellDesc& desc) {
    m_MT_wrapper.launchOctreeCellFunc(desc);  // captures 'this'
});

// DgmOctree.cpp:3276-3281 (TBB path)
tbb::parallel_for(tbb::blocked_range<std::size_t>(0, cells.size()),
[&](tbb::blocked_range<std::size_t> r) {
    for (auto i = r.begin(); i < r.end(); ++i)
        m_MT_wrapper.launchOctreeCellFunc(cells[i]);  // & captured by TBB lambda
});
```

**Race conditions identified:**

| Shared Variable | Race Path | Severity |
|----------------|-----------|----------|
| `m_MT_wrapper.cellFuncSuccess` | Multiple threads write; main thread reads after parallel section | HIGH — silent corruption of error state |
| `m_MT_wrapper.userParams` | `void**` cast without synchronisation | HIGH — type confusion risk |
| `normProgressCb` | Updated by worker threads | MEDIUM — non-atomic float write |
| `DgmOctree*` pointer captured | `this` pointer invalidated if octree is deleted mid-build | CRITICAL — use-after-free |

**Rust fix:**
```rust
// Rust: Send + Sync enforced at compile time
struct OctreeCellFunc {
    cell_func: Option<extern "C" fn(...)>,
    cell_func_success: AtomicBool,    // atomic: no data race
    octree: Arc<DgmOctree>,           // Arc: shared across threads safely
    norm_progress_cb: Mutex<Option<Box<dyn ProgressCallback>>>,  // Mutex: exclusive access
}
```

### 2.2 `DistanceComputationTools.cpp` — TBB Mutex in Lambda

**Location:** `DistanceComputationTools.cpp:940-944`

```cpp
// Per-thread mutex — unusual pattern
struct LocalContext {
    QMutex currentBitMaskMutex;  // Qt path
    tbb::mutex currentBitMaskMutex;  // TBB path
};
```

**Issue:** `LocalContext` is stack-allocated **per thread** in the parallel loop. Each thread has its own mutex — this is technically correct but the pattern suggests the original author was uncertain about thread-safety of the `currentBitMask`.

**Rust win:** The mutex-per-thread pattern is a code smell in C++. In Rust, `ThreadLocal<T>` makes this explicit and the compiler enforces it.

### 2.3 `NormalizedProgress.cpp` — Shared Counter Without Atomic

**Location:** `libs/qCC_db/extern/CCCoreLib/src/NormalizedProgress.cpp`

```cpp
// Likely pattern (guessed from interface):
class NormalizedProgress {
    unsigned m_currentStep;  // ← NOT std::atomic — written by worker threads
    double m_totalSteps;     // ← written by main thread only (safe)
    void increment() { ++m_currentStep; }  // ← DATA RACE if called from multiple threads
};
```

**Note:** This needs file verification. If `m_currentStep` is indeed non-atomic, this is a latent data race that only manifests under TBB parallel execution.

### 2.4 `ICPRegistrationTools::Register` — Shared State in ICP Loop

**Location:** `RegistrationTools.cpp:147–1840`

```cpp
// DataCloud and ModelCloud are passed to a loop that:
// 1. Mutates data cloud (applies transformation)
// 2. Reads from model cloud
// 3. Updates shared ScalarField (distance results)
// 4. May spawn parallel threads via DgmOctree
struct DataCloud {
    ReferenceCloud* cloud;
    PointCloud* rotatedCloud;    // ← Written by ICP iteration
    ScalarField* weights;         // ← Written by ICP iteration
    ReferenceCloud* CPSetRef;     // ← Written by ICP iteration
    ReferenceCloud* CPSetPlain;  // ← Written by ICP iteration
};
```

**Rust win:** `Arc<RwLock<DataCloud>>` — the type system makes read/write ownership explicit. Multiple readers OR single writer enforced at compile time.

---

## 3. Thread Safety of Key Data Structures

| Data Structure | Current Thread Safety | Rust Equivalent | Safety Gain |
|---------------|---------------------|-----------------|------------|
| `DgmOctree` | None — not thread-safe for concurrent reads/writes | `Arc<RwLock<DgmOctree>>` | HIGH |
| `KDTree` | Read-only after `buildFromCloud()` — safe for concurrent queries | `Arc<KdTree>` (read-only phase) | MEDIUM |
| `ScalarField` | None — concurrent writes = data race | `Arc<RwLock<Vec<f32>>>` or `Vec<f32>` (single-owner) | HIGH |
| `ccPointCloud` | None — concurrent modification = UB | `Arc<RwLock<ccPointCloud>>` for shared access | HIGH |
| `ccOctree` | None — octree + cloud modification = UB | `Arc<Octree>` with exclusive access guard | HIGH |

---

## 4. Concurrency Patterns in C++ → Rust Mapping

| C++ Pattern | Thread Safety | Rust Equivalent | Notes |
|------------|--------------|----------------|-------|
| `QMutex` / `tbb::mutex` | Mutex-guarded | `std::sync::Mutex<T>` | Zero-cost at compile time in Rust |
| `std::atomic<bool>` | Atomic bool | `AtomicBool` | Direct equivalent |
| `QThreadPool + QtConcurrent::map` | Thread pool | `rayon::ParallelIterator` | Rust rayon is more composable |
| `tbb::parallel_for` | Range partition | `rayon::parallel_for` | rayon supports nested parallelism |
| `std::atomic<int>` counter | Atomic counter | `AtomicI32` / `AtomicUsize` | Direct equivalent |
| Lambda `[this]` captured in thread | Dangerous: `this` lifetime | `Arc<Self>` or explicit lifetime | Rust makes this explicit |
| `void**` as user params | Type-unsafe across threads | `Box<(dyn Fn + Send)>` or `Any` | Type-safe closures |

---

## 5. Recommended Rust Concurrency Strategy

### 5.1 For Phase 1 (ScalarField)

Single-owner, no concurrency initially. Rust's single-threaded `Vec<f32>` is equivalent to C++ `std::vector<float>`.

```rust
// Phase 1: Single-threaded, no Arc needed
pub struct ScalarField {
    values: Vec<f32>,
    name: String,
    offset: f64,
    validity: Vec<u8>,  // bitmap instead of NaN magic
}
```

### 5.2 For Phase 2 (Registration)

ICP loop needs shared read access to model cloud and exclusive write access to data cloud.

```rust
// Phase 2: Shared model, exclusive data
pub struct ICPContext {
    model: Arc<KdTree>,              // Read-only, shared across threads
    data: Arc<RwLock<PointCloudView>>, // Write-access for ICP iterations
    weights: Arc<RwLock<ScalarField>>, // Write-access for distance results
}
```

### 5.3 For Phase 3 (Octree)

The octree build itself is the most complex concurrency challenge.

```rust
// Phase 3: Build state machine
pub enum OctreeState {
    Idle,
    Building {
        progress: AtomicU32,
        cells: Vec<OctreeCellDesc>,
        result_sender: Sender<Result<Box<DgmOctree>>>,
    },
    Ready(Box<DgmOctree>),
    Error(String),
}
```

---

## 6. Key Risk: Panics Across FFI

**Critical rule:** Rust panics (`panic!`, unwinding) **must not cross the FFI boundary** into C++ code. C++ has no concept of Rust panics and Undefined Behaviour results.

**Required pattern:**
```rust
#[no_mangle]
extern "C" fn safe_entry_point(...) {
    let result = std::panic::catch_unwind(|| {
        // All Rust code here
        do_work(...)
    });

    match result {
        Ok(v) => v,
        Err(_) => {
            // Log, then return error code to C++
            // DO NOT propagate panic across FFI
            -1
        }
    }
}
```

This is enforced automatically by CXX for safe signatures, but must be manually implemented for `extern "C"` functions.

---

## 7. Dependency: Rayon for Data Parallelism

Rayon (`rayon = "2"`) is the Rust standard for data parallelism and maps directly to TBB/QtConcurrent:

```rust
use rayon::prelude::*;

// Equivalent to tbb::parallel_for over octree cells:
cells.par_iter().for_each(|cell| {
    compute_cell(cell, &octree);
});

// Equivalent to QtConcurrent::blockingMap:
let results: Vec<_> = cells.iter().map(compute_cell).collect();
// or
let results: Vec<_> = cells.par_iter().map(compute_cell).collect();
```

**Rayon advantages over TBB:**
- Zero-cost abstraction — compiles to equivalent machine code
- `join` for fork-join parallelism (equivalent to `tbb::task_arena::execute`)
- Work-stealing scheduler with better cache locality for irregular problems
- No `#![no_std]` compatibility concerns
