cmake_minimum_required(VERSION 3.10)
project(Lab1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_COMPILER "/usr/bin/g++-13")

add_executable(lab1
    src/main.cpp
    src/dialog.cpp
    src/dictionary.cpp
)

target_include_directories(lab1 PRIVATE include)