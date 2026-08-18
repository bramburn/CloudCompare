# cmake/AGENTS.md

Top-level CMake helpers consumed by the root `CMakeLists.txt`. **Don't edit unless you're changing build behaviour for everyone.**

```
cmake/
├── CMakePolicies.cmake           # cmake_policy() settings
├── CMakeSetCompilerOptions.cmake # warning levels, optimisation flags
├── CMakeExternalLibs.cmake       # Qt 6 find_package() + component list
├── CMakeInclude.cmake            # file-copy helper for install
├── DeployQt.cmake                # windeployqt invocation (the deployqt\ bundle)
└── Install.cmake                 # InstallSharedLibrary / InstallStaticLibrary
```

For the **plugin** CMake helpers (`AddPlugin`), see `plugins/cmake/Plugins.cmake` — that lives separately because it's only consumed by plugin folders.

## When to edit

- **`CMakeSetCompilerOptions.cmake`** — when changing warning flags, optimisation, or C++ standard for everyone. Touch carefully; CI runs on three compilers (MSVC, GCC, Clang).
- **`CMakeExternalLibs.cmake`** — when adding a new Qt module to the `find_package()` list. Plugins that need extra Qt modules must respect what's already enabled here.
- **`DeployQt.cmake`** — when changing how the `deployqt\` self-contained bundle is built (Windows). The current logic runs `windeployqt` post-build to copy Qt6 DLLs + plugin DLLs alongside `CloudCompare.exe` / `ccViewer.exe`.
- **`Install.cmake`** — when changing the install layout (where DLLs / shared libs / shaders / translations land).

## When **not** to edit

- **`CMakePolicies.cmake`** — historical; do not touch without a strong reason.
- **`CMakeInclude.cmake`** — `copy_files()` helper; if you need a new install helper, add it here.
- Anything in `cmake/` for a per-plugin change — use `plugins/cmake/Plugins.cmake` or the plugin's own `CMakeLists.txt`.

## See also

- Root [`../AGENTS.md`](../AGENTS.md)
- [`../BUILD.md`](../BUILD.md) — upstream build instructions (the canonical place to learn what each option does)
- [`../plugins/cmake/Plugins.cmake`](../plugins/cmake/Plugins.cmake) — the `AddPlugin` helper
