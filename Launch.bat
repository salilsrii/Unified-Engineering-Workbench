@echo off
setlocal
cd /d "%~dp0"
if not exist "build\Release\workbench.exe" (
  powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build_windows.ps1"
  if errorlevel 1 exit /b 1
)
if not exist "%~dp0Launch.exe" (
  powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build_windows.ps1"
  if errorlevel 1 exit /b 1
)
start "" "%~dp0Launch.exe"
endlocal
