
add_executable(
    helloworld
    ${THORIN_EXAMPLES_DIR}/helloworld.cpp
)

target_link_libraries(
    helloworld
    PRIVATE
    thorin
)