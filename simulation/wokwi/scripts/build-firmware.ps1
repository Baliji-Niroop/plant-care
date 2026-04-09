param(
  [string]$Fqbn = "esp32:esp32:esp32",
  [switch]$Bootstrap
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectDir = Resolve-Path (Join-Path $scriptDir "..")
$sketchPath = Join-Path $projectDir "sketch.ino"
$buildDir = Join-Path $projectDir "build"
$stagingDir = Join-Path $buildDir "sketch"
$stagedSketch = Join-Path $stagingDir "sketch.ino"
$arduinoCli = Get-Command arduino-cli -ErrorAction SilentlyContinue

if ($null -eq $arduinoCli) {
  $candidate = "C:\Program Files\Arduino CLI\arduino-cli.exe"
  if (Test-Path $candidate) {
    $arduinoCli = $candidate
  }
}

if ($null -eq $arduinoCli) {
  throw "arduino-cli was not found in PATH. Install Arduino CLI first, or keep it at C:\Program Files\Arduino CLI\arduino-cli.exe."
}

if (-not (Test-Path $sketchPath)) {
  throw "Sketch file not found: $sketchPath"
}

New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
New-Item -ItemType Directory -Force -Path $stagingDir | Out-Null
Copy-Item -Path $sketchPath -Destination $stagedSketch -Force

if ($Bootstrap) {
  Write-Host "[setup] Updating package index..."
  & $arduinoCli core update-index

  Write-Host "[setup] Installing ESP32 core (if needed)..."
  & $arduinoCli core install esp32:esp32

  Write-Host "[setup] Installing required library (if needed)..."
  & $arduinoCli lib install "DHT sensor library for ESPx"
}

Write-Host "[build] Compiling $sketchPath"
& $arduinoCli compile `
  --fqbn $Fqbn `
  --build-path "$buildDir" `
  --output-dir "$buildDir" `
  "$stagingDir"

$targetBin = Join-Path $buildDir "sketch.ino.bin"
$targetElf = Join-Path $buildDir "sketch.ino.elf"

if (-not (Test-Path $targetBin)) {
  $latestBin = Get-ChildItem -Path $buildDir -Filter "*.bin" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
  if ($null -ne $latestBin) {
    Copy-Item -Path $latestBin.FullName -Destination $targetBin -Force
  }
}

if (-not (Test-Path $targetElf)) {
  $latestElf = Get-ChildItem -Path $buildDir -Filter "*.elf" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
  if ($null -ne $latestElf) {
    Copy-Item -Path $latestElf.FullName -Destination $targetElf -Force
  }
}

if (-not (Test-Path $targetBin)) {
  throw "Build completed but firmware binary was not found in $buildDir"
}
if (-not (Test-Path $targetElf)) {
  throw "Build completed but ELF file was not found in $buildDir"
}

Write-Host "[ok] Firmware: $targetBin"
Write-Host "[ok] ELF: $targetElf"
