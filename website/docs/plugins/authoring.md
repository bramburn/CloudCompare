---
title: Authoring a new plugin
sidebar_label: Authoring
sidebar_position: 7
---

# Authoring a new plugin

The shortest path from "I want to add a tool" to "the tool is in the
Plugins menu". The recipe is the same for Standard, I/O, and GL plugins;
the interface is what changes.

## 1. Copy the template

```bash
# Standard plugin
cp -r plugins/example/ExamplePlugin plugins/core/Standard/qMyTool

# I/O plugin
cp -r plugins/example/ExampleIOPlugin plugins/core/IO/qMyFormat

# GL plugin
cp -r plugins/example/ExampleGLPlugin plugins/core/GL/qMyEffect
```

The templates are intentionally minimal — one source file, one
`CMakeLists.txt`, one icon, and a stub `getActions()`.

## 2. Rename

Use `git mv` so the rename is tracked, then rewrite the class name
across the file:

```bash
git mv plugins/core/Standard/qMyTool/ExamplePlugin.h \
       plugins/core/Standard/qMyTool/qMyTool.h
git mv plugins/core/Standard/qMyTool/ExamplePlugin.cpp \
       plugins/core/Standard/qMyTool/qMyTool.cpp
```

Then in `qMyTool.h`:

```cpp
class Q_MY_TOOL_PLUGIN : public QObject, public ccStdPluginInterface {
  Q_OBJECT
  Q_INTERFACES(ccStdPluginInterface)
  Q_PLUGIN_METADATA(IID "ccorp.cloudcompare.plugin.qMyTool")

public:
  explicit Q_MY_TOOL_PLUGIN(QObject* parent = nullptr);
  ~Q_MY_TOOL_PLUGIN() override = default;
  // ...
};
```

`Q_PLUGIN_METADATA(IID "…")` must be unique across all loaded plugins.
The convention is `ccorp.cloudcompare.plugin.q<Name>`. The host uses
this IID to detect double-loads.

## 3. Wire it to the build

Each plugin's `CMakeLists.txt` starts with:

```cmake
option(PLUGIN_STANDARD_QMYTOOL "Install qMyTool plugin" OFF)

if(PLUGIN_STANDARD_QMYTOOL)
  project(qMyTool)
  AddPlugin(NAME ${PROJECT_NAME} TYPE standard)
  add_subdirectory(src)
endif()
```

The `AddPlugin` function lives in
[`cmake/Plugin.cmake`](https://github.com/bramburn/CloudCompare/blob/master/cmake/Plugin.cmake)
and sets up:

- The plugin's shared-library target (`qMyTool.dll` / `libqMyTool.so`).
- A `post-install` command that copies the plugin to the right subfolder
  of the install prefix.
- A `deployqt`-aware dependency on Qt 6 (so the plugin gets bundled on
  Windows).

## 4. Register in the parent `CMakeLists.txt`

Edit `plugins/core/Standard/CMakeLists.txt` (or `IO/` or `GL/`) and add:

```cmake
add_subdirectory(qMyTool)
```

inside the `if(PLUGIN_STANDARD_*)` block. The order doesn't matter; the
plugins are independent.

## 5. Verify

Re-configure with `--fresh`, build, and run. The plugin should appear
in **About > Plugins** and in the relevant menu.

```powershell
cmake -S C:\dev\CloudCompare -B C:\dev\CloudCompare\build -G Ninja --fresh `
  -DPLUGIN_STANDARD_QMYTOOL=ON

cmake --build C:\dev\CloudCompare\build --config Release --parallel 16
```

If your plugin doesn't appear:

1. Check the build log — `AddPlugin` should have reported a target.
2. Check the deploy bundle — `build\qCC\deployqt\plugins\standard\qMyTool.dll`
   should exist.
3. Check **About > Plugins** — if the plugin is listed but disabled,
   it's an `IID` collision (rename your `Q_PLUGIN_METADATA(IID …)`).
4. Run with `CC_CORE_LIB_VERBOSE=1` to get verbose plugin load messages.

## 6. CI

Add a `-DPLUGIN_STANDARD_QMYTOOL=ON` line to `.github/workflows/windows.yml`
if you want the slim CI to build your plugin. The upstream
`.github/workflows/build.yml` is the canonical "all plugins on" build
and is the right place to add the plugin to the test matrix if your
plugin needs a non-self-contained dependency.
