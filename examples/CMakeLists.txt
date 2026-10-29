# Each example picks its backend in code and is built when that backend is. Every built backend is linked,
# so switching an example's backend needs no change here, only the condition below.
set(THORIN_EXAMPLE_NAMES)
set(THORIN_EXAMPLE_BACKENDS)
if(THORIN_WEBGPU)
    list(APPEND THORIN_EXAMPLE_NAMES chart)
    list(APPEND THORIN_EXAMPLE_BACKENDS thorin::webgpu)
endif()
if(THORIN_OPENGL)
    list(APPEND THORIN_EXAMPLE_NAMES chart3d)
    list(APPEND THORIN_EXAMPLE_BACKENDS thorin::opengl)
endif()
if(THORIN_SDLGPU)
    list(APPEND THORIN_EXAMPLE_BACKENDS thorin::sdlgpu)
    # zion reads /proc and /sys, so it only builds on Linux.
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        list(PREPEND THORIN_EXAMPLE_NAMES zion)
    endif()
endif()

# Emscripten defaults to .js; .html also generates a page that loads the .js and .wasm.
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

foreach(example_name ${THORIN_EXAMPLE_NAMES})
    add_executable(${example_name} ${THORIN_EXAMPLES_DIR}/${example_name}.cpp)
    target_link_libraries(${example_name} PRIVATE thorin::extras ${THORIN_EXAMPLE_BACKENDS})
    # Examples load their images from the source tree, wherever they are run from.
    target_compile_definitions(${example_name} PRIVATE THORIN_EXAMPLES_ASSETS="${THORIN_EXAMPLES_DIR}/assets")
endforeach()
