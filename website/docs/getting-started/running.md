---
title: Running
sidebar_label: Running
sidebar_position: 2
---

# Running CloudCompare

After a build, you can run CloudCompare immediately without installing anything. The output is a fully self-contained bundle.

## After a local build

After a successful `cc-build.cmd`, run:

```powershell
& 'C:\dev\CloudCompare\build\qCC\deployqt\CloudCompare.exe'
```

The `deployqt\` folder is a ~70 MB self-contained bundle: Qt 6 runtime DLLs + all enabled plugin DLLs are copied alongside the `.exe` by `windeployqt`. No `PATH` changes, no registry, no system install.

### Iterating: rebuild + run

```powershell
& C:\dev\tools\cc-configure.cmd   # re-run when changing plugins or CMake options
& C:\dev\tools\cc-build.cmd       # incremental: < 30 s for small changes
& 'C:\dev\CloudCompare\build\qCC\deployqt\CloudCompare.exe'
```

Only re-configure when toggling plugins or CMake flags. For source-only changes, just rebuild.

## From a CI artifact

The [Windows CI build](/docs/ci/windows-build) produces a downloadable zip artifact (`cloudcompare-windows-x64`).

1. Open the workflow run at **GitHub > Actions > Windows Build**.
2. Scroll to **Artifacts** and download `cloudcompare-windows-x64`.
3. Unzip anywhere on a Windows x64 machine.
4. Run `CloudCompare.exe` directly from the unzipped folder.

No install needed — it's the same `deployqt\` bundle that `windeployqt` produces locally.

## When you do need an install

You only need a separate install step when:

- Switching to a different Qt version (re-run `aqtinstall` or install a new Qt).
- Enabling a plugin that needs an external library (LAS, E57, PCL) — run `vcpkg install` first, then re-configure and rebuild.
- Switching toolchain (different MSVC version, different Qt Creator).

For the everyday dev loop (source edits, UI tweaks, plugin logic), rebuilding is all it takes.
