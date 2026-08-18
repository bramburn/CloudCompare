---
title: Getting started
sidebar_label: Overview
sidebar_position: 1
---

# Getting started

The shortest path from a fresh clone to a working `CloudCompare.exe` and a
deployed docs site.

## Prerequisites

A working toolchain. The exact pins are documented per-platform:

- [Prerequisites](/docs/getting-started/prerequisites) — the OS-agnostic dependency list.
- [Quick tour](/docs/getting-started/quick-tour) — what gets built, in what order, and where it
  lands.

## The two-command build

On a fresh Windows checkout of `bramburn/CloudCompare`:

```powershell
# Configure (re-run when changing plugins, with --fresh)
& C:\dev\tools\cc-configure.cmd

# Build (incremental, ~6-15 min cold, \<30 s warm)
& C:\dev\tools\cc-build.cmd
```

The configure script calls `vcvars64.bat`, pins `cmake-4.3.0`, points at
`Qt/6.8.3/msvc2022_64`, and uses the choco `ninja.exe` (not the depot_tools
shim). The build script runs `cmake --build` against the configured directory
and packs a `deployqt\` bundle next to `CloudCompare.exe`.

Run the result:

```powershell
& 'C:\dev\CloudCompare\build\qCC\deployqt\CloudCompare.exe'
```

## Run the docs site locally

The docs site lives in [`website/`](https://github.com/bramburn/CloudCompare/tree/master/website).
To preview changes before they go live:

```bash
cd website
npm install
npm start          # http://localhost:3000/CloudCompare/
```

The site is published to GitHub Pages on every push to `master` that touches
`website/**` — see [CI & deployment / GitHub Pages](/docs/ci/github-pages).

## Where to go next

- [Build on Windows](/docs/build/windows) — full toolchain narrative and the
  gotchas (CMake version cap, depot_tools ninja shim, `cmake --fresh`).
- [Plugin overview](/docs/plugins/overview) — the model and the local set.
- [Architecture](/docs/architecture/overview) — when you need to read code.
