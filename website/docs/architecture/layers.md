---
title: Layer ownership
sidebar_label: Layers
sidebar_position: 2
---

# Layer ownership

A reference for **what each library owns, what it exposes, and who links
to it**. When you're about to touch a file, the table below tells you
which other libraries might be affected.

## `CCCoreLib` (`libs/CCCoreLib/`)

**Owns:** pure-C++ math, geometry, data structures, and algorithms.
Octree, KD-tree, distance, ICP, normals, RANSAC, Delaunay 2.5D,
Poisson reconstruction primitives, scalar fields, geometry structures
(plane, sphere, cone, cylinder, …).

**Exposes:** `CCCoreLib.h` (the umbrella header) and the per-feature
headers under `include/`.

**Linked by:** `qCC_db`, `qCC_io`, `qCC_glWindow`, `CCPluginAPI`, `qCC`,
`ccViewer`, **all** plugins.

**Key constraint:** must remain Qt-free and OpenGL-free. Anything
that's `QObject`-aware or `QOpenGLFunctions`-aware does not belong
here.

## `qCC_db` (`libs/qCC_db/`)

**Owns:** the in-memory data model — `ccHObject` and its subclasses
(`ccPointCloud`, `ccMesh`, `ccPolyline`, `ccFacet`, `ccCalibratedImage`,
…). Also owns `ccGenericMesh`, `ccDrawable`, `ccMaterial` and the
serialization layer (BIN file format).

**Exposes:** `ccHObject.h`, `ccPointCloud.h`, `ccMesh.h`, …

**Linked by:** `qCC_io`, `qCC_glWindow`, `qCC`, `ccViewer`, all plugins.

## `qCC_io` (`libs/qCC_io/`)

**Owns:** the legacy built-in file I/O filters that ship with
CloudCompare itself (not the plugin ones) — BIN, OBJ, PLY, ASCII, PTX,
PV, VTK, FBX-text, E57-legacy. New formats belong in
`plugins/core/IO/`, not here.

**Exposes:** the `FileIOFilter` base class (in `CCPluginAPI`) and the
concrete filter classes.

**Linked by:** `qCC`, all I/O plugins.

## `qCC_glWindow` (`libs/qCC_glWindow/`)

**Owns:** the 3D viewport widget (`ccGLWindow`), the camera, picking,
the axis gizmo, the LOD culling, the EDL/SSAO hook points, and the
Qt + OpenGL glue. This is the only place in the project that
simultaneously uses Qt and OpenGL.

**Exposes:** `ccGLWindow.h`, the `ccGenericCamera` API.

**Linked by:** `qCC`, `ccViewer`, GL plugins.

## `CCAppCommon` (`libs/CCAppCommon/`)

**Owns:** cross-app helpers — the 3DConnexion space-mouse driver, the
recent-files menu, the plugin stub, application-level QSettings
helpers, the `qCC_io` initialization shims.

**Exposes:** the `ccPlugin` stub class (for non-plugin use of the
filter API), the `ccApp` namespace.

**Linked by:** `qCC`, `ccViewer`. **Not** linked by plugins — plugins
use `CCPluginAPI` instead.

## `CCPluginAPI` (`libs/CCPluginAPI/`)

**Owns:** the **interface** that every plugin implements.
`ccPluginInterface`, `IStandardPlugin`, `FileIOFilter`, `IGLPlugin`,
`ccMainAppInterface`. The header-only side of the contract.

**Exposes:** headers in `include/`.

**Linked by:** `qCC`, `ccViewer`, **all** plugins.

**Note:** the `.cpp` side of `CCPluginAPI` is tiny — most of the
implementation lives in the host (`qCC/`'s `ccPluginManager`).

## `CCPluginStub` (`libs/CCPluginStub/`)

**Owns:** a stub library that links a small subset of the plugin
interfaces, useful for testing the plugin-loading code path without
shipping a full plugin.

**Linked by:** the unit tests, and the upstream CI.

## `CCFbo` (`libs/CCFbo/`)

**Owns:** the framebuffer-object helpers used by `qCC_glWindow` and the
GL plugins. Thin wrapper around `QOpenGLFramebufferObject` with the
multi-target / multi-sampler plumbing the viewport needs.

**Linked by:** `qCC_glWindow`, GL plugins.
