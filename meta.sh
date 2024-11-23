#!/bin/bash

if [ "$1" == "clean" ]; then
  rm -rf build
  exit 0
fi

mkdir -p build
cd build

# Default debug audio to OFF
debug_audio="OFF"

# Check for --debug-audio flag
for arg in "$@"; do
    if [ "$arg" == "--debug-audio" ]; then
        debug_audio="ON"
    fi
done

cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DENABLE_AUDIO_DEBUG=${debug_audio} .. --fresh

ninja

# Run tests if requested or if no specific target
if [ "$1" = "test" ] || [ -z "$1" ]; then
    # Run tests with color output and show detailed results
    GTEST_COLOR=1 ctest --output-on-failure --verbose
fi

cd ..        