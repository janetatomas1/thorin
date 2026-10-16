find_package(GTest REQUIRED)
include(GoogleTest)

# One executable per test file, each registered with CTest. Add new test files to this list.
# Headless: no window or GL context, only an ImGui context with a font atlas.
set(
    THORIN_TESTS
    measure_test
)

foreach(test ${THORIN_TESTS})
    add_executable(${test} main.cpp ${test}.cpp)
    target_link_libraries(${test} PRIVATE thorin gtest::gtest)
    gtest_discover_tests(${test})
endforeach()
