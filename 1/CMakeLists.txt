cmake_minimum_required(VERSION 3.10)
project(Lab1)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab1
    src/main.cpp
    src/dialog.cpp
    src/dictionary.cpp
)

target_include_directories(lab1 PRIVATE include)