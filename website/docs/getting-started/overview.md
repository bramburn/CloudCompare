---
title: Overview
sidebar_label: What you're building
sidebar_position: 1
---

# What you're building

The CloudCompare repository produces two executables and a folder of loadable
plugins. This page is a 60-second tour of the output.

## The two executables

| Binary | Source | Purpose |
|---|---|---|
| `qCC/CloudCompare.exe` | `qCC/` | Full GUI — db-tree, 3D view, console, plugin menus, command-line parser. |
| `ccViewer/ccViewer.exe` | `ccViewer/` | Read-only viewer with its own (separate) plugin path. |

The `deployqt\CloudCompare.exe` in the build output is a self-contained
~70&nbsp;MB bundle — `windeployqt` has copied every Qt 6 runtime DLL and
plugin DLL alongside the executable. No `PATH` manipulation is needed; you can
copy the folder to another Windows box with the same architecture and it just
runs.

## The plugins folder

The first time you run `CloudCompare.exe`, it creates a `plugins/` folder next
to itself. Each enabled plugin ships as one `.dll` (Windows), one `.so`
(Linux), or one `.dylib` (macOS, community-supported). Disabling a plugin is a
matter of removing its `.dll` (or using the `PLUGIN_*=OFF` flag at configure
time and rebuilding).

The fork ships with **18 plugins enabled** — see
[Plugins / Local set](/docs/plugins/local-set) for the full list and what each
one does. The disabled plugins and their external dependencies are listed in
[Plugins / Disabled priority](/docs/plugins/disabled-priority).

## The libs

Seven vendored libraries do the actual work. The split is deliberate — see
[Architecture / Layers](/docs/architecture/layers) for who links to whom. As a
rule:

- `CCCoreLib` does the math (octree, distance, ICP, normals, RANSAC).
- `qCC_db` owns the in-memory data model (`ccHObject` and its subclasses).
- `qCC_io` reads and writes file formats.
- `qCC_glWindow` owns the 3D viewport (Qt + OpenGL).
- `CCPluginAPI` is the contract every plugin implements.

If you only have time to read one source file, read `ccHObject.h`.

## What's not in the build

The C++ build does **not** produce the docs site. The docs site is built by
`website/` and published to GitHub Pages by a separate workflow. See
[CI / GitHub Pages](/docs/ci/github-pages).
