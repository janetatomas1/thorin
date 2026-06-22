
cmake_minimum_required(VERSION 3.28)
project(thorin)

set(CMAKE_CXX_STANDARD 23)

set(THORIN_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/thorin)
set(THORIN_HEADER_DIR ${CMAKE_SOURCE_DIR}/include/)
set(THORIN_EXAMPLES_DIR ${CMAKE_SOURCE_DIR}/examples)
set(
    THORIN_SOURCES
    ${THORIN_SOURCE_DIR}/thorin.cpp
)

add_library(
    thorin
    STATIC
    ${THORIN_SOURCES}
)

target_include_directories(
    thorin
    PUBLIC
    ${THORIN_HEADER_DIR}
)

add_subdirectory(${THORIN_EXAMPLES_DIR})
