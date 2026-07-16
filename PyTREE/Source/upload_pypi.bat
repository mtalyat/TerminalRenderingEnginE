@echo off
setlocal EnableExtensions

set "REPOSITORY=pypi"
set "SKIP_BUILD=0"
set "DRY_RUN=0"

:parse_args
if "%~1"=="" goto args_done
if /I "%~1"=="--repository" (
    if "%~2"=="" (
        echo ERROR: --repository requires a value ^(pypi^|testpypi^)
        exit /b 1
    )
    set "REPOSITORY=%~2"
    shift
    shift
    goto parse_args
)
if /I "%~1"=="--skip-build" (
    set "SKIP_BUILD=1"
    shift
    goto parse_args
)
if /I "%~1"=="--dry-run" (
    set "DRY_RUN=1"
    shift
    goto parse_args
)
if /I "%~1"=="--help" goto usage
if /I "%~1"=="-h" goto usage

echo ERROR: Unknown argument: %~1
exit /b 1

:args_done
if /I not "%REPOSITORY%"=="pypi" if /I not "%REPOSITORY%"=="testpypi" (
    echo ERROR: --repository must be pypi or testpypi
    exit /b 1
)

echo ==^> Working directory: %~dp0
cd /d "%~dp0" || exit /b 1

echo ==^> Ensuring build tools are installed
python -m pip install --upgrade pip build twine || exit /b 1

if "%SKIP_BUILD%"=="0" (
    echo ==^> Cleaning dist\
    if exist dist rmdir /s /q dist

    echo ==^> Building sdist and wheel
    python -m build || exit /b 1
)

if not exist dist (
    echo ERROR: dist\ does not exist. Build first or remove --skip-build.
    exit /b 1
)

echo ==^> Validating artifacts with twine
python -m twine check dist/* || exit /b 1

if "%DRY_RUN%"=="1" (
    echo ==^> Dry run requested, stopping before upload
    exit /b 0
)

if /I "%REPOSITORY%"=="pypi" (
    if not defined PYPI_TOKEN if not defined TWINE_PASSWORD (
        echo ERROR: Missing PYPI_TOKEN environment variable.
        echo        You can also set TWINE_PASSWORD.
        exit /b 1
    )
    if not defined TWINE_USERNAME set "TWINE_USERNAME=__token__"
    if not defined TWINE_PASSWORD set "TWINE_PASSWORD=%PYPI_TOKEN%"
    echo ==^> Uploading to PyPI
    python -m twine upload dist/* || exit /b 1
) else (
    if not defined TEST_PYPI_TOKEN if not defined TWINE_PASSWORD (
        echo ERROR: Missing TEST_PYPI_TOKEN environment variable.
        echo        You can also set TWINE_PASSWORD.
        exit /b 1
    )
    if not defined TWINE_USERNAME set "TWINE_USERNAME=__token__"
    if not defined TWINE_PASSWORD set "TWINE_PASSWORD=%TEST_PYPI_TOKEN%"
    echo ==^> Uploading to TestPyPI
    python -m twine upload --repository testpypi dist/* || exit /b 1
)

echo ==^> Upload completed
exit /b 0

:usage
echo Usage:
echo   upload_pypi.bat [--repository pypi^|testpypi] [--skip-build] [--dry-run]
echo.
echo Examples:
echo   upload_pypi.bat --dry-run
echo   upload_pypi.bat --repository testpypi
echo   upload_pypi.bat --repository pypi
exit /b 0
