#!/bin/bash

set -e
set -x

CMAKE_OPTS=""
# CMAKE_OPTS="--trace-expand"
CMAKE_OPTS="${CMAKE_OPTS} -DCMAKE_BUILD_TYPE=Release"

test -d "./opencv_build"

(
    mkdir -p ./build
    rm -rf ./build/*
    cd build
    cmake ${CMAKE_OPTS} -B . -S ..
    make -j $(nproc)
)
