cmake_minimum_required(VERSION 3.5)

project(MonotoneCubicSpline VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*[.cpp|.hpp])

add_library(${PROJECT_NAME} ${SOURCES})

source_group("" FILES ${SOURCES})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/example)
