@echo off
cd /d "%~dp0"
if exist "lib\glfw3.dll" copy /Y "lib\glfw3.dll" "build\Release\" >nul
if exist "lib\glew32.dll" copy /Y "lib\glew32.dll" "build\Release\" >nul

if not exist "build\Release\mountain-lake-aurora.exe" (
    echo [ERROR] mountain-lake-aurora.exe not found! Running build first...
    call build.bat
)

echo Starting Mountain Lake Aurora...
start "" "build\Release\mountain-lake-aurora.exe"
