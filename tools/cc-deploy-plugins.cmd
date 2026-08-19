@echo off
REM ============================================================================
REM CloudCompare plugin deployment helper
REM ----------------------------------------------------------------------------
REM cc-build.cmd + windeployqt only copies Qt dependencies into deployqt\.
REM CC plugins (qCoreIO, qCSF, qHelloCloud, ...) need to be copied separately
REM so the running app can discover them in <appDir>\plugins\.
REM
REM Run this AFTER a successful cc-build.cmd to get a fully deployable bundle.
REM Idempotent: safe to run multiple times.
REM
REM Usage:    tools\cc-deploy-plugins.cmd
REM Effect:   Mirrors build\plugins\*.dll into build\qCC\deployqt\plugins\.
REM Excludes: qReCapIO (needs the Autodesk ReCap SDK to load; not installed).
REM ============================================================================

setlocal

set "SRC=C:\dev\CloudCompare\build\plugins"
set "DST=C:\dev\CloudCompare\build\qCC\deployqt\plugins"
set "TMPEXCLUDE=%TEMP%\cc-deploy-exclude-%RANDOM%.txt"

if not exist "%SRC%" (
    echo [cc-deploy-plugins] No build\plugins folder. Run cc-build.cmd first.
    exit /b 1
)

if not exist "%DST%" mkdir "%DST%" >nul

REM xcopy /EXCLUDE needs one pattern per line
> "%TMPEXCLUDE%" echo qReCapIO\

xcopy /E /Y /I /EXCLUDE:%TMPEXCLUDE% "%SRC%\%DST:"=____%" "%DST%" >nul
set "RC=%ERRORLEVEL%"
del "%TMPEXCLUDE%" >nul 2>&1

if %RC% NEQ 0 (
    if %RC% NEQ 4 (
        echo [cc-deploy-plugins] xcopy failed with code %RC%
        exit /b %RC%
    )
)

echo [cc-deploy-plugins] Mirrored build\plugins into %DST% ^(qReCapIO skipped^)
echo [cc-deploy-plugins] To run: %DST%\..\CloudCompare.exe
echo [cc-deploy-plugins]          Help -^> About -^> Plugins to verify everything loaded.