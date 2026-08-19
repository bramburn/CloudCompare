# Configuring CCCoreLib for the Sandbox

These steps connect the sandbox to the actual CCCoreLib C++ library.
You only need to do this once, or when CCCoreLib headers change.

## Option A: Use existing build DLLs (fastest)

If you've already built CloudCompare and `build/libs/CCCoreLib/` exists:

```powershell
# 1. Copy CCCoreLib DLLs and import libs
$build = "C:\dev\CloudCompare\build\libs\qCC_db\extern\CCCoreLib"
$dest   = "C:\dev\CloudCompare\sandbox\external\CCCoreLib_build\lib"
New-Item -ItemType Directory -Force -Path $dest
Copy-Item "$build\*.dll" $dest
Copy-Item "$build\*.lib" $dest

# 2. Copy headers
$hdest = "C:\dev\CloudCompare\sandbox\external\CCCoreLib_include"
New-Item -ItemType Directory -Force -Path $hdest
Copy-Item "C:\dev\CloudCompare\libs\qCC_db\extern\CCCoreLib\include\*" $hdest -Recurse
Copy-Item "C:\dev\CloudCompare\libs\qCC_db\extern\CCCoreLib\libs\qCC_db\extern\CCCoreLib\include\*" $hdest -ErrorAction SilentlyContinue

# 3. Update build.rs path if needed (should already point to external/CCCoreLib_build)
# No changes needed if using Option A path above.

# 4. Build sandbox
& 'C:\dev\CloudCompare\tools\sandbox\sandbox-build.cmd' build --release
```

## Option B: Build CCCoreLib standalone for the sandbox

If you want a clean, reproducible build of just CCCoreLib (recommended):

```powershell
# Configure CCCoreLib standalone (no Qt, no plugins)
cmake -S C:\dev\CloudCompare\libs\qCC_db\extern\CCCoreLib `
      -B C:\dev\CloudCompare\sandbox\external\CCCoreLib_build `
      -G Ninja `
      -DCMAKE_BUILD_TYPE=Release `
      -DCCCORELIB_SHARED=OFF `
      -DCCCORELIB_USE_TBB=OFF `
      -DCCCORELIB_USE_QT_CONCURRENT=ON `
      -DCMAKE_PREFIX_PATH="C:/dev/tools/Qt/6.8.3/msvc2022_64" `
      -DCMAKE_INSTALL_PREFIX="C:\dev\CloudCompare\sandbox\external\CCCoreLib_build\install"

# Build
cmake --build C:\dev\CloudCompare\sandbox\external\CCCoreLib_build `
       --config Release --parallel 8

# Install (copies to CMAKE_INSTALL_PREFIX)
cmake --install C:\dev\CloudCompare\sandbox\external\CCCoreLib_build --config Release

# Copy headers
New-Item -ItemType Directory -Force -Path C:\dev\CloudCompare\sandbox\external\CCCoreLib_include
Copy-Item "C:\dev\CloudCompare\sandbox\external\CCCoreLib_build\install\include\*" `
         C:\dev\CloudCompare\sandbox\external\CCCoreLib_include\ -Recurse

# Build sandbox
& 'C:\dev\CloudCompare\tools\sandbox\sandbox-build.cmd' build --release
```

## Verifying the Setup

After setup, verify:

```powershell
# Should show CCCoreLib.lib exists
Test-Path "C:\dev\CloudCompare\sandbox\external\CCCoreLib_build\lib\CCCoreLib.lib"

# Should show ScalarField.h exists
Test-Path "C:\dev\CloudCompare\sandbox\external\CCCoreLib_include\ScalarField.h"

# Should show CCCoreLib.dll exists (for runtime)
Test-Path "C:\dev\CloudCompare\sandbox\external\CCCoreLib_build\lib\CCCoreLib.dll"
```

## If Linking Fails

Common issues and fixes:

| Error | Fix |
|-------|-----|
| `unresolved external: FileIOFilter::Open` | Link qCC_io.lib — FileIOFilter is in qCC_io, not CCCoreLib |
| `unresolved external: ccPointCloud` | Link QCC_DB_LIB |
| `cannot open kernel32.lib` | Need vcvars64.bat — run `sandbox-build.cmd` not bare `cargo` |
| `MSVC runtime mismatch` | Rebuild CCCoreLib from source (Option B) |

## Key Note: qCC_io + QCC_DB_LIB needed too

The LAS loader (`cpp/las_loader.cc`) uses `FileIOFilter::Open()` which is in `qCC_io.lib`,
not `CCCoreLib.lib`. The full link set for the sandbox C++ loader is:

```
CCCoreLib.lib     ← ScalarField, KD-tree, octree algorithms
QCC_DB_LIB.lib    ← ccPointCloud, ccHObject
QCC_IO_LIB.lib   ← FileIOFilter::Open()
```

Update `build.rs` to link all three:

```rust
println!("cargo:rustc-link-search=native={}", ...);
println!("cargo:rustc-link-lib=CCCoreLib");
println!("cargo:rustc-link-lib=QCC_DB_LIB");
println!("cargo:rustc-link-lib=QCC_IO_LIB");
```
