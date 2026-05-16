@echo off
setlocal EnableDelayedExpansion

:: ==============================================================================
:: build.bat  -  ZeroStyle C++ build script
::
:: Usage:
::   build.bat                   Release build (configure only, header-only lib)
::   build.bat debug             Debug build
::   build.bat --cxx23           Enable C++23 (std::expected backend)
::   build.bat --tests           Compile and run smoke test
::   build.bat --clean           Delete build\ then reconfigure
::   Flags can be combined:  build.bat debug --cxx23 --tests --clean
:: ==============================================================================

set BUILD_DIR=build
set CONFIG=Release
set CXX23=OFF
set BUILD_TESTS=OFF

:parse_args
if /i "%1"=="debug"   ( set "CONFIG=Debug"   & shift & goto parse_args )
if /i "%1"=="release" ( set "CONFIG=Release" & shift & goto parse_args )
if    "%1"=="--cxx23" ( set "CXX23=ON"       & shift & goto parse_args )
if    "%1"=="--tests" ( set "BUILD_TESTS=ON" & shift & goto parse_args )
if    "%1"=="--clean" (
    echo [clean] Removing %BUILD_DIR%\ ...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
    shift & goto parse_args
)
if not "%1"=="" (
    echo [error] Unknown argument: %1
    exit /b 1
)

:: --- Configure ---------------------------------------------------------------
echo [cmake] Configuring  config=%CONFIG%  cxx23=%CXX23%  tests=%BUILD_TESTS%
echo.

cmake -S . -B "%BUILD_DIR%" ^
    -DCMAKE_BUILD_TYPE=%CONFIG% ^
    -DZERO_USE_CXX23=%CXX23% ^
    -DZERO_BUILD_TESTS=%BUILD_TESTS%

if !errorlevel! neq 0 (
    echo.
    echo [error] CMake configure failed.
    exit /b 1
)

:: --- Build -------------------------------------------------------------------
echo.
echo [cmake] Building...

cmake --build "%BUILD_DIR%" --config %CONFIG%

if !errorlevel! neq 0 (
    echo.
    echo [error] Build failed.
    exit /b 1
)

:: --- Test --------------------------------------------------------------------
if not "%BUILD_TESTS%"=="ON" goto done

echo.
echo [ctest] Running tests...
ctest --test-dir "%BUILD_DIR%" -C %CONFIG% --output-on-failure
if !errorlevel! neq 0 (
    echo.
    echo [error] Tests failed.
    exit /b 1
)

:done
echo.
echo [done] %CONFIG% build complete  ( %BUILD_DIR%\ )
endlocal
