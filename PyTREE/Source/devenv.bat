@echo off
setlocal enabledelayedexpansion

pushd %~dp0

if "%1"=="build" (
    set "TREE_LIB=..\..\TREE\Source\Build\Release\TREE.lib"
    if not exist "!TREE_LIB!" (
        echo TREE.lib not found. Building TREE Release dependency...
        pushd ..\..\TREE\Source
        call devenv.bat build release
        set "TREE_BUILD_EXIT=!errorlevel!"
        popd
        if not "!TREE_BUILD_EXIT!"=="0" (
            echo Failed to build TREE dependency.
            popd
            exit /b 1
        )
        if not exist "!TREE_LIB!" (
            echo TREE dependency build completed but TREE.lib was not generated.
            popd
            exit /b 1
        )
    )

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