@echo off
:: sandbox-build.cmd — Build the cc-sandbox Rust crate with MSVC toolchain
::
:: Usage:
::   sandbox-build.cmd          (cargo check — fastest)
::   sandbox-build.cmd build    (cargo build --release)
::   sandbox-build.cmd test     (cargo test)
::   sandbox-build.cmd bench    (cargo bench)
::
:: IMPORTANT: vcvars64.bat does NOT propagate env vars to PowerShell.
:: We capture them here with `cmd /c` and pass them explicitly to cargo.

setlocal enabledelayedexpansion

:: ── Paths ───────────────────────────────────────────────────────────────────
set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community"
set "VCVARS=%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat"
set "VCVARS64=%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"
set "SANDBOX_ROOT=%~dp0"
set "RUST_SANDBOX=%SANDBOX_ROOT%"

:: ── Capture MSVC environment via cmd /c (the only reliable method) ──────────
:: This works because cmd.exe expands %INCLUDE% correctly when vcvars runs.
echo [sandbox-build] Capturing MSVC environment from vcvarsall.bat...

:: Capture all env vars set by vcvars into a temp file
"%SystemRoot%\System32\cmd.exe" /c ""%VCVARS%" x64 >nul 2>&1 && set" > "%TEMP%\sandbox_msvc_env.txt"

:: Read each line: NAME=VALUE
for /f "usebackq tokens=1,* delims==" %%A in ("%TEMP%\sandbox_msvc_env.txt") do (
    set "MSC_%%A=%%B"
)

:: Set the critical vars for cargo/rustc
if defined MSC_INCLUDE set "INCLUDE=%MSC_INCLUDE%"
if defined MSC_LIB set "LIB=%MSC_LIB%"
if defined MSC_LIBPATH set "LIBPATH=%MSC_LIBPATH%"

:: Set CC/CXX for CXX crate detection (must use cl.exe from MSVC, not g++)
set "CC=cl.exe"
set "CXX=cl.exe"

:: Set cargo to use the MSVC target (already default on Windows)
set "CARGO_TARGET_X86_64_PC_WINDOWS_MSVC_LINKER=link.exe"
set "CARGO_TARGET_X86_64_PC_WINDOWS_MSVC_RUSTC_WRAPPER="

:: Echo what we captured (sanitised)
echo [sandbox-build] INCLUDE length: !INCLUDE:~0,80!...
echo [sandbox-build] LIB length: !LIB:~0,80!...
echo [sandbox-build] CC=%CC%

:: ── Dispatch ────────────────────────────────────────────────────────────────
set "CMD=%~1"

if "%CMD%"=="" goto check
if "%CMD%"=="check" goto check
if "%CMD%"=="build" goto build
if "%CMD%"=="test" goto test
if "%CMD%"=="bench" goto bench
if "%CMD%"=="clean" goto clean

:check
echo [sandbox-build] Running cargo check...
cargo check --manifest-path "%RUST_SANDBOX%\Cargo.toml" %*
goto end

:build
echo [sandbox-build] Running cargo build --release...
cargo build --manifest-path "%RUST_SANDBOX%\Cargo.toml" --release %*
goto end

:test
echo [sandbox-build] Running cargo test...
cargo test --manifest-path "%RUST_SANDBOX%\Cargo.toml" %*
goto end

:bench
echo [sandbox-build] Running cargo bench...
cargo bench --manifest-path "%RUST_SANDBOX%\Cargo.toml" %*
goto end

:clean
echo [sandbox-build] Cleaning sandbox...
cargo clean --manifest-path "%RUST_SANDBOX%\Cargo.toml"
goto end

:end
del /f /q "%TEMP%\sandbox_msvc_env.txt" 2>nul
endlocal
