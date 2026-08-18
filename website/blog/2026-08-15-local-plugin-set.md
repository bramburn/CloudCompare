---
slug: local-plugin-set
title: "Local plugin set: 18 enabled, 20+ opt-in"
authors: [icelabz]
tags: [plugins, surveying]
---

The fork's local build enables a self-contained set of 18 plugins — every
plugin that doesn't need an external dependency beyond Qt 6 and the
standard system libraries. The other 20+ plugins are off by default and
need vcpkg to enable.

{/* truncate */}

## The 18 self-contained plugins

| Type | Plugin | What it does |
|---|---|---|
| I/O | `qCoreIO` | OBJ, PLY, BIN, ASCII, E57 (no Xerces), PTX, PV, VTK, FBX (text) |
| GL | `qEDL` | Eye Dome Lighting — screen-space technique for dense clouds |
| GL | `qSSAO` | Screen-Space Ambient Occlusion — contact-shadow post-filter |
| Standard | `qAnimation` | Camera-path animation / flythrough |
| Standard | `qBroom` | Interactive cropping tool |
| Standard | `qCSF` | Cloth Simulation Filtering — ground / non-ground segmentation |
| Standard | `qM3C2` | M3C2 multiscale cloud-to-cloud distance |
| Standard | `qPoissonRecon` | Surface reconstruction (Kazhdan & Hoppe) |
| Standard | `qRANSAC_SD` | Plane / sphere / cone / cylinder detection |
| Standard | `qSRA` | Surface Roughness Analysis |
| Standard | `qHPR` | Hidden Point Removal |
| Standard | `qPCV` | Principal Component View |
| Standard | `qColorimetricSegmenter` | Color-based segmentation |
| Standard | `qMPlane` | Manual plane definition |
| Standard | `qVoxFall` | Voxel fall direction analysis |
| Standard | `qCompass` | Structural geology compass / strike-dip tool |
| Standard | `qCanupo` | CANUPO classifier |
| Standard | `3DFin` | 3D fish / forestry biometrics |

These cover the bulk of a terrestrial-laser-scanning surveying workflow:
load (`qCoreIO` + the as-yet-not-enabled `qLASIO`), classify ground (`qCSF`),
compare scans (`qM3C2`), extract primitives (`qRANSAC_SD`), surface
(`qPoissonRecon`), and visualise the result (`qEDL`).

## The priority list to enable more

The next five to turn on, in order, for a surveying-company build:

1. **`qLASIO`** (LASzip) — LAS/LAZ is the dominant delivery format.
2. **`qE57IO`** (Xerces-C++) — Leica RTC360, Faro scanners.
3. **`qDracoIO`** (Draco) — Google Draco compressed mesh.
4. **`qPDALIO`** (PDAL) — LAS + DEMs (alternative to `qLASIO`).
5. **`qPCL`** (PCL) — ICP registration, filters, segmentation. The
   heavyweight; vcpkg build is ~30-60 minutes.

The full list is in the
[Disabled priority](/docs/plugins/disabled-priority) page, with the
exact `vcpkg install <pkg>:x64-windows` recipe for each.

## vcpkg recipe

```powershell
git clone https://github.com/microsoft/vcpkg C:\dev\vcpkg
C:\dev\vcpkg\bootstrap-vcpkg.bat
C:\dev\vcpkg\vcpkg integrate install

C:\dev\vcpkg\vcpkg install laszip:x64-windows
C:\dev\vcpkg\vcpkg install xerces-c:x64-windows
C:\dev\vcpkg\vcpkg install draco:x64-windows
C:\dev\vcpkg\vcpkg install pdal:x64-windows
C:\dev\vcpkg\vcpkg install pcl:x64-windows     # slow
```

Then add the vcpkg prefix and the plugin flag to
`C:\dev\tools\cc-configure.cmd`:

```bat
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64;C:/dev/vcpkg/installed/x64-windows ^
  -DPLUGIN_IO_QLAS=ON ^
  -DPLUGIN_IO_QE57=ON
```

Re-configure **with `--fresh`** (this matters — see
[Troubleshooting](/docs/build/troubleshooting)) and rebuild.
