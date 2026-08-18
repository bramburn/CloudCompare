---
title: File formats deep-dive
sidebar_label: Formats
sidebar_position: 7
description: Per-format reference for the I/O plugins — when to use which, what's preserved, what to watch for.
---

# File formats deep-dive

The [I/O plugin page](/docs/plugins/io) lists what the fork reads and
writes. This page is the per-format reference: when to use which format,
what's preserved on round-trip, and the gotchas that bite in production.

For task-shaped guidance on opening, saving, and converting files, see
the [cookbook](/docs/cookbook).

## Quick reference

| Format | Read | Write | Best for | Watch out for |
|---|---|---|---|---|
| `.las` / `.laz` | ✓ | ✓ | LiDAR, interchange, archival | Version, classification codes, VLRs |
| `.e57` | ✓ | ✓ | Multi-scan terrestrial + image | Tree structure, scan vs image |
| `.ply` | ✓ | ✓ | Mesh + cloud, research, custom props | ASCII vs binary, 32-bit overflow |
| `.obj` | ✗ | ✓ | Mesh export to CAD / DCC | No colours, no scalars, no scale |
| `.fbx` | ✗ | ✓ | Mesh to Unity / Unreal | Disabled by default (vcpkg) |
| `.bin` | ✓ | ✓ | CloudCompare working session | **Not an interchange format** |
| `.dxf` | ✓ | ✓ | Polylines, simple geometry, CAD | Round-trip loss of attributes |
| `.stl` | ✗ | ✓ | Mesh to 3D printing | ASCII / binary, no colour |
| `.vtk` | ✓ | ✓ | Legacy, ParaView interop | Schema mismatch |
| `.shp` | ✓ | ✓ | GIS polyline export | 2D only, projection needed |
| `.csv` / `.txt` | ✓ | ✓ | One-off export to spreadsheet | No standard schema |
| `.asc` | ✓ | ✓ | ESRI ASCII grid (raster DEM) | Float32 only |

## `.las` / `.laz` (LiDAR)

The de facto LiDAR interchange format. `.las` is uncompressed; `.laz`
is the lossless compressed variant.

**Versions:** 1.0, 1.1, 1.2, 1.3, 1.4, 1.5. The fork's
[`qCoreIO`](/docs/plugins/io) reads 1.0-1.4. For full 1.4 support
(including 64-bit coords, extra bytes, waveform), enable
[`qLASIO`](/docs/plugins/disabled-priority) (vcpkg,
LASzip).

**Preserved on round-trip:**

- Point count, point format (0-10), point record length
- X, Y, Z, intensity, return number, classification, scan angle
- GPS time (if present in the source)
- RGB colour (format 2, 3, 5, 7, 8)
- Classification codes (standard ASPRS)
- Per-VLR extra bytes (only if the read supports them — `qLASIO` does,
  `qCoreIO` may drop them)

**Watch out for:**

- **Coordinate scale mismatch.** LAS stores coordinates as integers
  scaled by `x_scale_factor` and `y_scale_factor`. If the source uses
  a scale of 0.01 and you re-save with the default 0.001, you've
  quantised to 1 cm precision. For survey-grade work, keep the
  source's scale.
- **`GLOBAL_SHIFT` on import.** Large coordinates (UTM, OSGB) need a
  global shift to fit in float32. The fork's `qCoreIO` handles this
  automatically; the CLI uses `-O -GLOBAL_SHIFT AUTO`.
- **Classification codes are the standard, not the source's local
  scheme.** The ASPRS codes (2 = ground, 6 = building, …) are
  what everyone downstream expects. Don't re-purpose them for
  project-specific classes.

## `.e57` (multi-scan + image)

The E57 format is a container that holds **multiple scans**, each
with its own transform, plus images and metadata. It is the standard
for terrestrial laser scanners (Leica, FARO, Trimble) and is what
they output when you ask for the "raw" or "complete" export.

**Tree structure:**

```
root
├── /images
│   └── /image0 (image data)
└── /scans
    ├── /scan0
    │   ├── points
    │   ├── pose (transform)
    │   ├── intensity
    │   └── color (optional)
    └── /scan1
        └── ...
```

When opened, the fork produces **one entity per scan** plus a
`/scans` folder containing them all. The transforms are applied
automatically so the scans appear in their world positions.

**Preserved on round-trip:**

- Per-scan transforms (the scanner's pose per acquisition)
- Intensity
- Colour (if present in the source)
- Cartesian / spherical / organised scan structure
- Original file metadata (instrument, date, software version)

**Watch out for:**

- **The "per-scan" entity model.** Operations like ICP happen on
  individual scans, not the whole merged cloud. Merge first
  (`Edit > Merge` after selecting the scans) if you want to operate
  on the whole site.
- **Re-saving as E57 with custom scalar fields loses the fields.**
  E57's strict schema doesn't have a generic "extra dims" slot. The
  scalar fields are converted to intensity or dropped.
- **Some scanner software uses the "unstructured" point format**;
  others use the "organised" (image-grid) format. The fork handles
  both, but the visual appearance is different — the organised form
  preserves scan-grid structure, which can be useful for colouring
  by scan-line.

## `.ply` (research / mesh)

A simple, well-documented format. Comes in three flavours:

- **ASCII** — human-readable, large files, slow to parse
- **Binary little-endian** — the default, fast, compact
- **Binary big-endian** — rare; CloudCompare reads and writes it

**Point formats:** `vertex` (x, y, z only), `vertex` with normals,
`vertex` with colour, `vertex` with custom properties.

**Preserved on round-trip:**

- Vertex positions
- Normals
- Colour (RGB)
- Custom properties (the fork reads them as scalar fields)
- Faces (for meshes)

**Watch out for:**

- **32-bit float precision is the default.** For large coordinates
  (e.g. UTM), quantisation noise can reach 1 mm. Use
  `-O -GLOBAL_SHIFT AUTO` on import and the same on export to keep
  the precision.
- **Custom properties become scalar fields on import.** Renaming a
  field on import doesn't rename the source; re-import brings it
  back.
- **ASCII PLY is ~3× larger than binary** for typical point clouds.
  Use binary unless you need to grep the file.

## `.obj` (mesh to CAD / DCC)

Wavefront OBJ. The most portable mesh format.

**What it preserves:** vertex positions, normals, UVs, face indices.
**What it does NOT preserve:** colours (no per-vertex RGB), scalar
fields, transforms, anything custom.

**Watch out for:**

- **OBJ is mesh-only.** You can't save a point cloud as OBJ; the
  fork will refuse. Convert to a mesh first (`Edit > Mesh > Delaunay
  2.5D` for topographic clouds, `qPoissonRecon` for organic surfaces).
- **No scale / transform** is stored. The cloud's coordinate system
  is preserved (the vertices are unchanged) but the **file** has
  no metadata saying "this is in metres, not feet". Tag the cloud
  before export (`Edit > Apply transformation` if needed).
- **Quads and n-gons are triangulated on import.** A `.obj` from
  Rhino with quad faces will be re-triangulated on import.

## `.fbx` (mesh to Unity / Unreal / Maya)

The Autodeskk FBX format. Disabled by default in the fork (vcpkg
build). Enable [`qFBXIO`](/docs/plugins/disabled-priority) if
you need FBX.

**What it preserves:** mesh + normals + UVs + material slots + animation
(if you have it). **What it does NOT preserve:** scalar fields,
classification codes, scan-specific transforms.

**Watch out for:**

- **Up axis is Y in FBX, Z in CloudCompare.** The fork rotates on
  import / export. If you re-import the FBX you exported, the
  transforms should round-trip.
- **The FBX SDK is large** (1 GB compiled) and slow to link. This is
  why the plugin is opt-in.

## `.bin` (CloudCompare working session)

A CloudCompare-internal format. **Not an interchange format** — no
other tool reads it. Use it for working state, not for deliverables.

**What it preserves:** everything in the DB tree. Geometry, scalar
fields, colours, normals, octree, group hierarchy, visibility,
camera angles, scalar field display parameters.

**Watch out for:**

- **It is large.** A 100M-point cloud with 5 scalar fields and
  normals is ~5 GB on disk.
- **It is opaque.** No way to read it with a text editor or another
  tool.
- **It is versioned.** A `.bin` written by CloudCompare 2.14 may
  not load in 2.13. Document the CC version in the file name
  (`session-2026-08-18-cc214.bin`).

## `.dxf` (polylines, simple geometry)

AutoCAD's interchange format. The fork reads and writes polylines,
simple meshes, and a few primitives.

**Watch out for:**

- **Round-trip loss of scalar fields.** DXF doesn't have a scalar
  field concept; per-vertex scalars are dropped on export.
- **No colour by default.** Some DXF readers ignore colour entities.
  Use `dxf_export_color` to control.
- **Scale / units are not stored.** The DXF file doesn't say
  "metres". The receiver has to know.

## `.stl` (3D printing)

A simple mesh format with no colour, no normals, no scene graph.

**Watch out for:**

- **ASCII vs binary** — most modern tools read both. The fork writes
  binary by default.
- **The "facet normal" is computed on import.** A non-manifold mesh
  (holes, self-intersections) gives wrong normals; clean up first
  with `Edit > Mesh > Repair`.

## `.vtk` (legacy / ParaView)

The Visualization Toolkit format. Useful for interop with ParaView
and some older GIS tools.

**Watch out for:**

- **Schema mismatch.** The fork writes in the older `PolyData` format;
  newer tools expect `UnstructuredGrid`. Converting is usually
  automatic but can fail on meshes with mixed cell types.

## `.shp` (GIS polylines)

The ESRI Shapefile format. Used for cross-loading into ArcGIS, QGIS,
and other GIS tools.

**Watch out for:**

- **2D only.** The fork drops Z on export. If you need 3D polylines
  in GIS, use `.dxf` instead.
- **Field schema is lost.** Per-vertex scalar fields don't survive
  the SHP export.
- **Projection file (`.prj`) is not generated.** The receiver has to
  know the coordinate system.

## `.csv` / `.txt` (one-off export)

Tabular plain text. Useful for one-off exports to a spreadsheet or a
small Python script.

**Default columns:** `x, y, z, intensity` (if present), `R, G, B`
(if colours), then one column per scalar field.

**Watch out for:**

- **No standard schema.** Different tools read the columns
  differently. Document the schema in the file name or in a sidecar
  `README.txt`.
- **Slow for large clouds.** A 100M-point CSV is ~3 GB and takes
  minutes to write.

## `.asc` (ESRI ASCII grid)

A simple raster format for DEMs.

**Watch out for:**

- **Float32 only.** For higher precision, use GeoTIFF.
- **No projection metadata.** The receiver has to know the coordinate
  system.

## When to use which

- **Working session** → `.bin` (fast round-trip, full state).
- **Archival** → `.laz` (lossless, compressed, standard).
- **CAD round-trip** → `.dxf` (polylines) or `.obj` (meshes).
- **GIS** → `.shp` (polylines) or `.asc` / GeoTIFF (DEMs).
- **3D printing** → `.stl`.
- **Game engine / DCC** → `.fbx` (enable `qFBXIO`).
- **ParaView / legacy** → `.vtk`.
- **One-off analysis** → `.csv`.
- **Always** → keep a working `.bin` of the original scan, never
  overwrite the source.
