cmake_minimum_required(VERSION 3.20)
project(CPU_Simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Gui
    Quick
    QuickWidgets
    Qml
    Quick3D          
    ShaderTools      
)


add_subdirectory(model)
add_subdirectory(presenter)
add_subdirectory(view)

add_executable(cpu_simulator_console
    main.cpp
)

target_link_libraries(cpu_simulator_console
    PRIVATE
        model_core
        presenter_lib
)

add_executable(cpu_simulator_gui
    main_qt.cpp
)

target_link_libraries(cpu_simulator_gui
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        Qt6::Quick
        Qt6::QuickWidgets
        Qt6::Qml

        view_lib
        presenter_lib
        model_core
)


target_include_directories(cpu_simulator_gui PRIVATE
    ${CMAKE_SOURCE_DIR}/model/include
    ${CMAKE_SOURCE_DIR}/presenter
    ${CMAKE_SOURCE_DIR}/view
)