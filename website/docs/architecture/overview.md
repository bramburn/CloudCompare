---
title: Architecture overview
sidebar_label: Overview
sidebar_position: 1
---

# Architecture overview

CloudCompare is built as a stack of independent libraries, a host app, and
a plugin folder. The split is deliberate: a new feature should almost
always be a plugin, and a change to a library is a big deal.

## The stack

```
+----------------------------------------------------------+
|                          qCC                            |
|  (full GUI: db-tree, 3D view, console, plugin menu,      |
|   command-line parser)                                   |
+----------------------------------------------------------+
|                          ccViewer                       |
|  (read-only viewer with its own plugin path)             |
+----------------------------------------------------------+
|                       plugins/*                          |
|  (Standard / I/O / GL shared libraries, loaded           |
|   dynamically)                                           |
+----------------------------------------------------------+
|                        CCPluginAPI                       |
|  (the contract: ccPluginInterface, IStandardPlugin,       |
|   FileIOFilter, IGLPlugin, …)                            |
+----------------------------------------------------------+
|                        CCAppCommon                       |
|  (3DConnexion driver, recent-files, plugin stub, …)      |
+----------------------------------------------------------+
|       qCC_db            qCC_io         qCC_glWindow      |
|  (ccHObject and     (file format     (3D viewport:       |
|   subclasses)        readers /        Qt + OpenGL)       |
|                      writers)                            |
+----------------------------------------------------------+
|                          CCCoreLib                       |
|  (octree, distance, ICP, normals, RANSAC, … — pure       |
|   math + data structures, no Qt, no OpenGL)              |
+----------------------------------------------------------+
|       CCFbo        (FBO helpers used by qCC_glWindow      |
|                    and the GL plugins)                   |
+----------------------------------------------------------+
```

The arrows point down the dependency chain: `qCC` links to everything;
`CCCoreLib` links to nothing.

## The rules

1. **`CCCoreLib` knows nothing about Qt or OpenGL.** This is the
   invariant that lets the math layer be unit-tested, reused, and
   swapped.
2. **`qCC_db` knows nothing about the host app.** It defines
   `ccHObject` and the cloud/mesh/scalar-field classes, but it doesn't
   know whether the host is `qCC`, `ccViewer`, a script, or a test
   harness.
3. **`qCC_io` knows nothing about `qCC`.** It produces `ccHObject`
   trees from disk; the host wires them into the db-tree.
4. **`qCC_glWindow` is the only place that does Qt + OpenGL.** Every
   viewport feature (trackball camera, picking, axis gizmo, EDL/SSAO
   hooks) lives here.
5. **Plugins go in `plugins/`, not in `libs/`.** Adding a new feature
   as a plugin keeps the core libraries stable and the build fast.

## What to read first

If you only have time to read three source files, read these:

- [`CCCoreLib/CCCoreLib.h`](https://github.com/bramburn/CloudCompare/blob/master/libs/CCCoreLib/include/CCCoreLib.h)
  — the public API of the math layer.
- [`qCC_db/ccHObject.h`](https://github.com/bramburn/CloudCompare/blob/master/libs/qCC_db/include/ccHObject.h)
  — the in-memory data model.
- [`CCPluginAPI/include/ccPluginManager.h`](https://github.com/bramburn/CloudCompare/blob/master/libs/CCPluginAPI/include/ccPluginManager.h)
  — the plugin loading sequence.

If you have time for one more, read
[`qCC/MainWindow.cpp`](https://github.com/bramburn/CloudCompare/blob/master/qCC/mainwindow.cpp)
— it's a 5000-line god-class but it shows you every event the host
emits and every UI element that subscribes to it.
