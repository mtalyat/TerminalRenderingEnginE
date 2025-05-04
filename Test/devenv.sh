#!/bin/bash

if [ "$1" == "build" ]; then
    if [ "$2" == "debug" ]; then
        BUILD_TYPE="Debug"
    elif [ "$2" == "release" ]; then
        BUILD_TYPE="Release"
    else
        echo "Invalid argument. Use debug or release."
        exit 1
    fi
    echo "Building $BUILD_TYPE..."
    mkdir -p Build
    cd Build
    cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE
    cmake --build . --config $BUILD_TYPE
    cd ..
elif [ "$1" == "clean" ]; then
    echo "Cleaning..."
    if [ -d "Build" ]; then
        rm -rf Build
    fi
elif [ "$1" == "run" ]; then
    if [ -d "Build" ]; then
        echo "Running application..."
        cd Build
        ./Test
        cd ..
    else
        echo "Build directory does not exist. Please build the project first."
        exit 1
    fi
else
    echo "Invalid argument. Use debug, release, or clean."
    exit 1
fi

echo "Done."
exit 0