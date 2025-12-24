cmake_minimum_required(VERSION 3.16)
project(cpu_simulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")


add_library(model_core
    src/device/bus.cpp
    src/command/binaryCommand.cpp
    src/command/unaryCommand.cpp
    src/command/dataDeclaration.cpp
    src/command/jumpCommand.cpp
    src/command/threadEnd.cpp
    src/command/threadInit.cpp
    src/controleUnit.cpp
    src/decoder.cpp
    src/device/memoryDevice.cpp
    src/device/alu.cpp
    src/processor.cpp
    src/loader.cpp
    src/assembler.cpp
)

target_include_directories(model_core PUBLIC include)


add_executable(test_prog
    test/test_alu.cpp        
    test/test_command.cpp       
    test/test_coverage.cpp          
    test/test_integration.cpp   
    test/test_model.cpp             
    test/test_small_commands.cpp      
    test/test_unary_command.cpp
    test/test_assembler.cpp  
    test/test_common.hpp        
    test/test_data_declaration.cpp  
    test/test_jump_command.cpp  
    test/test_processor.cpp         
    test/test_threadInit_command.cpp  
    test/tets_binary_command.cpp
    test/test_bus.cpp        
    test/test_controleUnit.cpp  
    test/test_decoder.cpp           
    test/test_memoryDevice.cpp  
    test/test_processor_loader.cpp  
    test/test_thread_command.cpp

)

target_include_directories(test_prog PUBLIC
    ${CMAKE_SOURCE_DIR}/include 
)

target_link_libraries(test_prog model_core GTest::gtest_main)


add_custom_target(coverage
    COMMAND echo "=== Запуск тестов с покрытием ==="
    COMMAND cd ${CMAKE_BINARY_DIR} && ./test_prog
    COMMAND echo "=== Генерация отчета ==="
    #COMMAND cd ${CMAKE_BINARY_DIR} && gcovr -r ${CMAKE_SOURCE_DIR} --exclude=".*/test/.*" --exclude=".*/googletest/.*" --html-details -o coverage.html
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Run tests and generate coverage report"
)


add_custom_target(coverage_text
    COMMAND echo "=== Текстовый отчет о покрытии ==="
    COMMAND cd ${CMAKE_BINARY_DIR} && ./test_prog
    COMMAND cd ${CMAKE_BINARY_DIR} && gcovr -r ${CMAKE_SOURCE_DIR} --exclude=".*/test/.*" --exclude=".*/googletest/.*" --print-summary
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generate text coverage report"
)

# ========== НАСТРОЙКА CTest ==========


enable_testing()


include(GoogleTest)


set(CTEST_OUTPUT_ON_FAILURE ON)


gtest_discover_tests(test_prog
    EXTRA_ARGS "--gtest_output=xml:${CMAKE_BINARY_DIR}/test_results.xml"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)


add_test(NAME AllTests
    COMMAND ${CMAKE_BINARY_DIR}/test_prog
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_test(NAME AssemblerTests
    COMMAND ${CMAKE_BINARY_DIR}/test_prog --gtest_filter="AssemblerTest*"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_test(NAME ProcessorTests
    COMMAND ${CMAKE_BINARY_DIR}/test_prog --gtest_filter="ProcessorTest*"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_test(NAME DecoderTests
    COMMAND ${CMAKE_BINARY_DIR}/test_prog --gtest_filter="DecoderTest*"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)


set_tests_properties(
    AssemblerTests ProcessorTests DecoderTests
    PROPERTIES LABELS "unit"
)
set_tests_properties(
    AllTests
    PROPERTIES LABELS "integration"
)