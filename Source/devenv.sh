#!/bin/bash

if [ "$1" == "debug" ]; then
    echo "Building Debug..."
    mkdir -p Build
    cd Build
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    cmake --build . --config Debug
    cd ..
elif [ "$1" == "release" ]; then
    echo "Building Release..."
    mkdir -p Build
    cd Build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release
    cd ..
elif [ "$1" == "clean" ]; then
    echo "Cleaning..."
    if [ -d "Build" ]; then
        rm -rf Build
    fi
else
    echo "Invalid argument. Use debug, release, or clean."
    exit 1
fi

echo "Done."
exit 0