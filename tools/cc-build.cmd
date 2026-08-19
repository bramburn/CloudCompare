@echo off
REM ============================================================================
REM CloudCompare incremental build script (in-repo)
REM ----------------------------------------------------------------------------
REM Lives at C:\dev\CloudCompare\tools\cc-build.cmd alongside cc-configure.cmd.
REM Use this for source-only changes after the initial `cmake --fresh` configure.
REM
REM Usage:    tools\cc-build.cmd
REM Effect:   Incremental build into C:\dev\CloudCompare\build\.
REM
REM For a clean re-configure (changing plugins, Qt version, toolchain):
REM   1. tools\cc-configure.cmd
REM   2. tools\cc-build.cmd
REM ============================================================================

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cmake --build C:\dev\CloudCompare\build --config Release --parallel 16
