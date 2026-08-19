# Swarm Report 05: Plugin System & CCPluginAPI Analysis
**Agent:** explore — plugin architecture specialist
**Scope:** `libs/CCPluginAPI/`, `plugins/core/`, `qCC/pluginManager/`
**Date:** 2026-08-19

---

## 1. Plugin Architecture Overview

CloudCompare uses a Qt plugin loader pattern:

```
qCC (main app)
  └── ccPluginManager (manages lifecycle)
        └── CCPluginInterface (base class)
              ├── StandardPlugin (UI actions: qCSF, qM3C2, etc.)
              ├── IOPlugin (file formats: qLASIO, qCoreIO)
              └── GLPlugin (OpenGL post-filters: qEDL, qSSAO)
```

**Key files:**
- `libs/CCPluginAPI/ccPluginInterface.h` — plugin base interface
- `qCC/pluginManager/ccPluginManager.cpp` — plugin discovery/loading
- `qCC/pluginManager/ccPluginInfoDlg.cpp` — About dialog showing loaded plugins

---

## 2. Plugin Interface Analysis

### 2.1 `ccPluginInterface`

```cpp
// libs/CCPluginAPI/ccPluginInterface.h (estimated structure)
class ccPluginInterface {
public:
    virtual ~ccPluginInterface() = default;
    virtual QString getName() const = 0;
    virtual QString getDescription() const = 0;
    virtual QString getVersion() const = 0;
    virtual void setMainAppInterface(ccMainAppInterface* app) = 0;
};
```

**Rust FFI:** This is a C++ abstract base class. CXX supports this via `extern "C++"` with opaque types:

```cpp
// In CXX bridge file:
extern "C++" {
    type ccPluginInterface;  // Opaque — Rust sees it as an interface
    fn ccPluginInterface_getName(self: &ccPluginInterface) -> String;
}
```

**Problem:** Virtual dispatch across FFI requires the Rust side to call through the C++ vtable. This is possible but complex. The `ccMainAppInterface*` parameter is a massive interface (~200 methods) that would require a massive FFI surface.

**Recommendation:** Do **not** attempt to make Rust plugins loadable as CloudCompare plugins. The Qt plugin ABI and `ccMainAppInterface` dependency graph are too deep.

### 2.2 Plugin API Surface

The plugin API exposes these core services to plugins:

| Service | Methods | Qt Dependency | FFI Feasibility |
|---------|---------|---------------|----------------|
| `ccMainAppInterface` | ~200 methods | Full Qt | **Not feasible** — too large |
| `ccDBRoot` (db-tree) | ~50 methods | Qt widgets | Low priority |
| `ccGLWindow` (viewport) | ~100 methods | Qt + OpenGL | **Never** — OpenGL |
| `ccPickingHub` (3D picking) | ~20 methods | Qt signals | Medium — can be wrapped |
| `FileIOFilter` (file save/load) | ~30 methods | Qt streams | **Good candidate** |
| `GenericProgressCallback` | ~5 methods | None | **Best candidate** — no Qt |

---

## 3. Priority: GenericProgressCallback

This is the **simplest and most valuable** plugin API surface to migrate to Rust:

```cpp
// libs/CCPluginAPI/include/ccMainAppInterface.h (estimated)
class GenericProgressCallback {
public:
    virtual ~GenericProgressCallback() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void update(double progress) = 0;  // 0.0–1.0
    virtual void setInfo(const QString& info) = 0;
    virtual void setMethod(const QString& method) = 0;
    virtual bool isCancelRequested() const = 0;
    virtual bool textCanBeEdited() const = 0;
    virtual bool isEnabled() const = 0;
    virtual void setLabel(const QString& label) = 0;
    // ...
};
```

**Usage in CCCoreLib:**
- `DgmOctree.cpp`: `m_normProgressCb` — progress reporting during octree build
- `DistanceComputationTools.cpp`: progress reporting during distance computation
- `RegistrationTools.cpp`: `GenericProgressCallback* progressCb` — progress during ICP

**Rust migration:**
```rust
// cc-rust/src/ffi/progress.rs
#[cxx::bridge]
mod ffi {
    extern "C++" {
        type GenericProgressCallback;
    }

    extern "Rust" {
        fn report_progress(
            cb: &mut Option<Box<dyn ProgressCallback>>,
            progress: f64,
            info: &str,
        ) -> bool;
    }
}

pub trait ProgressCallback {
    fn start(&mut self);
    fn stop(&mut self);
    fn update(&mut self, progress: f64) -> bool;  // returns false if cancelled
    fn set_info(&mut self, info: &str);
    fn is_cancel_requested(&self) -> bool;
}
```

This allows Rust computation code to receive progress callbacks from C++ without exposing the full `ccMainAppInterface`.

---

## 4. Plugin Loading — Why Not to Migrate

The plugin loading mechanism is fundamentally incompatible with Rust FFI:

```cpp
// qCC/pluginManager/ccPluginManager.cpp
void ccPluginManager::loadPlugins() {
    // Uses QPluginLoader to load .dll/.so files
    // The .dll must export a C++ factory function:
    //   extern "C" Q_DECL_EXPORT ccPluginInterface* createPlugin() {
    //       return new qMyPlugin();
    //   }
}
```

**Problem:** Qt's plugin system requires:
1. The plugin to be compiled with the **same Qt version** as the host app
2. The plugin to export C++ objects (not Rust structs) via the Qt plugin metadata system
3. `ccPluginInterface` to be loadable via `QPluginLoader` — which uses `dlsym` / `GetProcAddress` on the C++ factory function

**Rust cannot export C++ virtual dispatch tables** through its ABI. A Rust plugin cannot implement `ccPluginInterface` in a way that `QPluginLoader` can discover.

**Alternative:** Rust plugins could be loaded as **separate processes** communicating via IPC (JSON-RPC, gRPC, or Unix pipes). This is the approach `qJSonRPCPlugin` takes for the JSON-RPC server — but this plugin is currently disabled (needs Qt6 Network + WebSockets).

---

## 5. Strategic Recommendation

For the plugin system:

1. **Keep the Qt plugin architecture in C++** — it works, is well-tested, and is the right tool for the job
2. **Add a Rust subprocess communication channel** — plugins that need heavy computation (LAS parsing, octree building) spawn a Rust process, send input via IPC, receive results
3. **Use `qJSonRPCPlugin` as the model** — if/when that plugin is fixed for Qt 6.8.3, it provides the RPC bridge

```cpp
// Plugin (C++) → Rust subprocess
// Example: qLASIO plugin loads .las, sends raw bytes to Rust process:
struct LASProcessResult {
    ccPointCloud* cloud;       // wrapped in C++
    ScalarField* intensities;  // wrapped in C++
};
// The Rust process handles:
//   1. LASzip decompression (C++ → Rust via CXX)
//   2. Point validation (pure Rust)
//   3. Return structured result (CXX bridge)
```

---

## 6. CCPluginAPI FFI Surface Summary

| Interface | Methods | FFI Direction | Qt Deps | Recommendation |
|-----------|---------|--------------|---------|---------------|
| `GenericProgressCallback` | 8 | C++ → Rust | None | **Migrate (Phase 1)** |
| `GenericCloud` | 20 | Bidirectional | None | **Migrate (Phase 1)** |
| `ccDrawableObject` | 40 | C++ → Rust | OpenGL | **Partial (geometry only)** |
| `FileIOFilter` | 30 | Bidirectional | Qt I/O | **Migrate parser logic only** |
| `ccMainAppInterface` | 200+ | C++ → Rust | Full Qt | **Never** |
| `ccGLWindow` | 100+ | C++ → Rust | Qt+OpenGL | **Never** |
| `ccPickingHub` | 20 | Bidirectional | Qt signals | **Defer** |

---

## 7. Memory Safety Boundary for Plugin API

The key insight: **FFI safety in the plugin context is about data ownership at the boundary, not plugin interoperability.**

```
C++ plugin (qLASIO)
    ↓ [owns: raw bytes, filename]
    ↓ calls: Rust LAS parser (via CXX)
    ↑ returns: PointCloudView (no ownership transfer, view only)
    ↓ C++ wraps: PointCloudView → ccPointCloud (copies data)
```

**Rule:** Rust functions exposed to C++ should **never return borrowed references** to Rust-owned data that outlives the FFI call. Always copy or transfer ownership:

```rust
// WRONG — borrowed reference crosses FFI boundary
#[no_mangle]
extern "C" fn parse_las(path: *const c_char) -> *const PointView {
    let point_view = parse(path);  // Rust-owned
    std::mem::transmute(point_view)  // DANGER: use-after-free
}

// CORRECT — ownership transferred to C++
#[no_mangle]
extern "C" fn parse_las(path: *const c_char) -> *mut PointCloudData {
    let data = parse(path);  // Rust-owned Vec
    Box::into_raw(Box::new(data))  // C++ must call destroy_point_cloud_data()
}

// BEST — CXX handles ownership automatically
#[cxx::bridge]
mod ffi {
    extern "Rust" {
        fn parse_las(path: &str) -> Vec<u8>;  // CXX converts Vec → std::vector
    }
}
```
