$ErrorActionPreference = 'Stop'
cmake -S $PSScriptRoot -B "$PSScriptRoot\build" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build "$PSScriptRoot\build" --config Release
Copy-Item -Force "$PSScriptRoot\build\Release\workbench.exe" "$PSScriptRoot\Launch.exe"
Write-Host "Created Launch.exe with a statically linked MSVC runtime. Run it on Windows 11 to verify before submission."
