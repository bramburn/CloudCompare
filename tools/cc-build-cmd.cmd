@echo off
:: Build CloudCompare via Ninja, using the correct vcvars environment.
:: Usage: tools\cc-build-cmd.cmd
:: (No args needed — reads settings from the existing CMake cache)
setlocal

:: Set up MSVC environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1

:: CMake 4.3.0 pinned
set "CMAKE_BIN=C:\dev\tools\cmake-4.3.0\bin\cmake.exe"

:: Build
%CMAKE_BIN% --build C:\dev\CloudCompare\build --parallel 16
