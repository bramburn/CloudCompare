---
title: Plugin overview
sidebar_label: Overview
sidebar_position: 1
---

# Plugin overview

Almost every CloudCompare feature beyond the core 3D viewer is a **plugin**.
File formats, analysis algorithms, OpenGL post-filters — all of them are
separate shared libraries that CloudCompare discovers at startup.

The three plugin types are:

| Type | Folder | What it ships |
|---|---|---|
| **Standard** | `plugins/core/Standard/` | Analysis tools — qCSF, qM3C2, qRANSAC_SD, qPoissonRecon, qBroom, qAnimation, qCompass, qCanupo, 3DFin, qPCV, qColorimetricSegmenter, qMPlane, qVoxFall, qHPR, qSRA, … |
| **I/O** | `plugins/core/IO/` | File-format filters — qLASIO, qE57IO, qPDALIO, qFBXIO, qPhotoscanIO, qDracoIO, qStepCADImport, qRDBIO, qLASFWFIO, qCSVMatrixIO, qAdditionalIO, qTreeIso, qCoreIO |
| **GL** | `plugins/core/GL/` | OpenGL post-filters — qEDL (Eye Dome Lighting), qSSAO (Screen-Space Ambient Occlusion) |

The full source for every plugin in the upstream set lives in
`plugins/core/`. A canonical empty template for each type lives in
`plugins/example/` (Standard, IO, and GL).

## The contract

Every plugin implements the `ccPluginInterface` and exports a factory
function via the `Q_PLUGIN_METADATA` macro. The `ccPluginManager` (in
`qCC/`) loads the `.dll` / `.so` / `.dylib` on startup, queries it for
its capabilities, and registers it with the right manager:

- A Standard plugin with `IStandardPlugin` capability shows up under
  **Plugins > Standard Plugins**.
- An I/O plugin with `IFileIOFilter` capability shows up under
  **File > Open** / **File > Save** as a new format option.
- A GL plugin with `IGLPlugin` capability shows up in the 3D viewport's
  filter menu.

For the full contract and the API surface, see
[Architecture / Plugin system](/docs/architecture/plugin-system).

## The local set

The fork ships with **18 self-contained plugins enabled** and **20+
disabled by default** because they need external dependencies. The full
inventory is in:

- [Local set](/docs/plugins/local-set) — the 18 enabled, what each does.
- [Disabled priority](/docs/plugins/disabled-priority) — the disabled ones, their
  dependencies, and the vcpkg recipe.
- [Standard](/docs/plugins/standard) — the Standard plugin pattern in detail.
- [I/O](/docs/plugins/io) — the I/O plugin pattern in detail.
- [GL](/docs/plugins/gl) — the GL plugin pattern in detail.
- [Authoring](/docs/plugins/authoring) — how to add a new plugin end-to-end.

## Why this matters

Two consequences of the plugin model that affect day-to-day work:

1. **Adding a feature is almost always a plugin, not a core change.**
   Touching `libs/`, `qCC/`, or `ccViewer/` for a new feature is the wrong
   default — it forces a full rebuild of the vendored libraries and
   pollutes the upstream fork. New features belong in `plugins/`.
2. **The plugin set is configurable at configure time.** Each plugin's
   `CMakeLists.txt` has an `option(PLUGIN_* …)` line. The fork
   configures a specific 18-plugin set so the bundle stays self-contained
   and the CI stays fast. Adding a plugin = adding `-DPLUGIN_*=ON` to
   the configure command.
