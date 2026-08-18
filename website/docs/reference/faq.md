---
title: FAQ
sidebar_label: FAQ
sidebar_position: 4
---

# FAQ

Quick answers to the questions that show up repeatedly on the upstream
forum and in the fork's local issues.

## The C++ build

**Q. The build dies with "Compatibility with CMake \< 4.4 will be
removed". What now?**

A. Use the pinned `C:\dev\tools\cmake-4.3.0\bin\cmake.exe`. The
bundled `hidapi` submodule in
`libs/CCAppCommon/devices/3dConnexion/extern/hidapi/CMakeLists.txt`
caps at 4.3. See
[Build / Prerequisites](/docs/getting-started/prerequisites) and
[Build / Troubleshooting](/docs/build/troubleshooting).

**Q. I added `-DPLUGIN_IO_QLAS=ON` and `find_package(LASzip)` failed.**

A. Install LASzip via vcpkg and pass the vcpkg prefix to
`CMAKE_PREFIX_PATH`. See
[Plugins / Disabled priority](/docs/plugins/disabled-priority) for the
exact recipe.

**Q. `cmake --build` is slow even on a warm cache.**

A. You're probably not using the pinned CMake. The build script's
PATH order matters: `cc-build.cmd` calls the pinned 4.3.0 directly.
Manually calling `cmake --build` from a stock PowerShell will pick up
vcvars's 3.31.6 (which works, but is slower at dependency analysis).

**Q. Can I build with MSBuild instead of Ninja?**

A. Yes — pass `-G "Visual Studio 17 2022" -A x64` instead of
`-G Ninja`. The build is ~30% slower. The fork's CI uses Ninja; the
local wrapper scripts use Ninja; only the upstream Visual Studio
generator is tested.

## The plugins

**Q. I added a new Standard plugin but it doesn't show up in
**Plugins > Standard Plugins**.**

A. The plugin's `Q_PLUGIN_METADATA(IID "…")` is colliding with
another loaded plugin, or the plugin's `Q_EXPORT_PLUGIN2` macro
points to the wrong class name. Enable `CC_CORE_LIB_VERBOSE=1` to
get verbose plugin-load messages from the host.

**Q. Can I bundle a third-party library inside my plugin?**

A. Yes — add it as a private dependency in the plugin's
`CMakeLists.txt`. The plugin is a self-contained `.dll` / `.so` /
`.dylib`, so the dependency doesn't leak into the host.

**Q. My plugin needs OpenCV. Does that work?**

A. Yes — `q3DMASC` does it. Add `find_package(OpenCV REQUIRED)` in
the plugin's `CMakeLists.txt` and link the components you need. The
upstream CI installs OpenCV via apt; locally, use vcpkg's
`opencv4:x64-windows`.

## The data model

**Q. How do I get a pointer to the active 3D viewport from a plugin?**

```cpp
ccGLWindow* gl = m_app->getActiveGLWindow();
if (!gl) {
  m_app->dispToConsole("No active 3D window", ccConsole::ERR);
  return;
}
```

**Q. How do I add an entity to the db-tree?**

```cpp
ccHObject* newEntity = /* ... build it ... */;
m_app->addToDB(newEntity, /* updateZoom = */ true);
```

The `updateZoom` flag refits the camera to the new entity's bounding
box. Set to `false` if you're adding many entities in a loop (one
refit at the end is faster).

**Q. How do I read a per-point scalar field from a `ccPointCloud`?**

```cpp
ccPointCloud* cloud = /* ... */;
ccScalarField* sf = cloud->getCurrentDisplayedScalarField();
if (sf) {
  for (unsigned i = 0; i \< cloud->size(); ++i) {
    ScalarType value = sf->getValue(i);
    // ...
  }
}
```

## The docs site

**Q. I changed a doc page locally — when does it deploy?**

A. On the next push to `master`. The
[GitHub Pages workflow](/docs/ci/github-pages) builds and publishes in
~1-2 minutes.

**Q. The sidebar doesn't show a new doc page I added.**

A. The sidebar is generated from `sidebars.js` based on the file IDs
in the front-matter. Add the page's `id` to the relevant category in
`sidebars.js`.

**Q. Can I add a new top-level section (e.g. "Tutorials")?**

A. Yes. Add a new category in `sidebars.js`, create the section
folder under `website/docs/`, and add `_category_.json` to control
the label and position.

## Contributing

**Q. I have a PR. Where do I open it — upstream or the fork?**

A. Open it on the upstream `CloudCompare/CloudCompare` repo unless
it's specific to the fork's CI / build scripts / this docs site.
Fork-only changes (like the slim CI matrix or the `windows.yml`
file) live on the fork.

**Q. How do I sync my fork with upstream?**

A. `git pull --ff-only origin master`. The fork's `master` is
fast-forwarded from upstream on every sync.
