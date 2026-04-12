@echo off
setlocal enabledelayedexpansion

if "%1"=="build" (
    echo Building Python Module...
    if not exist Build mkdir Build
    cd Build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release
    cd ..
) else if "%1"=="clean" (
    echo Cleaning...
    if exist Build (
        rmdir /s /q Build
    )
) else (
    echo Invalid argument. Use build or clean.
    exit /b 1
)
echo Done.
exit /b 0