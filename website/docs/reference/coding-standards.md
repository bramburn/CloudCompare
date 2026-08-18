---
title: Coding standards
sidebar_label: Coding standards
sidebar_position: 1
---

# Coding standards

The minimum any contributor must follow. The full rules live in
[`CONTRIBUTING.md`](https://github.com/CloudCompare/CloudCompare/blob/master/CONTRIBUTING.md)
and `.clang-format` at the repo root.

## Language and toolchain

- **C++17** minimum. The fork uses C++20 where it gives a clean win
  (`std::span`, `std::ranges`) but every file should still compile
  on C++17.
- **Qt 6.8** conventions. `Q_OBJECT`, `QString::fromStdString`,
  `QStringLiteral`, `tr()` for user-visible strings.
- **OpenGL 2.1+** minimum (the fork enables 3.0+ via
  `qCC_glWindow`).
- **CMake 3.10+** for the C++ build; **CMake ≤ 4.3** (pinned to 4.3.0
  on this machine — see [Build on Windows](/docs/build/windows)).
- **clang-format** enforced in CI via the `check-format` target.

## Indentation and formatting

- Tabs of width 4. The fork's `.editorconfig` pins this.
- Run `cmake --build build --target check-format` before any commit
  that touches `.cpp`/`.h`. The target applies `.clang-format` to
  every changed file in the diff.

## Naming

| Construct | Style | Example |
|---|---|---|
| Class | `ccCamelCase` | `ccPointCloud` |
| Method / function | `lowerCamelCase` | `applyGLTransformation` |
| Variable | `lowerCamelCase` | `m_currentScalarField` |
| Member | `m_lowerCamelCase` | `m_app` |
| Static variable | `s_` prefix | `s_instanceCount` |
| Enumerator | `CC_` prefix | `CC_FERR_NO_ERROR` |
| Constant | `ALL_CAPS` | `MAX_POINTS_PER_OCTREE_LEVEL` |
| Macro | `MACRO_` prefix | `MACRO_GETTER` |

## File and class layout

- **File name matches the class name**: `ccConsole.h` / `ccConsole.cpp`
  for `class ccConsole`.
- **One class per header**, where practical. The exception is
  `ccHObject` and its many small subclasses — they live in
  `qCC_db/include/` in groups by topic.
- **Forward declarations** in headers where possible. Include only
  what you use in `.cpp` files.
- **Include guards** with `#pragma once`. The fork doesn't use
  `#ifndef HEADER_H` style — `.clang-format` reorders them anyway.

## The LGPL/GPL header

Every new file needs the official header at the top. The templates
are in `CONTRIBUTING.md`. The fork's two variants:

```cpp
//##########################################################################
//#                                                                        #
//#                CLOUDCOMPARE PLUGIN: qMyTool                            #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                    COPYRIGHT: Icelabz Surveying                        #
//#                                                                        #
//##########################################################################

// (or the LGPL version, for files in libs/qCC_*, libs/CCCoreLib, …)
```

## What `clang-format` won't catch

A few things you have to do manually:

1. **Initialize member variables in the header** (`int m_count = 0;`),
   not in the constructor body. The `.clang-format` config doesn't
   rewrite this.
2. **Use `nullptr`**, never `NULL` or `0`.
3. **Use `QString::fromStdString` / `QString::toStdString` at the
   boundary**, not in the inner loop. Most of `qCC_db` and
   `CCCoreLib` are std::string-based for performance; the conversion
   happens once when crossing into a Qt-aware layer.
4. **Don't `delete` something the parent owns.** `QObject`-derived
   classes are children of their parents and get deleted
   automatically.
5. **Don't use `qApp` directly.** Go through `ccMainAppInterface`.

## Verifying before commit

```bash
# Format
cmake --build build --target check-format

# Build
cmake --build build --config Release

# Smoke test
& 'C:\dev\CloudCompare\build\qCC\deployqt\CloudCompare.exe'
```

If `check-format` rewrites files, stage and commit the rewrites
separately so the diff is readable.
