$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectDir = Resolve-Path (Join-Path $scriptDir "..")
$buildDir = Join-Path $projectDir "build"

if (-not (Test-Path $buildDir)) {
  Write-Host "[clean] Build directory not found. Nothing to clean."
  exit 0
}

$entries = Get-ChildItem -Path $buildDir -Force
if ($entries.Count -eq 0) {
  Write-Host "[clean] Build directory is already clean."
} else {
  $entries | Remove-Item -Recurse -Force
  Write-Host "[clean] Removed build artifacts from $buildDir"
}

New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
New-Item -ItemType File -Force -Path (Join-Path $buildDir ".gitkeep") | Out-Null
