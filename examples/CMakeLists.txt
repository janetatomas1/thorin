# Each example is built when its backend is.
set(THORIN_EXAMPLE_NAMES)
if(THORIN_OPENGL)
    list(APPEND THORIN_EXAMPLE_NAMES chart chart3d)
    set(chart_BACKEND thorin::opengl)
    set(chart3d_BACKEND thorin::opengl)
endif()

# zion reads /proc and /sys, so it only builds on Linux.
if(THORIN_SDLGPU AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(PREPEND THORIN_EXAMPLE_NAMES zion)
    set(zion_BACKEND thorin::sdlgpu)
endif()

# Emscripten defaults to .js; .html also generates a page that loads the .js and .wasm.
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

foreach(example_name ${THORIN_EXAMPLE_NAMES})
    add_executable(${example_name} ${THORIN_EXAMPLES_DIR}/${example_name}.cpp)
    target_link_libraries(${example_name} PRIVATE thorin::extras ${${example_name}_BACKEND})
    # Examples load their images from the source tree, wherever they are run from.
    target_compile_definitions(${example_name} PRIVATE THORIN_EXAMPLES_ASSETS="${THORIN_EXAMPLES_DIR}/assets")
endforeach()
