@echo off
REM ============================================================================
REM CloudCompare configure script (in-repo)
REM ----------------------------------------------------------------------------
REM Lives at C:\dev\CloudCompare\tools\cc-configure.cmd so the build wrapper
REM ships with the repo. Update this file when you change any of:
REM   - the pinned CMAKE_PREFIX_PATH (Qt install dir)
REM   - the vcpkg toolchain file location
REM   - the enabled PLUGIN_* flags
REM
REM Toolchain layout this script assumes (Windows + VS 2022):
REM   - MSVC:        C:\Program Files\Microsoft Visual Studio\2022\Community\...
REM   - Qt 6.8.3:    C:\dev\tools\Qt\6.8.3\msvc2022_64
REM   - vcpkg:       C:\dev\CloudCompare\vcpkg  (inside the repo, by design)
REM   - ReCap SDK:   C:\ReCapSDK_v26.0.2        (optional, only for qReCapIO)
REM   - Ninja:       C:\ProgramData\chocolatey\bin\ninja.exe
REM
REM Usage:    tools\cc-configure.cmd
REM Effect:   Re-runs `cmake --fresh` into C:\dev\CloudCompare\build\.
REM
REM Companion script: tools\cc-build.cmd (incremental build, no --fresh).
REM ============================================================================

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
REM Use the pinned CMake 4.3.0 explicitly (vcvars64 prepends its own CMake 3.31 which shadows PATH)
set "CMAKE_PATH=C:\dev\tools\cmake-4.3.0\bin\cmake.exe"

%CMAKE_PATH% -S C:\dev\CloudCompare -B C:\dev\CloudCompare\build -G Ninja --fresh -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON -DCMAKE_PREFIX_PATH=C:/dev/tools/Qt/6.8.3/msvc2022_64;C:/dev/CloudCompare/vcpkg/installed/x64-windows;C:/ReCapSDK_v26.0.2 -DCMAKE_TOOLCHAIN_FILE=C:/dev/CloudCompare/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_MAKE_PROGRAM=C:/ProgramData/chocolatey/bin/ninja.exe -DPLUGIN_IO_QCORE=ON -DPLUGIN_GL_QEDL=ON -DPLUGIN_GL_QSSAO=ON -DPLUGIN_STANDARD_QANIMATION=ON -DPLUGIN_STANDARD_QBROOM=ON -DPLUGIN_STANDARD_QCSF=ON -DPLUGIN_STANDARD_QM3C2=ON -DPLUGIN_STANDARD_QPOISSON_RECON=ON -DPLUGIN_STANDARD_QHPR=ON -DPLUGIN_STANDARD_QPCV=ON -DPLUGIN_STANDARD_QCOLORIMETRIC_SEGMENTER=ON -DPLUGIN_STANDARD_QMPLANE=ON -DPLUGIN_STANDARD_QVOXFALL=ON -DPLUGIN_STANDARD_QCOMPASS=OFF -DPLUGIN_STANDARD_QRANSAC_SD=OFF -DPLUGIN_STANDARD_QSRA=OFF -DPLUGIN_STANDARD_QCANUPO=ON -DPLUGIN_STANDARD_3DFIN=ON -DPLUGIN_IO_QE57=OFF -DPLUGIN_IO_QLAS=ON -DPLUGIN_IO_QLAS_FWF=OFF -DPLUGIN_IO_QPHOTOSCAN=OFF -DPLUGIN_IO_QDRACO=OFF -DPLUGIN_IO_QPDAL=OFF -DPLUGIN_IO_QFBX=OFF -DPLUGIN_IO_QSTEP=OFF -DPLUGIN_IO_QRDB=OFF -DPLUGIN_IO_QADDITIONAL=OFF -DPLUGIN_IO_QCSV_MATRIX=OFF -DPLUGIN_STANDARD_QCORK=OFF -DPLUGIN_STANDARD_QHOUGH_NORMALS=OFF -DPLUGIN_STANDARD_QMESH_BOOLEAN=OFF -DPLUGIN_STANDARD_QFACETS=OFF -DPLUGIN_STANDARD_QMASONRY=OFF -DPLUGIN_STANDARD_QTREEISO=OFF -DPLUGIN_STANDARD_3DMASC=OFF -DPLUGIN_STANDARD_QCLOUDLAYERS=OFF -DPLUGIN_STANDARD_QG3POINT=OFF -DPLUGIN_STANDARD_QJSONRPC=OFF -DPLUGIN_STANDARD_QPCL=OFF -DPLUGIN_IO_QRECAP=OFF -DPLUGIN_EXPERIMENTAL_QHELLOCLOUD=ON
