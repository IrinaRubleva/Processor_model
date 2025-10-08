cmake_minimum_required(VERSION 3.14)
project(BinarySignalProject)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")

add_compile_options(-std=c++17 -finput-charset=UTF-8 -fexec-charset=UTF-8)

find_package(GTest REQUIRED)

add_library(signalState src/signalState.cpp include/signalState.h)
add_library(binarySignal src/binarySignal.cpp include/binarySignal.h)


target_include_directories(signalState PUBLIC include)
target_include_directories(binarySignal PUBLIC include)


add_subdirectory(tests)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_program(GCOVR gcovr)
    if(GCOVR)
        add_custom_target(coverage
            COMMAND ${GCOVR} -r ${CMAKE_SOURCE_DIR} --exclude tests/ --html --html-details -o ${CMAKE_BINARY_DIR}/coverage_report/index.html
            COMMAND echo "Coverage report generated: ${CMAKE_BINARY_DIR}/coverage_report/index.html"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS run_tests
        )
    endif()
endif()