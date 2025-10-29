
add_executable(run_tests
    test_signalState.cpp
    test_binarySignal.cpp
    test.cpp
)


target_include_directories(run_tests PRIVATE ../include)


target_link_libraries(run_tests
    signalState           
    binarySignal          
    GTest::gtest_main
)


enable_testing()
add_test(NAME AllTests COMMAND run_tests)