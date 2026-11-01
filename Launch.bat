@echo off
setlocal
cd /d "%~dp0"
if not exist "build\Release\workbench.exe" (
  powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build_windows.ps1"
  if errorlevel 1 exit /b 1
)
start "PASS Workbench API" "%~dp0build\Release\workbench.exe" serve 8080
timeout /t 2 /nobreak >nul
start "" http://127.0.0.1:8080
endlocal
