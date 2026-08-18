---
title: Data model
sidebar_label: Data model
sidebar_position: 3
---

# Data model

The in-memory data model is `ccHObject` and its subclasses. Almost
everything in the db-tree is an `ccHObject`. This page is the map.

## The class tree

```
ccHObject                         (the root — every entity inherits)
├── ccShiftedObject               (has a global 3D shift for float-precision)
│   ├── ccPointCloud              (the workhorse — a list of points + scalar fields)
│   │   └── ccMesh                (adds triangles on top of a point cloud)
│   ├── ccPolyline               (a connected 2D-or-3D polyline)
│   ├── ccFacet                  (a planar facet with polygon + contour + uncertainty)
│   ├── ccOctree                 (octree structure, can be visualised)
│   └── ccSensor                 (a TLS scan position, with calibration)
├── ccHObjectContainer           (a folder / grouping — has children, no geometry)
├── ccMaterial                   (texture / color)
├── ccCalibratedImage            (a camera image with pose)
├── cc2DViewportObject           (an overlay for the 2D viewer)
└── ccGenericPrimitive           (sphere / box / plane / cone / cylinder, parametric)
```

## The `ccHObject` API surface

The interesting methods:

| Method | Purpose |
|---|---|
| `getName()`, `setName()` | The display name. Inherited by all. |
| `getDisplayAccuracy()`, `setDisplayAccuracy()` | Per-entity point size for 3D display. |
| `applyGLTransformation(const ccGLMatrix&)` | Apply a 4x4 transform to the entity. Lazy — recomputes the bounding box on demand. |
| `getOwnBB(bool withGL)` | The entity's axis-aligned bounding box. |
| `addChild(ccHObject*)` / `removeChild(ccHObject*)` | Tree management. |
| `getParent()` | Walk up the tree. |
| `isVisible()` / `setVisible(bool)` | Visibility in the viewport. |
| `isLocked()` / `setLocked(bool)` | Whether the user can edit the entity. |
| `getUserData(int key)` / `setUserData(...)` | A small key-value store for plugin-specific data. |
| `hasOneOfFlag(CC_CLASS_ENUM)` | Check the entity's class (cloud, mesh, polyline, …). |
| `getClassID()` | The runtime class ID, used by the serialization layer. |
| `accept(ccGenericPointCloud*)` | The visitor-pattern hook used by filters and the iterator. |

## Scalar fields and the cloud

A `ccPointCloud` owns its points in a single contiguous array
(`Points->data()`). Each point can have zero or more **scalar fields**
(`ccScalarField`); the cloud has a list of them, and exactly one is the
"current" displayed scalar field. This is how M3C2 distance, CSF
classification, and the octree level are visualised.

The cloud also owns per-point metadata: normals, RGB color, and a
2.5D Delaunay structure (used by the 2.5D triangulation tool).

## The lazy-evaluation pattern

Heavy structures are computed **on demand**, never eagerly:

- The bounding box is cached and invalidated on transform changes.
- The octree is built the first time it's queried and cached.
- The 2.5D Delaunay structure is built the first time it's used.

The `ccShiftedObject` wrapper hides a large global translation under
the hood (a `ccPointCloud` with a billion points uses two 32-bit floats,
not two 64-bit doubles, plus a single 64-bit global shift). This is
why dense TLS scans can fit in 2&nbsp;GB of memory.

## The container

`ccHObjectContainer` is a non-geometric folder. The db-tree uses
`ccHObjectContainer` instances as folders; clicking the disclosure
triangle in the db-tree shows the children. The `MainWindow` creates
a top-level container per session; every entity loaded from a file
goes into it.

## What to read next

- [Plugin system](/docs/architecture/plugin-system) — how plugins see and manipulate the
  data model.
- [UI](/docs/architecture/ui) — how the db-tree binds to the data model.
