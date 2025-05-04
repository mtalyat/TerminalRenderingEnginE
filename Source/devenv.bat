@echo off

if "%1"=="debug" (
    echo Building Debug...
    if not exist Build mkdir Build
    cd Build
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    cmake --build . --config Debug
    cd ..
) else if "%1"=="release" (
    echo Building Release...
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
    echo Invalid argument. Use debug, release, or clean.
    exit /b 1
)
echo Done.
exit /b 0