@echo off
:: Configure CloudCompare with Ninja generator.
:: Must be run from an x64 Native Tools Developer Command Prompt for VS 2022,
:: or after calling vcvars64.bat.
:: Usage: tools\cc-configure.cmd [--fresh]
::   --fresh   Delete CMakeCache.txt before configuring

setlocal

set "SOURCE_DIR=C:\dev\CloudCompare"
set "BUILD_DIR=C:\dev\CloudCompare\build"

if "%1"=="--fresh" (
    echo [fresh] Removing CMake cache...
    if exist "%BUILD_DIR%\CMakeCache.txt" del /f /q "%BUILD_DIR%\CMakeCache.txt"
    if exist "%BUILD_DIR%\rules.ninja" del /f /q "%BUILD_DIR%\rules.ninja"
)

:: CMake 4.3.0 (pinned — hidapi submodule requires ≤4.3)
:: Always use this explicit path: vcvars64.bat prepends its bundled CMake 3.31 to PATH,
:: which shadows any standalone CMake and causes "unknown target" errors.
set "CMAKE_BIN=C:\dev\tools\cmake-4.3.0\bin\cmake.exe"

:: Qt 6.8.3
set "QT_PREFIX=C:\dev\tools\Qt\6.8.3\msvc2022_64"

:: vcpkg toolchain (vcpkg lives inside the repo at vcpkg/)
set "VCPKG_TOOLCHAIN=C:\dev\CloudCompare\vcpkg\scripts\buildsystems\vcpkg.cmake"

:: Ninja (chocolatey) — NOT depot_tools ninja (which is a Python script)
set "NINJA_BIN=C:\ProgramData\chocolatey\bin\ninja.exe"

:: Disabled plugins (known missing external dependencies on this machine)
::   cc3DFin: Taskflow requires C++20 on a C++17 project
::   qCompass/qRANSAC_SD/qSRA: ccTrace namespace collision with Qt 6.8.3 internal headers
:: Enabled plugins:
::   qM3C2: M3C2 statistics and normals tools
::   qCSF: Cloth Simulation Filter (Vec3, Particle, Cloth, Cloud2CloudDist)
::   qReCapIO: Autodesk ReCap SDK v27 — reads .rcs/.rcp files (native only, no ijwhost)
::   qLASIO: LAS/LAZ point cloud import via laszip (vcpkg-installed)
set "PLUGIN_EXTRAS=-DPLUGIN_STANDARD_3DFIN=OFF -DPLUGIN_STANDARD_QCOMPASS=OFF -DPLUGIN_STANDARD_QRANSAC_SD=OFF -DPLUGIN_STANDARD_QSRA=OFF -DPLUGIN_STANDARD_QM3C2=ON -DPLUGIN_STANDARD_QCSF=ON -DPLUGIN_STANDARD_QRUSTICP=ON -DPLUGIN_IO_QRECAP=ON -DPLUGIN_IO_QLAS=ON"

echo Configuring CloudCompare (Ninja generator)...
echo   CMake:  %CMAKE_BIN%
echo   Qt:     %QT_PREFIX%
echo   Ninja:  %NINJA_BIN%

"%CMAKE_BIN%" -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G Ninja ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="%QT_PREFIX%" ^
    -DCMAKE_TOOLCHAIN_FILE="%VCPKG_TOOLCHAIN%" ^
    -DCMAKE_MAKE_PROGRAM="%NINJA_BIN%" ^
    -DBUILD_TESTING=ON ^
    %PLUGIN_EXTRAS%

if errorlevel 1 (
    echo Configure FAILED
    exit /b 1
)

echo.
echo Configure succeeded
echo   Build:   cmake --build %BUILD_DIR% --parallel
echo   Run:     %BUILD_DIR%\qCC\deployqt\CloudCompare.exe
endlocal
