set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(presenter_lib)

target_sources(presenter_lib
    PRIVATE
        src/simulator_presenter.cpp
        include/simulator_presenter.hpp
)

target_include_directories(presenter_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/model/include
)
