@echo off
setlocal enabledelayedexpansion

if "%1"=="build" (
    if "%2" == "debug" (
        set "BUILD_TYPE=Debug"
    ) else if "%2" == "release" (
        set "BUILD_TYPE=Release"
    ) else (
        echo Invalid argument. Use debug or release.
        exit /b 1
    )
    echo Building !BUILD_TYPE!...
    if not exist Build mkdir Build
    cd Build
    cmake .. -DCMAKE_BUILD_TYPE=!BUILD_TYPE!
    cmake --build . --config !BUILD_TYPE!
    cd ..
) else if "%1"=="clean" (
    echo Cleaning...
    if exist Build (
        rmdir /s /q Build
    )
) else if "%1"=="run" (
    if exist Build (
        if "%2" == "debug" (
            set "BUILD_TYPE=Debug"
        ) else if "%2" == "release" (
            set "BUILD_TYPE=Release"
        ) else (
            echo Invalid argument. Use debug or release.
            exit /b 1
        )
        cd Build/!BUILD_TYPE!
        start /wait Test.exe
        cd ../..
    ) else (
        echo Build directory does not exist. Please build first.
        exit /b 1
    )
) else (
    echo Invalid argument. Use debug, release, or clean.
    exit /b 1
)
echo Done.
exit /b 0