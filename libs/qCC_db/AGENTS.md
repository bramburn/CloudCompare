# libs/qCC_db/AGENTS.md

The 3D scene-graph library. Owner of `ccHObject` and its ~50 subclasses (`ccPointCloud`, `ccMesh`, `ccPolyline`, `ccSphere`, `ccPlane`, `ccFacet`, …).

**CMake target:** `QCC_DB_LIB` (SHARED, LGPL).
**Public headers:** `include/`.
**Vendored under it:** `extern/CCCoreLib` (algorithms), `extern/cgaltest` (CGAL, optional).

Read [`../../AGENTS-libs.md`](../../AGENTS-libs.md) §2 for the public surface; [`../../AGENTS-architecture.md`](../../AGENTS-architecture.md) §4 for the `ccHObject` model.

## Folder layout

```
qCC_db/
├── CMakeLists.txt
├── include/                       # public headers (the API)
├── src/                           # implementations
├── extern/
│   ├── CCCoreLib/                 # submodule — pure algorithms
│   └── cgaltest/                  # optional CGAL bits
├── cmake/                         # install + CGAL glue
└── doc/                           # internal notes
```

## Key public headers (must-know subset)

| Header | Why you care |
|---|---|
| `ccHObject.h` | The scene-graph root — every entity inherits from it |
| `ccObject.h` | Base of `ccHObject` (flags, serialization, name, unique ID) |
| `ccHObjectCaster.h` | Safe casts (`ToPointCloud`, `ToMesh`, …) |
| `ccBasicTypes.h` | `CC_TYPES` enum + bit-flag class IDs |
| `ccPointCloud.h`, `ccGenericPointCloud.h` | Point-cloud entities |
| `ccMesh.h`, `ccGenericMesh.h`, `ccSubMesh.h` | Mesh entities |
| `ccPolyline.h`, `ccCircle.h` | Polylines |
| `ccSphere.h`, `ccPlane.h`, `ccCylinder.h`, `ccCone.h`, `ccBox.h`, `ccDisc.h`, `ccDish.h`, `ccExtru.h`, `ccQuadric.h`, `ccTorus.h` | Geometric primitives |
| `ccFacet.h` | Composite (cloud + 2.5D mesh + polyline) |
| `cc2DLabel.h`, `cc2DViewportLabel.h`, `cc2DViewportObject.h` | 2D annotations |
| `ccImage.h` | Raster image (attached to a cloud) |
| `ccSensor.h`, `ccCameraSensor.h`, `ccGBLSensor.h` | Sensors |
| `ccColorScale.h`, `ccColorScalesManager.h`, `ccColorTypes.h` | Color scales |
| `ccGLUtils.h`, `ccColorRampShader.h` | GL drawing helpers |
| `ccExternalFactory.h`, `ccExternalHObject.h` | Plugin-defined custom types (serialize through `.bin`) |
| `ccProgressDialog.h`, `ccLog.h`, `ccConsole.h` | Logging + progress UI |

## Don't

- Don't add Qt UI in this lib (`qCC_db` is a model lib, not UI). Use `qCC/` or a plugin for dialogs.
- Don't reach into `extern/CCCoreLib` and add stuff there — it has its own `CONTRIBUTING.md` / `AGENTS.md` and is a submodule.
- Don't `dynamic_cast` on `ccHObject` — use `ccHObjectCaster` or `isA(CC_TYPES::…)`.

## See also

- Root [`../../AGENTS.md`](../../AGENTS.md)
- [`../../AGENTS-architecture.md`](../../AGENTS-architecture.md)
- [`../../AGENTS-libs.md`](../../AGENTS-libs.md)
- [`../../AGENTS-coding-standards.md`](../../AGENTS-coding-standards.md)
