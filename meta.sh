#!/bin/bash

if [ "$1" == "clean" ]; then
  rm -rf build
  exit 0
fi

if [ ! -d "build" ]; then
    mkdir -p build
fi

mkdir -p build
cd build
cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
ninja

# Run tests if requested or if no specific target
if [ "$1" = "test" ] || [ -z "$1" ]; then
    # Run tests with color output and show detailed results
    ctest --output-on-failure --verbose
fi

cd ..        