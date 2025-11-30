cmake_minimum_required(VERSION 3.20)
project(MyVectorProject)

set(CMAKE_CXX_STANDARD 20)
enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip  
)
FetchContent_MakeAvailable(googletest)


add_library(myvector INTERFACE)

target_include_directories(myvector INTERFACE include)


add_executable(test_vector 
    include/vector/tests/tests_vector.cpp
)

target_link_libraries(test_vector PRIVATE myvector gtest_main)

include(GoogleTest)
gtest_discover_tests(test_vector)


target_compile_options(myvector INTERFACE --coverage -O0 -g)
target_compile_options(test_vector PRIVATE --coverage -O0 -g)
target_link_options(test_vector PRIVATE --coverage)