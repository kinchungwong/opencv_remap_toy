#!/bin/bash

set -e
set -x

CMAKE_OPTS=""
# CMAKE_OPTS="--trace-expand"
CMAKE_OPTS="${CMAKE_OPTS} -DCMAKE_BUILD_TYPE=Release"
CXXFLAGS="-std=c++17 -march=native -mtune=native -Og -O3 -g3 -ggdb -Wextra -Wpedantic"

test -d "./opencv_build"

(
    mkdir -p ./build
    rm -rf ./build/*
    cd build
    CXXFLAGS=${CXXFLAGS} cmake ${CMAKE_OPTS} -B . -S ..
    make -j $(nproc)
)

(
    DISASM_BINARY_FILE=./build/bin/int/libopencv_remap_toy_LIB.a
    # DISASM_BINARY_FILE=./build/bin/opencv_remap_toy
    DISASM_OUTPUT_FILE=./build/disasm.txt
    objdump -d -C -M intel "${DISASM_BINARY_FILE}" > "${DISASM_OUTPUT_FILE}"
)
