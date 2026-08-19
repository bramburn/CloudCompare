#!/usr/bin/env python3
import subprocess

cmake = r'C:\dev\tools\cmake-4.3.0\bin\cmake.exe'
src = r'C:\dev\CloudCompare'
build = r'C:\dev\CloudCompare\build'
ninja = r'C:\ProgramData\chocolatey\bin\ninja.exe'
qt = r'C:\dev\tools\Qt\6.8.3\msvc2022_64'
vcpkg = r'C:\dev\CloudCompare\vcpkg\scripts\buildsystems\vcpkg.cmake'
vcvars = r'C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat'

# Build the cmake command as a single string for cmd /c
cmake_cmd = (
    f'call "{vcvars}" && '
    f'"{cmake}" -S "{src}" -B "{build}" -G Ninja '
    f'-DCMAKE_BUILD_TYPE=Release '
    f'-DCMAKE_PREFIX_PATH="{qt}" '
    f'-DCMAKE_TOOLCHAIN_FILE="{vcpkg}" '
    f'-DCMAKE_MAKE_PROGRAM="{ninja}" '
    f'-DBUILD_TESTING=ON '
    f'-DPLUGIN_STANDARD_3DFIN=OFF '
    f'-DPLUGIN_STANDARD_QCOMPASS=OFF '
    f'-DPLUGIN_STANDARD_QRANSAC_SD=OFF '
    f'-DPLUGIN_STANDARD_QSRA=OFF '
    f'-DPLUGIN_STANDARD_QM3C2=ON '
    f'-DPLUGIN_STANDARD_QCSF=ON '
    f'-DPLUGIN_IO_QRECAP=ON'
)

print('Running configure...')
result = subprocess.run(
    ['cmd', '/c', cmake_cmd],
    capture_output=True, text=True, timeout=180
)
print(result.stdout[-5000:])
if result.stderr:
    print('STDERR:', result.stderr[-3000:])
print('Return code:', result.returncode)
