set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Quick 
    QuickWidgets
    Qml
    OpenGL
    OpenGLWidgets
)

add_library(view_lib
    src/memStateWindow.cpp
    src/helpWindow.cpp
    src/editorWindow.cpp
    src/highlighter.cpp
    src/glwidget.cpp
    src/mainWindow.cpp

    include/memStateWindow.h
    include/helpWindow.h
    include/editorWindow.h
    include/highlighter.h
    include/glwidget.h
    include/mainWindow.h
)

target_link_libraries(view_lib
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Quick
        Qt6::QuickWidgets
        Qt6::Qml
        Qt6::OpenGL
        Qt6::OpenGLWidgets
)

target_include_directories(view_lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

qt_add_resources(view_lib "qml_files"
    PREFIX "/"
    FILES
        cpu_texture.png
)
