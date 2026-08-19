# sandbox-build.ps1 — Build cc-sandbox with correct MSVC environment
#
# vcvars64.bat does NOT propagate env vars to the calling PowerShell session.
# This script captures the MSVC environment via `cmd /c` and passes it to cargo.
#
# Usage:
#   .\sandbox-build.ps1           # cargo check (fastest)
#   .\sandbox-build.ps1 build    # cargo build --release
#   .\sandbox-build.ps1 test     # cargo test
#   .\sandbox-build.ps1 clean    # cargo clean

param(
    [ValidateSet('', 'check', 'build', 'test', 'bench', 'clean')]
    [string]$Action = 'check'
)

$ErrorActionPreference = 'Stop'
$vcVarsBat = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
$sandboxRoot = Split-Path -Parent $MyInvocation.MyCommand.Path

Write-Host "[sandbox-build] Capturing MSVC environment from vcvarsall.bat..." -ForegroundColor Cyan

# Capture INCLUDE and LIB from vcvarsall.bat via cmd /c
# This is the ONLY reliable method — direct `& vcvarsall.bat` in PowerShell does NOT propagate
$includeLine = cmd /c "`"$vcVarsBat`" x64 >nul 2>&1 & set INCLUDE" 2>$null | Where-Object { $_ -match '^INCLUDE=' } | Select-Object -First 1
$libLine     = cmd /c "`"$vcVarsBat`" x64 >nul 2>&1 & set LIB"     2>$null | Where-Object { $_ -match '^LIB=' } | Select-Object -First 1

if ($includeLine) {
    $env:INCLUDE = $includeLine -replace '^INCLUDE=', ''
    Write-Host "[sandbox-build] INCLUDE: $($env:INCLUDE.Substring(0, [Math]::Min(80, $env:INCLUDE.Length)))..." -ForegroundColor Gray
}
if ($libLine) {
    $env:LIB = $libLine -replace '^LIB=', ''
    Write-Host "[sandbox-build] LIB: $($env:LIB.Substring(0, [Math]::Min(80, $env:LIB.Length)))..." -ForegroundColor Gray
}

# MSVC compiler/linker
$env:CC  = 'cl.exe'
$env:CXX = 'cl.exe'

# ── Dispatch ─────────────────────────────────────────────────────────────────
$manifestPath = Join-Path $sandboxRoot 'Cargo.toml'
$extraArgs = $args

switch ($Action) {
    'check' {
        Write-Host "[sandbox-build] cargo check..." -ForegroundColor Yellow
        cargo check --manifest-path $manifestPath @extraArgs
    }
    'build' {
        Write-Host "[sandbox-build] cargo build --release..." -ForegroundColor Yellow
        cargo build --manifest-path $manifestPath --release @extraArgs
    }
    'test' {
        Write-Host "[sandbox-build] cargo test..." -ForegroundColor Yellow
        cargo test --manifest-path $manifestPath @extraArgs
    }
    'bench' {
        Write-Host "[sandbox-build] cargo bench..." -ForegroundColor Yellow
        cargo bench --manifest-path $manifestPath @extraArgs
    }
    'clean' {
        Write-Host "[sandbox-build] cargo clean..." -ForegroundColor Yellow
        cargo clean --manifest-path $manifestPath
    }
}
