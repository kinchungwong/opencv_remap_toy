# This CMakeLists.txt is adapted and minimized from the following:
#    https://github.com/mortinger91/cpp-cmake-template/blob/master/projectFolder/CMakeLists.txt
# 

cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(
    "opencv_remap_toy"
    DESCRIPTION "Idea exploration for alternative ways to implement opencv remap function"
    HOMEPAGE_URL "https://github.com/kinchungwong/opencv_remap_toy"
)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
# Clang compile commands are generated here:
# build/Debug/compile_commands.json
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
endif()

# Dependencies
find_package(OpenCV REQUIRED PATHS "${CMAKE_SOURCE_DIR}/opencv_build" NO_DEFAULT_PATH)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/int")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/lib")

add_executable(${PROJECT_NAME} src/main.cpp)
add_subdirectory(src)
target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        ${OpenCV_LIBS}
    PRIVATE
        ${PROJECT_NAME}_LIB
)

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
