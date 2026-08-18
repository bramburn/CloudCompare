---
title: Naming conventions
sidebar_label: Naming
sidebar_position: 2
---

# Naming conventions

The naming rules from [Coding standards](/docs/reference/coding-standards)
applied in detail, with examples drawn from the existing code.

## Classes

`ccCamelCase`. Prefix `cc` for everything that lives in the
CloudCompare project (the `ccHObject` hierarchy, the `ccConsole`
widget, the `ccGLWindow` viewport). Third-party code keeps its
upstream prefix.

```
ccHObject, ccPointCloud, ccMesh, ccPolyline, ccOctree,
ccRasterizeTool, ccConsole, ccGLWindow, ccPluginManager
```

## Methods and functions

`lowerCamelCase`. Verb-first for actions, `is` / `has` / `can` for
predicates.

```
applyGLTransformation(...)
getName()
setVisible(bool)
isVisible() const
hasOneOfFlag(CC_CLASS_ENUM) const
canLoad(QString filename) const
```

## Variables

`lowerCamelCase`. Members get the `m_` prefix. Statics get `s_`.

```cpp
int m_currentScalarField;
QString m_lastError;
static int s_instanceCount;
```

## Constants

`ALL_CAPS`, with underscores.

```cpp
const int MAX_OCTREE_LEVEL = 12;
const double DEFAULT_CELL_SIZE = 1.0;
```

`constexpr` is preferred over `const`:

```cpp
constexpr int kMaxOctreeLevel = 12;
```

The fork uses both styles; new code should use `constexpr`.

## Enumerators

`CC_` prefix on the enumerator value; the enum itself is
`CamelCase`.

```cpp
enum class CC_FILE_ERROR {
  CC_FERR_NO_ERROR,
  CC_FERR_BAD_ARGUMENT,
  CC_FERR_UNKNOWN_FILE,
  CC_FERR_READING,
  CC_FERR_WRITING,
  CC_FERR_NOT_ENOUGH_MEMORY,
  CC_FERR_CONSOLE_ERROR,
  CC_FERR_CANCELED_BY_USER
};
```

## Macros

`MACRO_` prefix (most of the time). The fork's macro conventions:

```cpp
#define MACRO_GETTER(TYPE, NAME) \
  TYPE get##NAME() const { return m_##NAME; }

#define CC_FILE_ERROR_ALREADY_DEFINED 1
```

Avoid new macros when a `constexpr` function or a template will do.

## Plugin prefixes

Every plugin has a `Q_PLUGIN_METADATA(IID "ccorp.cloudcompare.plugin.q<Name>")`.
The `IID` is the unique identifier. The `Q_EXPORT_PLUGIN2(ClassName, ClassName)`
macro at the bottom of the plugin's main `.cpp` uses the **class name**,
not the plugin name. So:

```cpp
// File: plugins/core/Standard/qMyTool/qMyTool.h
class Q_MY_TOOL : public QObject, public ccStdPluginInterface {
  Q_OBJECT
  Q_INTERFACES(ccStdPluginInterface)
  Q_PLUGIN_METADATA(IID "ccorp.cloudcompare.plugin.qMyTool")
  // ...
};

// File: plugins/core/Standard/qMyTool/qMyTool.cpp
Q_EXPORT_PLUGIN2(Q_MY_TOOL, Q_MY_TOOL)
```

## CMake flags

`PLUGIN_<TYPE>_<NAME>`. The `<TYPE>` is `IO`, `STANDARD`, or `GL`.
The `<NAME>` is the plugin's `q`-prefixed name in **uppercase**, with
non-alphanumeric characters dropped (`3DFin` → `3DFIN`).

```
PLUGIN_IO_QLAS
PLUGIN_IO_QE57
PLUGIN_STANDARD_QCSF
PLUGIN_STANDARD_QM3C2
PLUGIN_STANDARD_3DFIN
PLUGIN_GL_QEDL
PLUGIN_GL_QSSAO
```

## What the rules don't cover

- **File naming**: matches the class. `ccConsole.h` for `class
  ccConsole`. Plugins use `q<Name>.h` for the main class and
  `q<Name>Dialog.h` for dialogs.
- **CMake target naming**: matches the plugin name.
  `add_library(qMyTool …)`, not `add_library(my_tool …)`.
- **Documentation comments**: the fork uses `///` for Doxygen
  comments on public APIs and `//` for implementation comments.
