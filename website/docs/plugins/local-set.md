---
title: Local set
sidebar_label: Local set
sidebar_position: 2
---

# Local set

The fork ships with **16 self-contained plugins** — no external
dependencies beyond Qt 6 and the standard system libraries. This is the
default set in the wrapper scripts.

:::note
`qM3C2` and `qCSF` are explicitly enabled in `tools/cc-configure.cmd`
(`-DPLUGIN_STANDARD_QM3C2=ON -DPLUGIN_STANDARD_QCSF=ON`) as the
T1 test targets. All others default to `OFF` in their `CMakeLists.txt`.
:::

| Plugin | Type | What it does |
|---|---|---|
| `qCoreIO` | I/O | OBJ, PLY, BIN, ASCII, E57 (no Xerces), PTX, PV, VTK, FBX (text) |
| `qEDL` | GL | Eye Dome Lighting — a screen-space technique that makes dense point clouds readable without per-point normals |
| `qSSAO` | GL | Screen-Space Ambient Occlusion — contact-shadow post-filter for meshes |
| `qAnimation` | Standard | Camera-path animation / flythrough |
| `qBroom` | Standard | Interactive cropping tool ("the broom") |
| `qCSF` | Standard | Cloth Simulation Filtering — ground / non-ground segmentation (T1 test target) |
| `qM3C2` | Standard | M3C2 multiscale cloud-to-cloud distance (T1 test target) |
| `qPoissonRecon` | Standard | Surface reconstruction from oriented points (Kazhdan & Hoppe) |
| `qHPR` | Standard | Hidden Point Removal — point-cloud visibility from a viewpoint |
| `qPCV` | Standard | Principal Component View — turntable spin image |
| `qColorimetricSegmenter` | Standard | Color-based segmentation (HSV / Lab) |
| `qMPlane` | Standard | Manual plane definition |
| `qVoxFall` | Standard | Voxel fall direction analysis |
| `qCanupo` | Standard | CANUPO classifier (Brodu & Lague) |
| `3DFin` | Standard | 3D fish morphology / forestry biometrics |
| `qTreeIso` | Standard | Tree isolation from point clouds |

## Experimental plugins

`plugins/experimental/` holds plugins under active development that are
not yet ready for the default build:

| Plugin | Status | What it does |
|---|---|---|
| `qHelloCloud` | Smoke test | Minimal Standard plugin scaffold — validates the plugin loading code path end-to-end. Used to verify namespace collisions before enabling a new plugin. |

## Verifying the local set on your build

Run the binary and open **About > Plugins**. The list should match this
table. Anything missing means a plugin was disabled at configure time or
its `.dll` failed to load (usually a missing dependency on a non-self-
contained plugin — but all 18 above are self-contained).

You can also check the build output:

```bash
# Linux
ls build/plugins/*/libq*.so
# Windows
Get-ChildItem build\plugins\*\Release\q*.dll
# macOS (community-supported, not CI-validated)
ls build/qCC/CloudCompare.app/Contents/PlugIns/
```

## Why this set, not the upstream's all-on set

The upstream default enables every plugin whose dependencies are
auto-detectable. That includes the heavyweight ones (PCL, PDAL, FBX SDK,
OpenCASCADE, Xerces-C++). For a surveying-company build that targets
mobile and terrestrial laser scans, those plugins are *useful* but not
*required* — and they add 1-2&nbsp;hours to a clean build plus 5-10&nbsp;GB
of vcpkg install footprint.

Three plugins from the upstream default are **Qt 6.8.3 incompatible**
and are excluded regardless of dependencies:
`qCompass` (class-name collision in `qvectornd.h`),
`qRANSAC_SD`, and `qSRA`. They will return when the Qt 6 upstream
migration is complete.

The fork's default is the set you can build in 15 minutes on a clean
checkout with no extra dependencies. The disabled plugins are *opt-in*;
see [Disabled priority](/docs/plugins/disabled-priority) for the recipe.
