set(
    THORIN_EXAMPLE_NAMES
    chart
    chart3d
)

# zion reads /proc and /sys, so it only builds on Linux.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(PREPEND THORIN_EXAMPLE_NAMES zion)
endif()

foreach(example_name ${THORIN_EXAMPLE_NAMES})
    add_executable(${example_name} ${THORIN_EXAMPLES_DIR}/${example_name}.cpp)
    target_link_libraries(${example_name} PRIVATE thorin)
    # Examples load their images from the source tree, wherever they are run from.
    target_compile_definitions(${example_name} PRIVATE THORIN_EXAMPLES_ASSETS="${THORIN_EXAMPLES_DIR}/assets")
endforeach()
