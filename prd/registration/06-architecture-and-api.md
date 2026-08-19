# 06 — Architecture & API

> **How is it built? What can other plugins call into it?**

This document is for the implementer. It is the bridge between the wireframes in [`04-ui-pages.md`](04-ui-pages.md) and the actual C++/Qt code that gets written. It assumes the reader has read [`../../AGENTS_REGISTRATION.md`](../../AGENTS_REGISTRATION.md) and [`../../docs/context/registration/`](../../docs/context/registration/) — those are the agent-facing "how" docs. This is the product-facing "how" doc: less code-archaeology, more design intent and contract.

---

## 1. The plugin's responsibility boundaries

```
+-----------------------------------------------------------+
| qRegistration plugin (this PRD)                          |
|                                                           |
|  - Registration tree panel (UX)                           |
|  - Correspondence view (UX)                               |
|  - Target detection orchestration                         |
|  - Auto-registration orchestration                        |
|  - Verify overlay (UX)                                    |
|  - Report generation                                      |
|  - Sidecar I/O                                            |
|  - CLI command                                            |
|  - Public API for other plugins                           |
+-----------------------------------------------------------+
                |
                v
+-----------------------------------------------------------+
| CloudCompare core (READ-ONLY in v1)                      |
|                                                           |
|  - ccHObject (db-tree, hierarchy, metadata)               |
|  - ccPickingHub (picking events)                          |
|  - ccGLWindow (3D viewport widget)                        |
|  - ccMainAppInterface (createGLWindow, addToDB, etc.)     |
|  - CCCoreLib (math: registration, ICP, scalar fields)     |
|  - ccGui::ParamStruct (user preferences)                  |
+-----------------------------------------------------------+
```

The plugin **delegates** to the core for math and rendering. It **owns** the user-facing surface. It does not modify `qCC/`, `ccViewer/`, or `libs/` for v1.

The one exception: **the plugin's API surface is exposed via `ccMainAppInterface`** if we need to push a method up. But for v1, all the primitives we need are already exposed.

---

## 2. The class structure

### 2.1 Top-level

```
qRegistration                           (ccStdPluginInterface)
├── qRegistrationDlg                    (QMainWindow — the registration tree panel)
│   ├── RegistrationTreeModel           (QAbstractItemModel)
│   ├── RegistrationTreeView            (QTreeView)
│   └── ClusterPropertiesDlg            (QDialog)
├── CorrespondenceViewDlg                (QDialog)
│   ├── CorrespondenceGLWidget           (QWidget wrapping ccGLWindow)
│   ├── PairTableModel                   (QAbstractTableModel)
│   └── PairRegistration                (free function — Umeyama + RMS)
├── TargetDetectionDlg                   (QDialog)
│   └── TargetDetector                   (per-scan worker)
├── AutoRegisterWorker                   (QObject — runs on QtConcurrent::run)
│   ├── TBRRegistration                 (target-based)
│   ├── TDRRegistration                 (top-view)
│   ├── C2CRegistration                 (ICP)
│   └── HybridRegistration              (TDR + C2C, the default)
├── VerifyOverlayDlg                     (QDialog)
├── ReportViewerDlg                      (QDialog)
├── TargetEditorDlg                      (QDialog)
├── CliCommand                           (ccCommandLineInterface::Command)
├── Sidecar                              (I/O — read/write .qreg.json)
└── RegistrationApi                      (public API for other plugins)
```

### 2.2 The data model

```cpp
namespace qreg
{
    using ClusterId = uint64_t;  // stable across save/load; FNV-1a of the path

    enum class RegistrationState {
        Unregistered,
        InProgress,
        Registered,
        Stale,
        Failed
    };

    enum class LockState {
        Free,        // not locked
        Locked,      // locked
    };

    struct PointPair {
        CCVector3d aligned;     // picked in viewport A
        CCVector3d reference;   // picked in viewport B
    };

    struct RegistrationReport {
        QString mode;                  // "TBR", "TDR", "C2C", "TDR_C2C", "Manual"
        QDateTime timestamp;
        double rmsOverall = 0.0;
        QString rmsUnits = "m";
        QList<PairResult> pairs;       // per-pair residuals
        QList<TransformResult> transforms;
    };

    struct ClusterRegistration {
        ClusterId id;
        QString name;
        QString path;                  // "/Project/Building/Floor1"
        LockState lock = LockState::Free;
        bool isReference = false;
        RegistrationState state = RegistrationState::Unregistered;
        RegistrationReport lastReport;
        QList<PointPair> savedPairs;
        QList<QString> childClusterIds;
        QList<QString> childScanIds;
    };

    struct ProjectRegistration {
        QList<ClusterRegistration> clusters;
        // scan-level transforms are stored in the .bin; the sidecar
        // stores the cluster-level relationships.
    };
}
```

This is the in-memory model. It's serialised to the `.qreg.json` sidecar (see §6).

---

## 3. The public API (for other plugins)

This is the surface that other CloudCompare plugins can call into. It's a small, deliberate API — not the full internal model.

```cpp
namespace qreg
{
    class RegistrationApi
    {
    public:
        /// Check if a cluster is locked.
        virtual bool isClusterLocked(ClusterId id) const = 0;

        /// Check if a cluster is the reference of its parent.
        virtual bool isClusterReference(ClusterId id) const = 0;

        /// Lock or unlock a cluster. Returns false if the operation is invalid
        /// (e.g. trying to unlock while a parent is registered to it).
        virtual bool setClusterLocked(ClusterId id, bool locked) = 0;

        /// Set a cluster as the reference of its parent. Auto-clears reference
        /// on any other sibling. Returns false if the cluster has no parent
        /// (references only make sense within a parent).
        virtual bool setClusterReference(ClusterId id, bool isReference) = 0;

        /// Get the current registration state of a cluster.
        virtual RegistrationState clusterState(ClusterId id) const = 0;

        /// Run an auto-registration on a cluster. Returns the report on success,
        /// or an empty report + an error string on failure. The report is also
        /// stored in the cluster's lastReport.
        virtual RegistrationReport autoRegister(
            ClusterId id,
            RegistrationMode mode,
            QString* error = nullptr) = 0;

        /// Open the correspondence view (manual point-pair) for two scans or
        /// two clusters. The dialog is non-modal; the caller can connect to
        /// the `committed` signal to know when the user clicks Apply.
        virtual void openCorrespondenceView(
            ccHObject* aligned,
            ccHObject* reference) = 0;

        /// Get the last registration report for a cluster.
        virtual RegistrationReport lastReport(ClusterId id) const = 0;

        /// Listen for registration events. Connect before the user does
        /// anything; you'll get notified when any cluster's state changes.
        virtual void connectEventListener(QObject* listener, const char* slot) = 0;

    signals:
        void clusterStateChanged(ClusterId id, RegistrationState newState);
        void registrationCommitted(ClusterId id, RegistrationReport report);
        void clusterLockChanged(ClusterId id, bool locked);
        void clusterReferenceChanged(ClusterId id, bool isReference);
    };
}
```

The plugin's `getActions()` method also returns a `RegistrationApi*` so other plugins can call into it. This is the existing `ccStdPluginInterface` pattern extended.

### 3.1 Why these specific methods

The API is the **minimum** a downstream plugin would need to integrate with the registration workflow. Concretely:

- A custom report generator (think "send report to client portal") calls `lastReport()` and listens for `registrationCommitted`.
- A custom UI (think "show registration status in my own dock") calls `clusterState()`, `isClusterLocked()`, `isClusterReference()`, and listens for state-change signals.
- A custom registration mode (think "ICP with a custom metric") would NOT be in v1 — we'd add `registerCustomMode()` in v2 if there's demand.

### 3.2 What the API does NOT expose

- The internal `ProjectRegistration` struct (it's serialisation detail).
- The `ccGLWindow` widgets inside the correspondence view (other plugins can't manipulate them directly).
- The CLI command (CLI is the plugin's own surface, not a callable API).
- The sidecar format (it's an implementation detail; could change).

---

## 4. The math

### 4.1 The five algorithms in v1

| Algorithm | Used by | Library call | Notes |
|---|---|---|---|
| **Umeyama (rigid)** | Manual point-pair (FR-70) | `UmeyamaRegistration` from `qCC/ccPointPairRegistrationDlg.cpp` | Rigid only; scale is discarded |
| **Horn (rigid)** | TBR, fallback for manual | `CCCoreLib::HornRegistrationTools::FindAbsoluteOrientation` | Same result as Umeyama for rigid; legacy |
| **ICP (point-to-point)** | C2C | `CCCoreLib::ICP` | Standard, well-tested |
| **ICP (point-to-plane)** | C2C, optional | `CCCoreLib::ICP` with `CCCoreLib::ICPLib::PointToPlane` | Better for surfaces; v1 supports the flag |
| **Top-view (XY projection)** | TDR | Custom (centroid alignment on projected 2D) | Trivial, but tested |

### 4.2 The Umeyama path (manual point-pair)

The `qCC/ccPointPairRegistrationDlg.cpp::UmeyamaRegistration` function is **directly reusable** with one change: we wrap it in a free function `qreg::fitRigidFromPairs(pairs, outTransform, outRms)` so it doesn't depend on the dialog's member state.

```cpp
namespace qreg
{
    struct FitResult {
        ccGLMatrixd transform;
        double rms = 0.0;
        QList<double> perPairResiduals;
        bool success = false;
        QString errorMessage;
    };

    FitResult fitRigidFromPairs(const QList<PointPair>& pairs);
}
```

The implementation mirrors the existing function. The only delta is error handling: instead of returning a matrix with no signal of success/failure, we return a `FitResult` that explicitly says.

### 4.3 The ICP path (C2C)

The plugin wraps `CCCoreLib::ICP` in a function that takes two `ccPointCloud*` and a `ccGLMatrixd` initial guess:

```cpp
namespace qreg
{
    struct IcpParams {
        int maxIterations = 50;
        double rmsEpsilon = 1e-4;       // 0.1mm
        int multiResolutionLevels = 1;  // 1 = single res, 2-3 = multi
        bool pointToPlane = false;
    };

    struct IcpResult {
        ccGLMatrixd transform;
        double finalRms = 0.0;
        int iterationsUsed = 0;
        QString errorMessage;
    };

    IcpResult runIcp(
        ccPointCloud* source,
        ccPointCloud* target,
        const ccGLMatrixd& initialGuess,
        const IcpParams& params = IcpParams());
}
```

The implementation **does not re-implement ICP**. It calls `CCCoreLib::ICP` and translates the inputs / outputs. The wrapping exists for:
- Error reporting (CCCoreLib's ICP returns silently on divergence; we detect this).
- Multi-resolution (CCCoreLib doesn't ship multi-res ICP; we subsample the source and run ICP at each level).
- Point-to-plane (CCCoreLib supports it but the existing wrapper has a different API).

### 4.4 The TDR path

The TDR function is custom (CCCoreLib doesn't have it). It:

1. Projects both point clouds onto the user's "up" axis (default +Z).
2. Computes the 2D centroid of each.
3. Computes the 2D rotation that best aligns the two 2D point sets (using a simplified 2D ICP, or just centroid + angle if scans are pre-rotated).
4. Returns a `ccGLMatrixd` with the rotation around the up axis and the centroid translation.

This is a **coarse** alignment. It's expected that TDR will be followed by C2C. The RMS from TDR is reported but flagged as "coarse".

### 4.5 The TBR path

The TBR function:

1. For each pair of child scans, find the target matches.
2. For each pair, fit a rigid transform (using `fitRigidFromPairs` with the matched targets as "pairs").
3. Apply outlier rejection (3 × RMS threshold).
4. Re-fit.
5. Return the per-pair transforms and overall RMS.

Target matching itself is the trickier part; it lives in `qreg::matchTargets`. The matching strategies are "by name" (FR-32 strategy 1) and "geometric nearest within radius" (FR-32 strategy 2).

### 4.6 The math invariants (test contracts)

Every registration function in the plugin has a set of invariants the QA team tests against:

1. **Identity input → identity output.** If the two clouds are already aligned, the transform is (numerically) the identity.
2. **Translation symmetry.** Translating one cloud by a known vector T and re-registering gives T back.
3. **Rotation symmetry.** Rotating one cloud by a known matrix R and re-registering gives R back.
4. **Scale discarded.** Scaling one cloud by 2 and re-registering gives a worse RMS, not a different scale factor.
5. **Numerical stability.** A registration with 100 pairs and one outlier gives a result within 1mm of the result with the outlier removed.
6. **Reversibility.** If you apply T to cloud A, then fit T' from A→B and B→A, T' = T^-1.

These are the unit-test contracts. They are not the user-visible behaviour; they are the implementation correctness checks.

---

## 5. Performance budget

The plugin is a desktop application on a Windows laptop. The performance budget:

| Operation | Cloud size | Target time | Hard limit |
|---|---|---|---|
| Detect targets (sphere) | 5M points | 5s | 15s |
| Detect targets (checkerboard) | 5M points | 10s | 30s |
| TBR registration | 8 scans × 5M pts, 30 targets | 10s | 30s |
| TDR coarse | 8 scans × 5M pts | 5s | 15s |
| C2C ICP, 50 iters | 2 × 10M pts | 60s | 180s |
| C2C ICP multi-res (3 levels) | 2 × 10M pts | 90s | 240s |
| Preview transform | any | <100ms | 500ms |
| Apply transform | 10M points | 2s | 5s |
| Save / load sidecar | any | <1s | 3s |
| Open correspondence view | any | <2s | 5s |

**The hard limit is the user-visible timeout.** If an operation hits the hard limit, the dialog reports "Operation took too long; consider [alternative]" and offers to abort.

**The target time is what the user should expect** in a normal job. The plugin is "fast enough" if it hits the target time on a typical Icelabz job (8-15 scans of 3-5M points each).

**Stress tests** are run on a 50M-point cloud to validate the worst case. If the hard limit is exceeded on the stress test, the plugin is broken.

---

## 6. The sidecar format

### 6.1 The file

`<project>.qreg.json` — JSON, UTF-8, schema-versioned.

### 6.2 The schema (v1.0)

```json
{
  "schemaVersion": "1.0",
  "pluginVersion": "1.0.0",
  "savedAt": "2026-08-18T22:14:33Z",
  "scannerHint": "Faro Focus S70",          // optional, best-effort from metadata
  "rootCluster": {
    "id": "f0e1d2c3b4a5",                   // FNV-1a of path, stable
    "name": "Project",
    "lock": "free",                          // "free" | "locked"
    "isReference": false,
    "state": "registered",                   // "unregistered" | "in_progress" | "registered" | "stale" | "failed"
    "lastReport": {                          // optional
      "mode": "TDR_C2C",
      "timestamp": "2026-08-18T22:14:33Z",
      "rmsOverall": 0.0031,
      "rmsUnits": "m",
      "pairs": [
        {"a": "Scan1", "b": "Scan2", "rms": 0.0028, "matchedTargets": 4}
      ],
      "transforms": [
        {"scan": "Scan2", "matrix": [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0.012, 0.005, -0.003, 1]}
      ]
    },
    "children": [
      {
        "id": "a1b2c3d4e5f6",
        "name": "Building",
        "lock": "free",
        "isReference": false,
        "state": "registered",
        "children": [
          {
            "id": "1234567890ab",
            "name": "Floor1",
            "lock": "locked",                // <-- the two-tier lock
            "isReference": true,             // <-- the two-tier reference
            "state": "registered",
            "lastReport": { ... },
            "children": [
              {"id": "scan-id-1", "name": "Scan1", "isScan": true},
              {"id": "scan-id-2", "name": "Scan2", "isScan": true},
              {"id": "scan-id-3", "name": "Scan3", "isScan": true}
            ]
          },
          { "id": "...", "name": "Floor2", "lock": "locked", "isReference": false, "children": [...] }
        ]
      }
    ]
  },
  "pairSets": [                               // optional, only if user saved any
    {
      "name": "Staircase corner pairs",
      "savedAt": "2026-08-18T22:14:33Z",
      "pairs": [
        {"aligned": [1.234, 5.678, 0.012], "reference": [1.245, 5.690, 0.008]}
      ]
    }
  ],
  "settings": {                               // optional, only if user changed defaults
    "rmsThresholdMm": 5.0,
    "icpMaxIterations": 50,
    "sphereRadiusMm": 70.0
  }
}
```

### 6.3 Versioning

- The `schemaVersion` is the format version. Bump on backwards-incompatible changes.
- The `pluginVersion` is the version of the plugin that wrote the file. Used for diagnostics, not for migration.
- The migration strategy is: load the file; if `schemaVersion` is newer than the plugin knows, warn; if it's older, run migrations.

### 6.4 Why sidecar, not in .bin

- The `.bin` format is owned by the core and changes infrequently; we don't want to fork it.
- The sidecar is plain JSON; users can edit it in a text editor if they really need to.
- The sidecar can be versioned in git independently of the .bin.
- Multiple registration "histories" can coexist as multiple sidecars (e.g. `_v1.qreg.json`, `_v2.qreg.json`).

### 6.5 What the sidecar does NOT contain

- The point cloud geometry (in `.bin`).
- The scalar fields (in `.bin`).
- The picking history / picking session state (in-memory only).
- The user's viewport state (in the `.bin` per-viewport state, existing behaviour).

---

## 7. Threading model

The plugin is a desktop application. Threading rules:

- **The UI thread is sacred.** All Qt widget updates happen on the UI thread. No exceptions.
- **Long operations run on `QtConcurrent::run` or a dedicated `QThread`.** The progress dialog is updated via `QMetaObject::invokeMethod` with `Qt::QueuedConnection`.
- **The `ccGLWindow::redraw()` is called on the UI thread.** Worker threads do not call OpenGL.
- **The picking listener (`ccPickingListener::onItemPicked`) is called on the UI thread** (existing behaviour).
- **The `applyRigidTransformation` is called on the UI thread** because it modifies the `ccHObject` tree, which is not thread-safe.

The threading model is the existing CloudCompare pattern. We do not introduce new threading primitives.

### 7.1 The cancel mechanism

Worker operations check a `QAtomicInt` cancel flag. The progress dialog's Cancel button sets the flag. The worker checks it at each safe point (per scan, per ICP iteration, per pair fit).

---

## 8. Error handling

Every function in the public API returns a result struct (or an `std::optional` / `expected` equivalent). Errors are reported as:

1. A `success` boolean.
2. A `QString errorMessage` (user-visible).
3. A `QString technicalDetails` (log-only; for the "Send to developer" copy-paste).

Errors are **never thrown as exceptions**. The plugin is a Qt application; Qt's signal/slot mechanism doesn't propagate exceptions, and we don't want to start mixing paradigms.

The error messages are localised (see [`05-ux-patterns.md` §13](05-ux-patterns.md#13-internationalisation)).

---

## 9. Logging

The plugin logs to:

1. **The existing `ccLog` system** (the console widget in CloudCompare). INFO level for normal events, WARN for recoverable issues, ERROR for failures.
2. **A per-session log file** in the user's temp directory (`%TEMP%/qRegistration_<session-id>.log`). Useful for "send me the log" bug reports.
3. **The `.qreg.json` sidecar** has its own log of "what happened" via the `lastReport.timestamp` and the report's content.

The log levels are:

| Level | When |
|---|---|
| DEBUG | Verbose mode (`-VERBOSE` for CLI). Disabled by default. |
| INFO | Normal events: registration completed, project saved, etc. |
| WARN | Recoverable issues: 1 of 7 pairs failed, scale mismatch detected, etc. |
| ERROR | Failures: registration aborted, sidecar corrupt, ICP diverged. |

---

## 10. Testing strategy

### 10.1 Unit tests

Per the existing CloudCompare pattern: each `.cpp` has a corresponding `_test.cpp` with a `QTest` fixture. Tests use the existing `ccTestLib` helpers. The plugin's tests live at `plugins/core/Standard/qRegistration/tests/`.

Critical unit tests: Umeyama / ICP / TDR invariants (§4.6), target matching (name + geometric), sidecar round-trip + migration, lock / reference state machine (every transition), tree model (every cell, every state).

### 10.2 Integration tests

A small set that exercises the full pipeline on synthetic data:

- 4 synthetic scans with known transforms → TBR → identical transforms back.
- 2 synthetic clouds with known rigid transform → manual point-pair → identical transform back.
- 2 clusters of 3 scans → lock + reference at parent → TDR+C2C → parent transform within 1mm of expected.

Slow (seconds each); run nightly, not on every commit.

### 10.3 Real-data tests (dogfooding)

The plugin is dogfooded on real Icelabz jobs during development. The dogfood criterion is in [`01-product-overview.md` §6.3](01-product-overview.md#63-real-world-datasets-break-assumptions-medium-risk-high-impact): every milestone gets verified on real data, not on unit-test mocks.

Real-data tests are not automated (they require human judgement). They're a release gate: "has the user used this on a real job?" must be yes before v1 ships.

---

## 11. Build & CI

### 11.1 CMake

```cmake
option(PLUGIN_STANDARD_QREGISTRATION "Register the qRegistration Standard plugin" OFF)

if(PLUGIN_STANDARD_QREGISTRATION)
    project(qRegistration)
    AddPlugin(NAME ${PROJECT_NAME} TYPE standard)
    add_subdirectory(src)
    add_subdirectory(include)
    add_subdirectory(ui)
endif()
```

The plugin's `src/CMakeLists.txt` lists the source files, links against `CCCoreLib`, `qCC_db`, `qCC_io`, `qCC_glWindow`, `CCPluginAPI`, and `Qt6::Widgets`. Nothing else.

### 11.2 CI

Add to `.github/workflows/build.yml` (all 4 jobs):

```yaml
-DPLUGIN_STANDARD_QREGISTRATION=ON
```

The plugin is enabled in the fork's `cc-configure.cmd` after the first merge.

### 11.3 Local build

The fork's `cc-configure.cmd` is updated:

```powershell
cmake -S C:\dev\CloudCompare -B C:\dev\CloudCompare\build -G Ninja --fresh `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64 `
  -DCMAKE_MAKE_PROGRAM=C:/ProgramData/chocolatey/bin/ninja.exe `
  -DPLUGIN_STANDARD_QREGISTRATION=ON
```

---

## 12. The agent-facing cross-references

For the LLM agent (or human) about to implement this:

- **The plugin recipe** — [`../../AGENTS-plugin-dev.md`](../../AGENTS-plugin-dev.md) §1.1 (Standard plugin template)
- **The existing single-window reference** — `qCC/ccPointPairRegistrationDlg.{h,cpp}` + `qCC/ui_templates/pointPairRegistrationDlg.ui`
- **The picking system details** — [`../../docs/context/registration/picking-system.md`](../../docs/context/registration/picking-system.md)
- **The dual-viewport pattern** — [`../../docs/context/registration/dual-viewport.md`](../../docs/context/registration/dual-viewport.md)
- **The transform math details** — [`../../docs/context/registration/transform-math.md`](../../docs/context/registration/transform-math.md)
- **The data flow** — [`../../docs/context/registration/data-flow.md`](../../docs/context/registration/data-flow.md)
- **The architecture** — [`../../AGENTS-architecture.md`](../../AGENTS-architecture.md) §3 (plugin model), §4 (`ccHObject`)
- **The library ownership** — [`../../AGENTS-libs.md`](../../AGENTS-libs.md)
- **The UI patterns** — [`../../AGENTS-ui.md`](../../AGENTS-ui.md) §4 (`ccOverlayDialog`), §5 (db-tree)
- **The coding standards** — [`../../AGENTS-coding-standards.md`](../../AGENTS-coding-standards.md) (file headers, naming, clang-format)
- **The goal-level milestones** — [`../../AGENTS_REGISTRATION.md`](../../AGENTS_REGISTRATION.md) §5 (M1-M8)

---

## 13. Pointers

- **The features** — [`02-features.md`](02-features.md)
- **The workflows** — [`03-user-workflows.md`](03-user-workflows.md)
- **The screens** — [`04-ui-pages.md`](04-ui-pages.md)
- **The UX patterns** — [`05-ux-patterns.md`](05-ux-patterns.md)
- **The roadmap** — [`07-roadmap.md`](07-roadmap.md)
