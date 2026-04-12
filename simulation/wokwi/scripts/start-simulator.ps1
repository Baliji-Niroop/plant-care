# Purpose:
# - Attempt to trigger Wokwi simulator start through VS Code CLI.
# - Fall back to clear manual instructions when CLI triggering is unavailable.

$ErrorActionPreference = "Stop"

$codeCommand = Get-Command code -ErrorAction SilentlyContinue
if ($null -eq $codeCommand) {
  Write-Host "[start] VS Code CLI ('code') was not found in PATH."
  Write-Host "[start] Open VS Code and run: F1 -> Wokwi: Start Simulator"
  exit 0
}

# Best-effort trigger for the Wokwi command from CLI via VS Code command URI.
try {
  & code --open-url "vscode://command/wokwi.start"
  Write-Host "[start] Start command sent. If nothing opens, run: F1 -> Wokwi: Start Simulator"
} catch {
  Write-Host "[start] Could not trigger Wokwi from CLI. Run manually: F1 -> Wokwi: Start Simulator"
  exit 1
}
