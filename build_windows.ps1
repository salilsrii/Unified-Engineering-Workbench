$ErrorActionPreference = 'Stop'
cmake -S $PSScriptRoot -B "$PSScriptRoot\build" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build "$PSScriptRoot\build" --config Release
Write-Host "Built build\Release\workbench.exe. Windows 11 launch has not been independently verified."
