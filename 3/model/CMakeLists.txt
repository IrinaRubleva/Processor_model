set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Создаем основную библиотеку проекта
add_library(model_core)

target_sources(model_core
    PRIVATE
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

target_include_directories(model_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Флаги для покрытия кода для библиотеки
target_compile_options(model_core PRIVATE --coverage -O0 -g)
target_link_options(model_core PRIVATE --coverage)

# Создаем тестовый исполняемый файл
add_executable(test_prog
    test/test_model.cpp
)

# Флаги для покрытия кода для тестов
target_compile_options(test_prog PRIVATE --coverage -O0 -g)
target_link_options(test_prog PRIVATE --coverage)

# Линкуем тесты с библиотекой и gtest
target_link_libraries(test_prog
    PRIVATE
        model_core
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_prog)


find_program(GCOVR gcovr)
if(GCOVR)
    # Определяем корневую директорию проекта
    get_filename_component(PROJECT_ROOT "${CMAKE_SOURCE_DIR}" ABSOLUTE)
    
    # Создаем список фильтров для ваших исходных директорий
    set(MY_SOURCE_FILTERS
        "${PROJECT_ROOT}/src/.*"
        "${PROJECT_ROOT}/include/.*"
    )
    
    add_custom_target(coverage_quick
        COMMAND ${GCOVR} -r ${CMAKE_SOURCE_DIR}
                # Просто исключаем все ненужное, остальное автоматически включится
                --exclude=".*/usr/.*"
                --exclude=".*/googletest.*"
                --exclude=".*/gtest.*"
                --exclude=".*/build/.*"
                --exclude=".*/include/vector/build/.*"
                --exclude=".*/test/.*"
                --print-summary
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS test_prog
        COMMENT "Quick coverage summary"
    )
     add_custom_target(coverage_html
        COMMAND echo "=== Generating HTML coverage report ==="
        COMMAND ${GCOVR} -r ${CMAKE_SOURCE_DIR}
                --exclude=".*/usr/.*"
                --exclude=".*/build/.*"
                --exclude=".*/test/.*"
                --exclude=".*/include/vector/build/.*"
                --html --html-details -o ${CMAKE_BINARY_DIR}/coverage.html
                --exclude-unreachable-branches
                --exclude-throw-branches
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS test_prog
        COMMENT "Generating HTML coverage report"
    )
    
    # Алиас coverage для удобства (вызывает coverage_html)
    add_custom_target(coverage
        COMMAND ${CMAKE_MAKE_PROGRAM} coverage_html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Building coverage HTML report"
    )
endif()