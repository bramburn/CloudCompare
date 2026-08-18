---
title: Build flags
sidebar_label: Build flags
sidebar_position: 3
---

# Build flags

The CMake options the fork exposes. The full list is in
`plugins/core/CMakeLists.txt` and per-plugin `CMakeLists.txt` files.

## Top-level

| Flag | Default | What it does |
|---|---|---|
| `BUILD_TESTING` | `OFF` | Build the unit-test target (`cc-test-lib`). |
| `BUILD_DOC` | `OFF` | Build the Doxygen documentation (the upstream `doc/` HTML). Not the same as this Docusaurus site. |
| `PLUGIN_IO_<NAME>` | per-plugin | Enable the I/O plugin `<NAME>`. |
| `PLUGIN_STANDARD_<NAME>` | per-plugin | Enable the Standard plugin `<NAME>`. |
| `PLUGIN_GL_<NAME>` | per-plugin | Enable the GL plugin `<NAME>`. |
| `WITH_PDAL` | `OFF` | Switch the LAS I/O to PDAL-based instead of the built-in PLY-based one. |
| `WITH_GDAL` | `OFF` | Enable GDAL-based raster import. |
| `WITH_FFMPEG` | `OFF` | Enable ffmpeg-based video / animation export. |
| `WITH_3DCONNEXION` | `ON` | Build the 3DConnexion space-mouse driver. |
| `WITH_PYTHON` | `OFF` | Build the Python plugin API. |
| `CC_CORE_LIB_VERBOSE` | `OFF` | Print verbose plugin-load messages on startup. |

## Plugin flags

The local set (default-on) is in [Plugins / Local set](/docs/plugins/local-set).
The disabled set and their dependencies is in
[Plugins / Disabled priority](/docs/plugins/disabled-priority).

The naming convention is `PLUGIN_<TYPE>_<NAME>` in upper-case, with the
`q` prefix dropped. Examples:

```
PLUGIN_IO_QLAS       → plugins/core/IO/qLASIO
PLUGIN_IO_QE57       → plugins/core/IO/qE57IO
PLUGIN_STANDARD_QCSF → plugins/core/Standard/qCSF
PLUGIN_STANDARD_3DFIN → plugins/core/Standard/3DFin  (no q prefix)
PLUGIN_GL_QEDL       → plugins/core/GL/qEDL
```

## Example invocations

### Default slim set

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64
```

### With LAS / E57

```bash
cmake -S . -B build -G Ninja --fresh \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64;C:/dev/vcpkg/installed/x64-windows \
  -DPLUGIN_IO_QLAS=ON \
  -DPLUGIN_IO_QE57=ON
```

### With unit tests

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_TESTING=ON

cmake --build build --target cc-test-lib
ctest --test-dir build --output-on-failure
```

### With everything

```bash
cmake -S . -B build -G Ninja --fresh \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64;C:/dev/vcpkg/installed/x64-windows \
  -DPLUGIN_IO_QLAS=ON \
  -DPLUGIN_IO_QE57=ON \
  -DPLUGIN_IO_QDRACO=ON \
  -DPLUGIN_STANDARD_QPCL=ON \
  -DPLUGIN_GL_QEDL=ON \
  -DPLUGIN_GL_QSSAO=ON \
  -DWITH_PDAL=ON \
  -DWITH_GDAL=ON
```

This takes 1-2 hours cold and produces a 200+ MB `deployqt\` bundle.
Not for the faint of heart.

## Cache management

Always pass `--fresh` when toggling plugin options. Without it,
stale `PLUGIN_*=ON` / `PLUGIN_*=OFF` values stick in the cache and
the plugin's `find_package(X)` runs (or doesn't) regardless of what
you think you just turned on.

If you want a *partial* fresh (keep some options, drop others), edit
`build/CMakeCache.txt` directly or use `cmake-gui`'s **Delete Cache**
button.
