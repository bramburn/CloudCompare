---
title: Disabled priority
sidebar_label: Disabled priority
sidebar_position: 3
---

# Disabled priority

The plugins below are **off by default** in the fork. Each needs an
external dependency. The table is in the order a surveying-company build
should re-enable them, based on how often each plugin shows up in
deliverables.

## Priority list

| Plugin | Needs | Why you'd enable it |
|---|---|---|
| `qLASIO` | LASzip | **#1 priority** — LAS/LAZ (LiDAR). The single most useful disabled plugin for surveying. |
| `qE57IO` | Xerces-C++ | E57 format. Required if your scanner exports E57 (Leica RTC360, Faro). |
| `qPDALIO` | PDAL | LAS + DEMs. Compete with `qLASIO`; pick one. |
| `qPCL` | PCL | **Big one** — ICP registration, filters, segmentation. ~30-60 min vcpkg build. |
| `qDracoIO` | Draco | Compressed Google Draco mesh format. |
| `qPhotoscanIO` | zlib + quazip | Agisoft Photoscan project files. |
| `qFBXIO` | FBX SDK | Autodesk FBX. |
| `qStepCADImport` | OpenCASCADE | STEP CAD files (BIM, mechanical). |
| `qMeshBoolean` | libigl + Eigen + CGAL | Mesh boolean operations. |
| `qHoughNormals` | Eigen | Hough-transform normals (heavy clouds). |
| `qFacets` | ShapeLib | Facet extraction from a mesh. |
| `qCork` | Cork + MPIR | Mesh CSG. |
| `qMasonry` | (none extra) | qAutoSeg / qManualSeg — verify before re-enabling. |
| `q3DMASC` | OpenCV | Multi-scale cloud segmentation. |
| `qG3Point` | Open3D | Geological analysis. |
| `qJSonRPCPlugin` | Qt6 Network+WebSockets | JSON-RPC server. |
| `qLASFWFIO` | LASLib | Old LAS reader. Prefer `qLASIO`. |
| `qRDBIO` | — | Riegl RDB. |
| `qAdditionalIO` | various | Extra formats (GTS, DP, SOCET, etc.). |
| `qCSVMatrixIO` | — | CSV matrix. |
| `qTreeIso` | Eigen3 (optional) | Tree isolation. Works as-is. |

## The vcpkg recipe

```powershell
git clone https://github.com/microsoft/vcpkg C:\dev\vcpkg
C:\dev\vcpkg\bootstrap-vcpkg.bat
C:\dev\vcpkg\vcpkg integrate install

# Surveying priorities (in order)
C:\dev\vcpkg\vcpkg install laszip:x64-windows     # → qLASIO
C:\dev\vcpkg\vcpkg install xerces-c:x64-windows   # → qE57IO
C:\dev\vcpkg\vcpkg install draco:x64-windows      # → qDracoIO
C:\dev\vcpkg\vcpkg install pdal:x64-windows       # → qPDALIO
C:\dev\vcpkg\vcpkg install pcl:x64-windows        # → qPCL (~30-60 min build)
```

## Wiring it into the build

Once the dependency is installed, edit `C:\dev\tools\cc-configure.cmd`
and add the vcpkg prefix path and the plugin flag:

```bat
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64;C:/dev/vcpkg/installed/x64-windows ^
  -DPLUGIN_IO_QLAS=ON ^
  -DPLUGIN_IO_QE57=ON
```

Then re-configure **with `--fresh`** (this matters — see
[Troubleshooting / Plugin find_package failed](/docs/build/troubleshooting))
and rebuild.

## Compile-time flag naming

The flag name is always `PLUGIN_<CATEGORY>_<NAME>` where:

- `<CATEGORY>` is `IO`, `STANDARD`, or `GL`
- `<NAME>` is the plugin's prefix in **uppercase**, with non-alphanumeric
  characters dropped (e.g. `3DFin` → `PLUGIN_STANDARD_3DFIN`).

The exact name is on the first line of each plugin's `CMakeLists.txt`.
