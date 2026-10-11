set(
    THORIN_EXAMPLE_NAMES
    zion
    chart
    chart3d
)

foreach(example_name ${THORIN_EXAMPLE_NAMES})
    add_executable(${example_name} ${THORIN_EXAMPLES_DIR}/${example_name}.cpp)
    target_link_libraries(${example_name} PRIVATE thorin)
    # Examples load their images from the source tree, wherever they are run from.
    target_compile_definitions(${example_name} PRIVATE THORIN_EXAMPLES_ASSETS="${THORIN_EXAMPLES_DIR}/assets")
endforeach()
