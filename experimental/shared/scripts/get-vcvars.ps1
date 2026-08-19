# Get-VCVars.ps1
# Capture MSVC environment into the current PowerShell session.
# Usage:
#   . C:\dev\CloudCompare\experimental\shared\scripts\get-vcvars.ps1
# Then $env:INCLUDE, $env:LIB, $env:LIBPATH, $env:VCToolsInstallDir are set.

# Try several known locations for vcvars64.bat (Community, BuildTools, Pro, Enterprise)
$vcvarsCandidates = @(
    'C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat',
    'C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat',
    'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat',
    'C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat'
)

$vcvars = $null
foreach ($cand in $vcvarsCandidates) {
    if (Test-Path $cand) { $vcvars = $cand; break }
}

if (-not (Test-Path $vcvars)) {
    throw "vcvars64.bat not found at: $vcvars"
}

# Write a temporary .cmd that runs vcvars64 then echoes the env vars we want.
$tmp = [System.IO.Path]::GetTempFileName() + '.cmd'
$sb = New-Object System.Text.StringBuilder
[void]$sb.AppendLine('@echo off')
[void]$sb.AppendLine('call "' + $vcvars + '" x64 >nul 2>&1')
[void]$sb.AppendLine('echo ---ENV-START---')
[void]$sb.AppendLine('echo INCLUDE=%INCLUDE%')
[void]$sb.AppendLine('echo LIB=%LIB%')
[void]$sb.AppendLine('echo LIBPATH=%LIBPATH%')
[void]$sb.AppendLine('echo VCToolsInstallDir=%VCToolsInstallDir%')
[void]$sb.AppendLine('echo WindowsSdkDir=%WindowsSdkDir%')
[void]$sb.AppendLine('echo VCINSTALLDIR=%VCINSTALLDIR%')
[void]$sb.AppendLine('echo WindowsSDKVersion=%WindowsSDKVersion%')
[void]$sb.AppendLine('echo ---ENV-END---')
[System.IO.File]::WriteAllText($tmp, $sb.ToString())

$output = & cmd.exe /c $tmp 2>&1
Remove-Item $tmp -ErrorAction SilentlyContinue

$inBlock = $false
foreach ($line in $output) {
    if ($line -eq '---ENV-START---') { $inBlock = $true; continue }
    if ($line -eq '---ENV-END---') { $inBlock = $false; break }
    if (-not $inBlock) { continue }
    if ($line -match '^([^=]+)=(.*)$') {
        $name = $matches[1]
        $value = $matches[2]
        if ($value -ne '' -and $value -ne '%' + $name + '%') {
            Set-Item -Path "Env:$name" -Value $value
        }
    }
}

# Prepend MSVC bin to PATH so cl.exe, link.exe, lib.exe are found
$vcBin = Join-Path $env:VCToolsInstallDir 'bin\Hostx64\x64'
$env:Path = $vcBin + ';' + $env:Path

# Prepend Windows SDK bin to PATH
if ($env:WindowsSdkDir -and $env:WindowsSDKVersion) {
    $sdkBin = Join-Path $env:WindowsSdkDir ('bin\' + $env:WindowsSDKVersion + 'x64')
    if (Test-Path $sdkBin) {
        $env:Path = $sdkBin + ';' + $env:Path
    }
}

Write-Host 'OK MSVC environment loaded'
Write-Host ('  VCToolsInstallDir: ' + $env:VCToolsInstallDir)
Write-Host ('  WindowsSDKVersion: ' + $env:WindowsSDKVersion)
$cl = & where.exe cl.exe 2>$null | Select-Object -First 1
Write-Host ('  cl.exe: ' + $cl)
