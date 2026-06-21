
cmake_minimum_required(VERSION 3.28)
project(thorin)

set(CMAKE_CXX_STANDARD 23)

set(THORIN_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/thorin)
set(THORIN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/thorin)
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

add_executable(
    helloworld
    ${THORIN_EXAMPLES_DIR}/helloworld.cpp
)
