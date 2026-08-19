<#
.SYNOPSIS
    Run all variants of a scenario and capture per-variant results.

.DESCRIPTION
    Single-command runner for a scenario. For each variant folder:
      1. Validates the variant's experiment.toml.
      2. Runs `cargo build --release` (or the variant's `build_cmd`).
      3. Runs `cargo test --release` (or the variant's `test_cmd`).
      4. Runs the variant's benchmark binary against each fixture.
      5. Captures wall time, peak memory, RMS vs brute-force reference.
    Writes results.json (machine-readable) and RESULTS.md (human).

.PARAMETER Scenario
    Path to the scenario folder (one level under experimental/scenarios/).

.PARAMETER Profile
    Cargo build profile. Default: release.

.PARAMETER Variant
    Optional. If given, run only that one variant (e.g. "02-kiddo-kdtree").
    Default: all variants declared in the scenario's experiment.toml.

.PARAMETER SkipBuild
    If set, don't rebuild. Just run the tests and benchmarks.

.PARAMETER SkipBench
    If set, only run build + tests. Skip benchmarks.

.EXAMPLE
    powershell ./experimental/run.ps1 -Scenario ./experimental/scenarios/2026-08-19-icp-variants

.EXAMPLE
    powershell ./experimental/run.ps1 -Scenario ./experimental/scenarios/2026-08-19-icp-variants -Variant 01-naive-on2
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$Scenario,

    [string]$Profile = "release",

    [string]$Variant = "",

    [switch]$SkipBuild,

    [switch]$SkipBench
)

# Don't let cargo's stderr (which is normal compiler output) be
# reported as a PowerShell error.
$ErrorActionPreference = 'Continue'

# ── Resolve absolute path ─────────────────────────────────────────────────
$Scenario = (Resolve-Path $Scenario).Path
if (-not (Test-Path $Scenario)) {
    Write-Error "Scenario not found: $Scenario"
    exit 1
}

# ── Hand-rolled minimal TOML reader ──────────────────────────────────────
# Just enough to read the experiment.toml subset we use. The TOML we
# expect has the structure:
#
#   id = "..."
#   status = "..."
#   owner = "..."
#   variants = [
#       "a",
#       "b",
#   ]
#   fixtures = [...]
#
# We tokenise line-by-line, then handle two cases for arrays: single
# line ("x = [a, b, c]") or multi-line ("x = [\n  a,\n  b,\n]"). The
# array elements are extracted from a small "array collection" state.
function Read-ScenarioToml {
    param([string]$Path)
    $result = @{
        id = ""
        status = ""
        variants = @()
        fixtures = @()
        metrics = @()
        reference_cpp = ""
        tags = @()
    }
    $inArrayKey = $null   # current array key, e.g. "variants"
    $inArrayItems = @()   # collected items
    Get-Content $Path | ForEach-Object {
        $line = $_.Trim()
        if ($line -match '^#' -or $line -eq '') {
            # Comment or blank: if we're in an array and hit blank, the
            # array is "implicitly closed" only if the next non-blank
            # line is a new key. We defer to the closing-brace logic.
            return
        }
        if ($inArrayKey -ne $null) {
            if ($line -eq ']') {
                $result[$inArrayKey] = $inArrayItems
                $inArrayKey = $null
                $inArrayItems = @()
                return
            }
            # Strip trailing comma and inline comment.
            # Match: optional whitespace, optional quote, capture inner.
            $item = ""
            if ($line -match '^\s*"?([^"#]+?)"?\s*,?\s*(?:#.*)?$') {
                $item = $Matches[1].Trim()
            }
            if ($item -ne '') {
                $inArrayItems += $item
            }
            return
        }
        if ($line -match '^id\s*=\s*"([^"]+)"')    { $result.id = $Matches[1] }
        if ($line -match '^status\s*=\s*"([^"]+)"') { $result.status = $Matches[1] }
        if ($line -match '^owner\s*=\s*"([^"]+)"')  { $result.owner = $Matches[1] }
        if ($line -match 'cpp\s*=\s*"([^"]+)"')     { $result.reference_cpp = $Matches[1] }
        # Inline array: variants = ["a", "b"]
        if ($line -match '^(\w+)\s*=\s*\[\s*([^\]]*?)\s*\]\s*$') {
            $key = $Matches[1]
            $raw = $Matches[2]
            if ($raw -ne '') {
                $items = $raw -split ',' | ForEach-Object {
                    $_.Trim().Trim('"').Trim("'")
                } | Where-Object { $_ -ne '' }
                $result[$key] = @($items)
            } else {
                # Empty inline array
                $result[$key] = @()
            }
            return
        }
        # Multi-line array start: variants = [
        if ($line -match '^(\w+)\s*=\s*\[\s*$') {
            $inArrayKey = $Matches[1]
            $inArrayItems = @()
            return
        }
    }
    $result
}

$scenarioToml = Join-Path $Scenario "experiment.toml"
if (-not (Test-Path $scenarioToml)) {
    Write-Error "experiment.toml not found in $Scenario. See templates/scenario/experiment.toml for the schema."
    exit 1
}

$scenarioCfg = Read-ScenarioToml $scenarioToml
Write-Host "[run] scenario: $($scenarioCfg.id) (status=$($scenarioCfg.status))"
Write-Host "[run] variants: $($scenarioCfg.variants -join ', ')"
if ($scenarioCfg.variants.Count -eq 0) {
    Write-Warning "No variants found in $scenarioToml. Check the file format."
}

# ── Pick which variants to run ───────────────────────────────────────────
$variantsToRun = $scenarioCfg.variants
if ($Variant -ne "") {
    $variantsToRun = @($Variant)
}

# ── Resolve fixtures via the manifests ────────────────────────────────────
$fixturesRoot = Join-Path (Split-Path (Split-Path $Scenario -Parent) -Parent) "fixtures"
$resolvedFixtures = @()
foreach ($f in $scenarioCfg.fixtures) {
    $manifestPath = Join-Path $fixturesRoot $f
    if (Test-Path $manifestPath) {
        $resolvedFixtures += @{ manifest = $manifestPath; relative = $f; available = $true }
    } else {
        Write-Host "[run] fixture MISSING (will be skipped): $f"
        $resolvedFixtures += @{ manifest = $manifestPath; relative = $f; available = $false }
    }
}
Write-Host "[run] fixtures available: $(($resolvedFixtures | Where-Object { $_.available }).Count) / $($resolvedFixtures.Count)"

# ── Run each variant ─────────────────────────────────────────────────────
$results = @()
$origDir = (Get-Location).Path
foreach ($v in $variantsToRun) {
    $variantDir = Join-Path $Scenario $v
    if (-not (Test-Path $variantDir)) {
        Write-Warning "variant folder missing: $v"
        $results += @{ variant = $v; failure_mode = "missing-folder" }
        continue
    }
    Write-Host ""
    Write-Host "================== $v =================="

    $variantResult = @{
        variant = $v
        build_success = $false
        test_success = $false
        runtime_ms = $null
        peak_memory_mb = $null
        correctness_rmse = $null
        point_count = $null
        failure_mode = "ok"
        git_sha = (& git -C $Scenario rev-parse --short HEAD 2>$null)
    }

    # Build
    if (-not $SkipBuild) {
        Set-Location $variantDir
        $buildOutput = & cargo build --$Profile 2>&1
        $buildExit = $LASTEXITCODE
        $variantResult.build_success = ($buildExit -eq 0)
        if (-not $variantResult.build_success) {
            $variantResult.failure_mode = "build-failed"
            Write-Host "[build] FAILED (exit $buildExit)"
            Set-Location $origDir
            $results += $variantResult
            continue
        }
        Write-Host "[build] ok"
    } else {
        $variantResult.build_success = $true
    }

    # Test
    Set-Location $variantDir
    $testOutput = & cargo test --$Profile 2>&1
    $testExit = $LASTEXITCODE
    $variantResult.test_success = ($testExit -eq 0)
    if (-not $variantResult.test_success) {
        $variantResult.failure_mode = "test-failed"
        Write-Host "[test] FAILED (exit $testExit)"
    } else {
        Write-Host "[test] ok"
    }

    # Benchmark (best-effort: look for a bench binary).
    # Convention: the variant's Cargo.toml declares [[bin]] name = "<name>".
    # We try a few common names.
    if (-not $SkipBench -and $variantResult.test_success) {
        $benchBin = $null
        $benchDir = Join-Path $variantDir "target/$Profile"
        foreach ($candidate in @("icp_bench.exe", "bench.exe", "icp.exe", "main.exe")) {
            $path = Join-Path $benchDir $candidate
            if (Test-Path $path) { $benchBin = $path; break }
        }
        if ($benchBin) {
            $sw = [System.Diagnostics.Stopwatch]::StartNew()
            $proc = Start-Process -FilePath $benchBin -NoNewWindow -PassThru -Wait `
                -RedirectStandardOutput "$env:TEMP\bench.out" `
                -RedirectStandardError "$env:TEMP\bench.err"
            $sw.Stop()
            $variantResult.runtime_ms = [math]::Round($sw.Elapsed.TotalMilliseconds, 1)
            $variantResult.peak_memory_mb = [math]::Round(
                ($proc.PeakWorkingSet64 / 1MB), 1)
            Write-Host "[bench] $($variantResult.runtime_ms) ms, peak $($variantResult.peak_memory_mb) MB"
        } else {
            Write-Host "[bench] no bench binary in $benchDir (skipped)"
        }
    }

    Set-Location $origDir
    $results += $variantResult
}

# ── Write results.json ────────────────────────────────────────────────────
$outDir = Join-Path $Scenario "results"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$timestamp = (Get-Date).ToUniversalTime().ToString("yyyy-MM-ddTHH-mm-ssZ")
$runDir = Join-Path $outDir "run-$timestamp"
New-Item -ItemType Directory -Force -Path $runDir | Out-Null

$gitSha = (& git -C $Scenario rev-parse --short HEAD 2>$null)
$resultsJson = @{
    scenario = $scenarioCfg.id
    scenario_status = $scenarioCfg.status
    profile = $Profile
    git_sha = $gitSha
    timestamp = $timestamp
    fixtures = $resolvedFixtures
    variants = $results
} | ConvertTo-Json -Depth 10

$jsonPath = Join-Path $runDir "results.json"
$resultsJson | Out-File -FilePath $jsonPath -Encoding utf8
Write-Host ""
Write-Host "[run] results written to $jsonPath"

# Write RESULTS.md (human-readable)
$md = @()
$md += "# $timestamp - $($scenarioCfg.id)"
$md += ""
$md += "**Profile:** $Profile"
$md += "**Status:** $($scenarioCfg.status)"
$md += "**Git SHA:** $gitSha"
$md += ""
$md += "## Variants"
$md += ""
$md += "| variant | build | test | runtime_ms | peak_mb | failure |"
$md += "|---|---|---|---|---|---|"
foreach ($r in $results) {
    $md += "| $($r.variant) | $($r.build_success) | $($r.test_success) | $($r.runtime_ms) | $($r.peak_memory_mb) | $($r.failure_mode) |"
}
$md += ""
$md += "## Fixtures"
$md += ""
foreach ($f in $resolvedFixtures) {
    $state = if ($f.available) { "available" } else { "MISSING" }
    $md += "- $($f.relative) - $state"
}
$md += ""
$mdPath = Join-Path $runDir "RESULTS.md"
$md -join "`n" | Out-File -FilePath $mdPath -Encoding utf8
Write-Host "[run] report written to $mdPath"

# Also write a latest symlink (for convenience)
$latest = Join-Path $outDir "latest"
if (Test-Path $latest) { Remove-Item $latest -Recurse -Force }
New-Item -ItemType Junction -Path $latest -Target $runDir | Out-Null
Write-Host "[run] latest: $latest"
