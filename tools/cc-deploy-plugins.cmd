@echo off
REM ============================================================================
REM CloudCompare plugin AND core-lib deployment helper
REM ----------------------------------------------------------------------------
REM cc-build.cmd + windeployqt only copies Qt dependencies into deployqt\.
REM Two more things are needed for a self-contained, runnable bundle:
REM   1) CC core libs (CCAppCommon, CCPluginAPI, QCC_DB_LIB, ..., CCCoreLib)
REM      -- live at the deployqt\ root so the .exe finds them next to itself
REM   2) CC plugin DLLs (qCoreIO, qCSF, qHelloCloud, ...) under deployqt\plugins\
REM      -- so the runtime plugin discovery picks them up
REM
REM Run this AFTER a successful cc-build.cmd to get a fully deployable bundle.
REM Idempotent: safe to run multiple times.
REM
REM Usage:    tools\cc-deploy-plugins.cmd
REM Effect:   (1) copies CC core lib DLLs to deployqt\ root
REM           (2) mirrors build\plugins\*.dll into deployqt\plugins\
REM Excludes: qReCapIO and its native deps (needs the Autodesk ReCap SDK)
REM ============================================================================

setlocal

set "BUILD=C:\dev\CloudCompare\build"
set "LIBSRC=%BUILD%\libs"
set "PLUGINSRC=%BUILD%\plugins"
set "DEPLOYQT=%BUILD%\qCC\deployqt"
set "DSTPLUGINS=%DEPLOYQT%\plugins"
set "TMPEXCLUDE=%TEMP%\cc-deploy-exclude-%RANDOM%.txt"

if not exist "%BUILD%" (
    echo [cc-deploy-plugins] No build folder. Run cc-build.cmd first.
    exit /b 1
)

REM ------------------------------------------------------------------
REM 1. CC core libs: copy DLLs from build\libs\<lib>\<lib>.dll to
REM    deployqt\ root. Skip build/CMakeFiles and build/<lib>/Release/
REM    duplicates to keep the deployqt root lean.
REM ------------------------------------------------------------------
if exist "%LIBSRC%" (
    echo [cc-deploy-plugins] Copying CC core libs to %DEPLOYQT%\ ...
    for /f "delims=" %%F in ('dir /b /s /a-d "%LIBSRC%\*.dll" 2^>nul') do (
        set "FULL=%LIBSRC%\%%F"
        setlocal enabledelayedexpansion
        echo !FULL! | findstr /I /C:"CMakeFiles" >nul
        if not errorlevel 1 (
            endlocal
        ) else (
            echo !FULL! | findstr /I /C:"\Release\" >nul
            if not errorlevel 1 (
                endlocal
            ) else (
                copy /Y "!FULL!" "%DEPLOYQT%\" >nul
                endlocal
            )
        )
    )
) else (
    echo [cc-deploy-plugins] No build\libs folder found, skipping core libs.
)

REM ------------------------------------------------------------------
REM 2. CC plugin DLLs: mirror build\plugins\* into deployqt\plugins\.
REM    xcopy with /E preserves the per-plugin subfolder layout.
REM ------------------------------------------------------------------
if exist "%PLUGINSRC%" (
    if not exist "%DSTPLUGINS%" mkdir "%DSTPLUGINS%" >nul

    REM xcopy /EXCLUDE needs one pattern per line
    > "%TMPEXCLUDE%" echo qReCapIO\

    xcopy /E /Y /I /EXCLUDE:%TMPEXCLUDE% "%PLUGINSRC%" "%DSTPLUGINS%" >nul
    set "RC=%ERRORLEVEL%"
    del "%TMPEXCLUDE%" >nul 2>&1

    if %RC% NEQ 0 (
        if %RC% NEQ 4 (
            echo [cc-deploy-plugins] xcopy failed with code %RC%
            exit /b %RC%
        )
    )
) else (
    echo [cc-deploy-plugins] No build\plugins folder found, skipping plugins.
)

echo [cc-deploy-plugins] Deploy bundle ready.
echo [cc-deploy-plugins] To run: %DEPLOYQT%\CloudCompare.exe
echo [cc-deploy-plugins]          Help -^> About -^> Plugins to verify everything loaded.