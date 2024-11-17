#!/bin/bash

if [ "$1" == "clean" ]; then
  rm -rf build
  exit 0
fi

mkdir -p build
cd build
cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. --fresh

ninja

# Run tests if requested or if no specific target
if [ "$1" = "test" ] || [ -z "$1" ]; then
    # Run tests with color output and show detailed results
    GTEST_COLOR=1 ctest --output-on-failure --verbose
fi

cd ..        