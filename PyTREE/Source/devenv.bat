@echo off
setlocal enabledelayedexpansion

pushd %~dp0

if "%1"=="build" (
    echo Building...
    pip install -r requirements.txt
    if not exist Build mkdir Build
    pushd Build
    python ../setup.py build
    popd ..
) else if "%1"=="clean" (
    echo Cleaning...
    if exist Build (
        rmdir /s /q Build
    )
) else (
    echo Invalid argument. Use build or clean.
    popd
    exit /b 1
)

echo Done.
popd
exit /b 0