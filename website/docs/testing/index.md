---
title: Unit testing
sidebar_label: Unit testing
sidebar_position: 10
---

# Unit testing

CloudCompare uses **Qt Test** (`Qt6::Test`) for unit tests. `BUILD_TESTING=ON`
is set in `tools/cc-configure.cmd`, so tests are compiled by default in the
local build.

The full test coverage roadmap is in
[`test-coverage-action-list.md`](https://github.com/bramburn/CloudCompare/blob/master/test-coverage-action-list.md)
(T1–T4, 30% → 90% coverage).

## Test binaries

```
C:\dev\CloudCompare\build\qCC\test\TestArgumentParser.exe
C:\dev\CloudCompare\build\qCC\test\TestFileIOFilter.exe
C:\dev\CloudCompare\build\qCC\test\TestPointCloud.exe
C:\dev\CloudCompare\build\qCC\test\TestShiftedObject.exe
C:\dev\CloudCompare\build\qCC\test\TestRegistration.exe
C:\dev\CloudCompare\build\qCC\test\TestM3C2.exe
C:\dev\CloudCompare\build\qCC\test\TestDBTree.exe
plugins/core/Standard/qCSF/TestCSF.exe
```

## Running tests

:::warning
Never run Qt Test executables directly from PowerShell without a DLL PATH.
The DLLs are spread across `build/libs/` subdirectories — the system
`PATH` does not include them. Use Python `subprocess` to set `PATH`
explicitly, or call the CMake test runner which sets up the environment.
:::

### Option 1 — CMake test runner (recommended)

```powershell
cmake --build build --target check     # runs all registered tests
ctest --test-dir build --output-on-failure  # alternative
```

### Option 2 — Run a single binary with DLL path

```python
import subprocess, os

dll_dirs = [
    'C:/dev/CloudCompare/build/libs/CCPluginAPI',
    'C:/dev/CloudCompare/build/libs/qCC_db',
    'C:/dev/CloudCompare/build/libs/qCC_db/extern/CCCoreLib',
    'C:/dev/CloudCompare/build/libs/CCFbo',
    'C:/dev/CloudCompare/build/libs/qCC_glWindow',
    'C:/dev/CloudCompare/build/libs/qCC_io',
    'C:/dev/CloudCompare/build/libs/qCC_io/extern/shapelib',
    'C:/dev/tools/Qt/6.8.3/msvc2022_64/bin',
]
env = os.environ.copy()
env['PATH'] = ';'.join(dll_dirs) + ';' + env.get('PATH', '')
result = subprocess.run(
    ['C:/dev/CloudCompare/build/qCC/test/TestArgumentParser.exe',
     '-txt', '-o', 'C:/dev/CloudCompare/test_result.txt'],
    capture_output=True, text=True, env=env, timeout=30
)
with open('C:/dev/CloudCompare/test_result.txt') as f:
    print(f.read())
```

## Issues hit and fixed

These are documented so they are not repeated:

| Issue | Symptom | Fix |
|---|---|---|
| `d3d11.lib not found` on link | Missing Windows SDK lib path | `cmake/CMakeExternalLibs.cmake` now auto-detects the newest installed SDK (`CC_WINDOWS_SDK_LIB_DIR`) and adds `/LIBPATH:` via `add_link_options()` in `qCC/test/CMakeLists.txt`. |
| `FileIOFilter::GetRealFilename` unresolved | Method had no export macro | `QCC_IO_LIB_API` now declared on the static method in `libs/qCC_io/include/FileIOFilter.h`. Any static method defined in a shared lib `.cpp` and called from outside needs the `*_LIB_API` macro. |
| `ccPointCloud::addColor()` access violation | Color table is a separate allocation | Call `reserveTheRGBTable()` before `addColor()`. Same pattern: `reserveTheNormsTable()` before `addNorm()`. |
| `cleanup()` slot ordering | `cleanup()` runs after every test function and can re-initialise filters | Structure tests so assertions on empty state come before any test that calls `UnregisterAll()` without re-init. |
| Wrong filter string assertions | `FileIOFilter::ImportFilterList()` uses `QObject::tr("All (*.*)")` not `"All Files (*.*)"` | Always grep the actual source for the exact string before writing assertions. |

## Writing new tests

All test files live in `qCC/test/`. Each new binary needs an entry in
`qCC/test/CMakeLists.txt`. Copy the existing pattern:

```cmake
add_executable(TestMyFeature test/TestMyFeature.cpp)
target_link_libraries(TestMyFeature PRIVATE
    Qt6::Test
    QCC_DB_LIB
    CCCoreLib
    QCC_IO_LIB
)
add_test(NAME TestMyFeature COMMAND TestMyFeature)
```

**Rules:**

- Use `QTEST_GUILESS_MAIN(ClassName)` for headless tests.
- `CCVector3` / `CCVector3d` have no `operator==` — compare `.x/.y/.z`
  individually.
- Scalar field: `addScalarField()` returns an index, `sf->setValue(i, val)`,
  `sf->computeMinAndMax()`, then `sf->getMin()` / `sf->getMax()`.
- Any new test target in `qCC/test/` must include the same `add_link_options`
  guard for `CC_WINDOWS_SDK_LIB_DIR` — otherwise it will fail to link on
  this machine.

## What to read next

- [Architecture / Layers](/docs/architecture/layers) — what each library owns.
- [Build / Windows](/docs/build/windows) — how the build is wired.
- [`test-coverage-action-list.md`](https://github.com/bramburn/CloudCompare/blob/master/test-coverage-action-list.md)
  — the T1–T4 coverage roadmap.
