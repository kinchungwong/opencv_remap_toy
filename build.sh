#!/bin/bash

set -e
set -x

CMAKE_OPTS=""
# CMAKE_OPTS="--trace-expand"

test -d "./opencv_build"

(
    mkdir -p ./build
    rm -rf ./build/*
    cd build
    cmake ${CMAKE_OPTS} -B . -S ..
    make -j $(nproc)
)
