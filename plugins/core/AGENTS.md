# plugins/core/AGENTS.md

Shipped plugins (the ones enabled by CI and by `cc-configure.cmd`).

> Read this **in addition to** the root [`AGENTS.md`](../AGENTS.md). The
> root file has the build commands, plugin-development workflow, and
> repo conventions. This file adds the **shipped-plugin**-specific
> rules.

## Hard rule: do not add new plugins here directly

New plugin work goes to
[`../experimental/AGENTS.md`](../experimental/AGENTS.md) first. A
plugin only gets moved (copied) into `plugins/core/` once the user
has explicitly graduated it. If you (the agent) are creating a
brand-new `q<Name>/` folder under `Standard/`, `IO/`, or `GL/`,
**stop** — that belongs in `plugins/experimental/q<Name>/`.

Bug fixes, new menu actions on existing plugins, and small
refactors stay here. Everything else graduates via the experimental
gate. See [`../experimental/AGENTS.md`](../experimental/AGENTS.md) for
the full workflow.

## Layout

```
plugins/core/
├── CMakeLists.txt
├── IO/                # file-format I/O plugins
├── Standard/          # analysis tools / menu actions
└── GL/                # OpenGL post-filters
```

Each subdirectory has its own `CMakeLists.txt` enumerating the plugins. To add a new shipped plugin, drop it under one of these three and `add_subdirectory()` it in the parent.

## The shipped set (canonical, from CI matrix in `.github/workflows/build.yml`)

### GL

| Plugin | CMake flag | Dep | What it does |
|---|---|---|---|
| `qEDL` | `PLUGIN_GL_QEDL` | none | Eye-Dome Lighting shader |
| `qSSAO` | `PLUGIN_GL_QSSAO` | none | Screen-Space Ambient Occlusion shader |

### IO (file formats)

| Plugin | CMake flag | Dep | Format |
|---|---|---|---|
| `qCoreIO` | `PLUGIN_IO_QCORE` | none (default ON) | BIN, ASCII, PLY |
| `qAdditionalIO` | `PLUGIN_IO_QADDITIONAL` | various | Bin2, Cloud, DepthMap, DXF, SHP, … |
| `qCSVMatrixIO` | `PLUGIN_IO_QCSVMATRIX` | none | CSV matrices |
| `qDracoIO` | `PLUGIN_IO_QDRACO` | Draco | Google Draco compressed .drc |
| `qE57IO` | `PLUGIN_IO_QE57` | Xerces-C++ | ASTM E57 |
| `qFBXIO` | `PLUGIN_IO_QFBX` | FBX SDK | Autodesk FBX |
| `qLASIO` | `PLUGIN_IO_QLAS` | LASzip | LAS / LAZ (LiDAR) |
| `qLASFWFIO` | `PLUGIN_IO_QLASFWF` | LASLib | Old LAS reader (deprecated) |
| `qPDALIO` | `PLUGIN_IO_QPDAL` | PDAL | LAS + DEMs |
| `qPhotoscanIO` | `PLUGIN_IO_QPHOTOSCAN` | zlib + quazip | Agisoft Photoscan |
| `qRDBIO` | `PLUGIN_IO_QRDB` | RIEGL SDK | Riegl RDB |
| `qStepCADImport` | `PLUGIN_IO_QSTEP` | OpenCASCADE | STEP CAD |

### Standard (analysis tools)

| Plugin | CMake flag | Dep | What it does |
|---|---|---|---|
| `q3DMASC` | `PLUGIN_STANDARD_Q3DMASC` | OpenCV | Multi-scale shape/appearance segmentation |
| `3DFin` (cc3DFin) | `PLUGIN_STANDARD_3DFIN` | none | Individual tree detection |
| `qAnimation` | `PLUGIN_STANDARD_QANIMATION` | ffmpeg (opt) | Camera animation exporter |
| `qBroom` | `PLUGIN_STANDARD_QBROOM` | none | Point cloud cleaning tool |
| `qCanupo` | `PLUGIN_STANDARD_QCANUPO` | none | CANUPO classifier |
| `qCloudLayers` | `PLUGIN_STANDARD_QCLOUDLAYERS` | none | Cloud layers |
| `qColorimetricSegmenter` | `PLUGIN_STANDARD_QCOLORIMETRIC_SEGMENTER` | none | Colorimetric segmentation |
| `qCompass` | `PLUGIN_STANDARD_QCOMPASS` | none | Compass strike/dip measurement |
| `qCSF` | `PLUGIN_STANDARD_QCSF` | none | Cloth Simulation Filter (ground filtering) |
| `qCork` | `PLUGIN_STANDARD_QCORK` | Cork + MPIR | Mesh CSG (alt) |
| `qFacets` | `PLUGIN_STANDARD_QFACETS` | ShapeLib | Facet extraction |
| `qG3Point` | `PLUGIN_STANDARD_QG3POINT` | Open3D | Geological point analysis |
| `qHoughNormals` | `PLUGIN_STANDARD_QHOUGH_NORMALS` | Eigen | Hough normals |
| `qHPR` | `PLUGIN_STANDARD_QHPR` | qhull (vendored) | Hidden Point Removal (visibility) |
| `qJSonRPCPlugin` | `PLUGIN_STANDARD_QJSONRPC` | Qt6 Network+WebSockets | JSON-RPC server |
| `qM3C2` | `PLUGIN_STANDARD_QM3C2` | none | Multiscale Model-to-Model Cloud Comparison |
| `qMasonry` | `PLUGIN_STANDARD_QMASONRY` | none | Masonry analysis (qAutoSeg / qManualSeg) |
| `qMeshBoolean` | `PLUGIN_STANDARD_QMESHBOOLEAN` | libigl + Eigen + CGAL | Mesh boolean ops |
| `qMPlane` | `PLUGIN_STANDARD_QMPLANE` | none | MPlane plugin |
| `qPCL` | `PLUGIN_STANDARD_QPCL` | PCL | **Big one** — ICP, registration, filters, segmentation |
| `qPCV` | `PLUGIN_STANDARD_QPCV` | none | Point Cloud Virtual Scanner |
| `qPoissonRecon` | `PLUGIN_STANDARD_QPOISSON_RECON` | none | Poisson surface reconstruction |
| `qRANSAC_SD` | `PLUGIN_STANDARD_QRANSAC_SD` | none | RANSAC shape detection (planes, spheres, cylinders) |
| `qSRA` | `PLUGIN_STANDARD_QSRA` | none | Surface of Revolution Analysis |
| `qTreeIso` | `PLUGIN_STANDARD_QTREEISO` | Eigen3 (opt) | Tree isolation |
| `qVoxFall` | `PLUGIN_STANDARD_QVOXFALL` | none | VoxFall plugin |

> **Naming note:** the option-flag convention is `PLUGIN_<TYPE>_<NAME>` where `<NAME>` is the directory name in **uppercase** with the leading `q` dropped and the rest uppercased. E.g. `qHPR` → `QHPR`, `qCoreIO` → `QCORE`, `qJSonRPCPlugin` → `QJSONRPC`. **Exceptions** are `3DFin` (stays `3DFIN`) and a few historical oddities (`qMPlane` → `QMPLANE`, not `QM_PLANE`); match what you find in the plugin's `CMakeLists.txt`.

## Default-on vs default-off

Default **ON**: only `qCoreIO` (the BIN/ASCII/PLY filter) — everything else is `OFF`. CI turns on the no-external-dep plugins explicitly; external-dep plugins are gated behind `-D<flag>=ON` plus whatever `find_package` they need.

## Editing a shipped plugin

1. Edit the plugin folder in place.
2. If you added a new source file, append it to `src/CMakeLists.txt` and `include/CMakeLists.txt`.
3. **Run `cmake --build build --target check-format`** before commit.
4. **Update the CI matrix** if you change the default-on/off of the plugin (in `.github/workflows/build.yml`, the single Windows MSVC job — macOS and Linux are not in the matrix).

## See also

- [`../../AGENTS-plugin-dev.md`](../../AGENTS-plugin-dev.md) — full add/edit/remove recipe
- [`../example/AGENTS.md`](../example/AGENTS.md) — copy-paste templates
- [`../AGENTS.md`](../AGENTS.md) — plugin contract
