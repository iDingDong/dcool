cmake_minimum_required(VERSION 3.18)

project(dcool VERSION 0.1.0 LANGUAGES "CXX")

set(DCOOL_ROOT_DIRECTORY "${PROJECT_SOURCE_DIR}")
set(DCOOL_INCLUDE_DIRECTORY "${DCOOL_ROOT_DIRECTORY}/include")
set(DCOOL_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/binary")

add_subdirectory("./source")
add_subdirectory("./test")
