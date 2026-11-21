@echo off
setlocal

set CMAKE_BIN=cmake
where %CMAKE_BIN% >nul 2>nul
if %ERRORLEVEL% EQU 0 goto cmake_found

if exist "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
    set CMAKE_BIN="C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    goto cmake_found
)

if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
    set CMAKE_BIN="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    goto cmake_found
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
    set CMAKE_BIN="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    goto cmake_found
)

echo [ERROR] CMake executable not found!
exit /b 1

:cmake_found
echo [1/3] Configuring CMake for Mountain Lake Aurora...
%CMAKE_BIN% -B build -S . -DVCPKG_MANIFEST_MODE=OFF
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed!
    exit /b %ERRORLEVEL%
)

echo.
echo [2/3] Building Mountain Lake Aurora (Release)...
%CMAKE_BIN% --build build --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed!
    exit /b %ERRORLEVEL%
)

echo.
echo [3/3] Deploying runtime dynamic libraries (DLLs)...
if exist "lib\glfw3.dll" copy /Y "lib\glfw3.dll" "build\Release\" >nul
if exist "lib\glew32.dll" copy /Y "lib\glew32.dll" "build\Release\" >nul

echo.
echo ========================================================
echo [SUCCESS] Build completed successfully!
echo Executable located in build\Release\mountain-lake-aurora.exe
echo ========================================================
