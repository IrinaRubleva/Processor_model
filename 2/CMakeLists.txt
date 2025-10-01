cmake_minimum_required(VERSION 3.14)
project(BinarySignalProject)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")

add_compile_options(-std=c++17 -finput-charset=UTF-8 -fexec-charset=UTF-8)

find_package(GTest REQUIRED)

add_library(signalState src/signalState.cpp include/signalState.h)
add_library(binarySignal src/binarySignal.cpp include/binarySignal.h)


target_include_directories(signalState PUBLIC include)
target_include_directories(binarySignal PUBLIC include)


add_subdirectory(tests)