@echo off
setlocal

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

"C:\dev\tools\cmake-4.3.0\bin\cmake.exe" -S "C:\dev\CloudCompare" -B "C:\dev\CloudCompare\build" -G Ninja ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="C:\dev\tools\Qt\6.8.3\msvc2022_64" ^
    -DCMAKE_TOOLCHAIN_FILE="C:\dev\CloudCompare\vcpkg\scripts\buildsystems\vcpkg.cmake" ^
    -DCMAKE_MAKE_PROGRAM="C:\ProgramData\chocolatey\bin\ninja.exe" ^
    -DBUILD_TESTING=ON ^
    -DPLUGIN_STANDARD_3DFIN=OFF ^
    -DPLUGIN_STANDARD_QCOMPASS=OFF ^
    -DPLUGIN_STANDARD_QRANSAC_SD=OFF ^
    -DPLUGIN_STANDARD_QSRA=OFF ^
    -DPLUGIN_STANDARD_QM3C2=ON ^
    -DPLUGIN_STANDARD_QCSF=ON ^
    -DPLUGIN_IO_QRECAP=ON

if errorlevel 1 (
    echo Configure FAILED
    exit /b 1
)
echo Configure succeeded
endlocal
