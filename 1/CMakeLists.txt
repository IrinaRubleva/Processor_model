cmake_minimum_required(VERSION 3.10)
project(Lab1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_COMPILER "/usr/bin/g++-13")

set(SOURCES
    src/dialog.cpp
    src/dictionary.cpp
)

set(HEADERS
    include/dialog.h
    include/dictionary.h
)

add_library(dictlib
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(dictlib
    PUBLIC include
)

add_executable(lab1 src/main.cpp)

target_link_libraries(lab1 PRIVATE dictlib)