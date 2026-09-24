find_package(GTest REQUIRED)

# One executable for all tests. Add new test files to this list.
# Headless: no window or GL context, only an ImGui context with a font atlas.
add_executable(
    thorin_tests
    main.cpp
    measure_test.cpp
)
target_link_libraries(thorin_tests PRIVATE thorin GTest::gtest)
