#!/bin/bash

if [ "$1" == "clean" ]; then
  rm -rf build
  exit 0
fi

mkdir -p build
cd build

# Default debug audio to OFF
debug_audio="OFF"
build_audio="ON"
build_ipc="ON"
tests="ON"
integration="ON"


# Parse flags
for arg in "$@"; do
    case $arg in
        --debug-audio)
            debug_audio="ON"
            ;;
        --no-audio)
            build_audio="OFF"
            ;;
        --no-ipc)
            build_ipc="OFF"
            ;;

        --no-tests)
            tests="OFF"
            ;;
    esac
done

cmake -G Ninja \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DENABLE_AUDIO_DEBUG=${debug_audio} \
    -DBUILD_AUDIO=${build_audio} \
    -DBUILD_IPC=${build_ipc} \
    -DENABLE_TESTING=${tests} \
    -DENABLE_INTEGRATION_TESTING=${integration} \
    .. --fresh

ninja

# Run tests if requested or if no specific target
if [ "$tests" = "ON" ] && [ "$1" = "test" ]; then
    if [ ! -z "$2" ]; then 
        if [ "$2" == "integration" ]; then
           GTEST_COLOR=1 GTEST_FILTER="*integration*" ctest --output-on-failure --verbose
        else
            GTEST_COLOR=1 GTEST_FILTER="*$2*" ctest --output-on-failure --verbose
        fi
    else
        GTEST_COLOR=1 ctest --output-on-failure --verbose
    fi
fi

cd ..        