
cmake_minimum_required(VERSION 3.28)
project(thorin)

set(CMAKE_CXX_STANDARD 23)

set(THORIN_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/thorin)
set(THORIN_HEADER_DIR ${CMAKE_SOURCE_DIR}/include/)
set(THORIN_EXAMPLES_DIR ${CMAKE_SOURCE_DIR}/examples)
set(
    THORIN_SOURCES
    ${THORIN_SOURCE_DIR}/thorin.cpp
    ${THORIN_SOURCE_DIR}/windowmanager.cpp
    ${THORIN_SOURCE_DIR}/glwindow.cpp
    ${THORIN_SOURCE_DIR}/actionmanager.cpp
    ${THORIN_SOURCE_DIR}/window.cpp
    ${THORIN_SOURCE_DIR}/widget.cpp
)

find_package(sdl REQUIRED)
find_package(glbinding REQUIRED)
find_package(concurrentqueue REQUIRED)
find_package(imgui REQUIRED)
find_package(function2 REQUIRED)
find_package(yoga REQUIRED)

add_library(
    thorin
    STATIC
    ${THORIN_SOURCES}
)

target_link_libraries(
    thorin
    PUBLIC
    sdl::sdl
    glbinding::glbinding
    concurrentqueue::concurrentqueue
    imgui::imgui
    function2::function2
    yoga::yoga
)
target_include_directories(
    thorin
    PUBLIC
    ${THORIN_HEADER_DIR}
)

add_subdirectory(${THORIN_EXAMPLES_DIR})
