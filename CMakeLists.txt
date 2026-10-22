
cmake_minimum_required(VERSION 3.28)
project(thorin VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 23)

include(GNUInstallDirs)

set(THORIN_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/thorin)
set(THORIN_HEADER_DIR ${PROJECT_SOURCE_DIR}/include/)
set(THORIN_EXAMPLES_DIR ${PROJECT_SOURCE_DIR}/examples)
set(
    THORIN_SOURCES
    ${THORIN_SOURCE_DIR}/thorin.cpp
    ${THORIN_SOURCE_DIR}/theme.cpp
    ${THORIN_SOURCE_DIR}/font.cpp
    ${THORIN_SOURCE_DIR}/image.cpp
    ${THORIN_SOURCE_DIR}/log.cpp
    ${THORIN_SOURCE_DIR}/windowmanager.cpp
    ${THORIN_SOURCE_DIR}/actionmanager.cpp
    ${THORIN_SOURCE_DIR}/window.cpp
    ${THORIN_SOURCE_DIR}/widget.cpp
    ${THORIN_SOURCE_DIR}/layout.cpp
    ${THORIN_SOURCE_DIR}/gpubackend.cpp
    ${THORIN_SOURCE_DIR}/button.cpp
    ${THORIN_SOURCE_DIR}/checkbox.cpp
    ${THORIN_SOURCE_DIR}/radiobutton.cpp
    ${THORIN_SOURCE_DIR}/radiogroup.cpp
    ${THORIN_SOURCE_DIR}/dropdown.cpp
    ${THORIN_SOURCE_DIR}/textinput.cpp
    ${THORIN_SOURCE_DIR}/text.cpp
    ${THORIN_SOURCE_DIR}/colorpicker.cpp
    ${THORIN_SOURCE_DIR}/menu.cpp
    ${THORIN_SOURCE_DIR}/menubar.cpp
    ${THORIN_SOURCE_DIR}/table.cpp
    ${THORIN_SOURCE_DIR}/tabwidget.cpp
    ${THORIN_SOURCE_DIR}/childwindow.cpp
    ${THORIN_SOURCE_DIR}/selectable.cpp
    ${THORIN_SOURCE_DIR}/treenode.cpp
    ${THORIN_SOURCE_DIR}/treeview.cpp
    ${THORIN_SOURCE_DIR}/popup.cpp
    ${THORIN_SOURCE_DIR}/modal.cpp
    ${THORIN_SOURCE_DIR}/progressbar.cpp
    ${THORIN_SOURCE_DIR}/logwidget.cpp
    ${THORIN_SOURCE_DIR}/clipboard.cpp
    ${THORIN_SOURCE_DIR}/separator.cpp
    ${THORIN_SOURCE_DIR}/pane.cpp
    ${THORIN_SOURCE_DIR}/dockarea.cpp
    ${THORIN_SOURCE_DIR}/series.cpp
    ${THORIN_SOURCE_DIR}/chartwidget.cpp
    ${THORIN_SOURCE_DIR}/series3d.cpp
    ${THORIN_SOURCE_DIR}/chart3dwidget.cpp
    ${THORIN_SOURCE_DIR}/style.cpp
    ${THORIN_SOURCE_DIR}/color.cpp
)

find_package(sdl REQUIRED)
find_package(concurrentqueue REQUIRED)
find_package(imgui REQUIRED)
find_package(implot REQUIRED)
find_package(implot3d REQUIRED)
find_package(function2 REQUIRED)
find_package(yoga REQUIRED)
find_package(libassert REQUIRED)
find_package(stb REQUIRED)
find_package(spdlog REQUIRED)

add_library(
    thorin
    STATIC
    ${THORIN_SOURCES}
)
# The name apps link against, the same whether thorin comes from find_package() or add_subdirectory().
add_library(thorin::thorin ALIAS thorin)
# The headers use C++23 (deducing this), so apps need it too.
target_compile_features(thorin PUBLIC cxx_std_23)

target_link_libraries(
    thorin
    PUBLIC
    sdl::sdl
    concurrentqueue::concurrentqueue
    imgui::imgui
    implot::implot
    implot3d::implot3d
    function2::function2
    yoga::yoga
    libassert::assert
    spdlog::spdlog
    PRIVATE
    # Only image.cpp decodes images.
    stb::stb
)
# OpenGL backend and the GL widgets (RenderWidget, ImageWidget, ImageButton). Off leaves no backend.
option(THORIN_GL "Build the OpenGL backend" ON)
if(THORIN_GL)
    find_package(glbinding REQUIRED)
    find_package(globjects REQUIRED)
    target_sources(
        thorin
        PRIVATE
        ${THORIN_SOURCE_DIR}/gl/glbackend.cpp
        ${THORIN_SOURCE_DIR}/gl/renderwidget.cpp
        ${THORIN_SOURCE_DIR}/gl/imagebutton.cpp
    )
    target_link_libraries(thorin PUBLIC glbinding::glbinding globjects::globjects)
    target_compile_definitions(thorin PUBLIC THORIN_GL)
endif()

# Inter, JetBrains Mono and Noto Sans (OFL, see fonts/*/OFL.txt) compiled in as Font::inter() etc.
option(THORIN_BUNDLE_FONTS "Embed the fonts in fonts/ (~1.3 MB)" ON)
if(THORIN_BUNDLE_FONTS)
    foreach(font IN ITEMS
        "inter|inter/Inter-Regular.ttf"
        "jetbrains_mono|jetbrains-mono/JetBrainsMono-Regular.ttf"
        "noto_sans|noto-sans/NotoSans-Regular.ttf"
    )
        string(REPLACE "|" ";" font "${font}")
        list(GET font 0 name)
        list(GET font 1 file)
        set(input ${PROJECT_SOURCE_DIR}/fonts/${file})
        set(output ${CMAKE_CURRENT_BINARY_DIR}/fonts/${name}.cpp)
        add_custom_command(
            OUTPUT ${output}
            COMMAND ${CMAKE_COMMAND} -DINPUT=${input} -DOUTPUT=${output} -DNAME=${name}
                -P ${PROJECT_SOURCE_DIR}/cmake/embed.cmake
            DEPENDS ${input} ${PROJECT_SOURCE_DIR}/cmake/embed.cmake
            COMMENT "Embedding font ${file}"
        )
        target_sources(thorin PRIVATE ${output})
    endforeach()
    target_compile_definitions(thorin PUBLIC THORIN_BUNDLE_FONTS)
endif()

# Widget inspector overlay, opened with F12 or Window::set_inspector(). Turn off for shipped apps.
option(THORIN_INSPECTOR "Build the widget inspector overlay" ON)
if(THORIN_INSPECTOR)
    target_sources(thorin PRIVATE ${THORIN_SOURCE_DIR}/inspector.cpp)
    target_compile_definitions(thorin PRIVATE THORIN_INSPECTOR)
endif()

configure_file(
    ${THORIN_HEADER_DIR}/thorin/version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/thorin/version.hpp
    @ONLY
)
target_include_directories(
    thorin
    PUBLIC
    $<BUILD_INTERFACE:${THORIN_HEADER_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# CMake package: find_package(thorin) and link thorin::thorin. Usable from the build tree (thorin_DIR =
# this build directory) or after cmake --install (CMAKE_PREFIX_PATH = the install prefix). The app has to
# provide the same dependencies, e.g. through the same Conan toolchain.
include(CMakePackageConfigHelpers)
install(TARGETS thorin EXPORT thorinTargets)
if(NOT THORIN_GL)
    set(THORIN_INSTALL_EXCLUDE PATTERN "gl" EXCLUDE)
endif()
install(
    DIRECTORY ${THORIN_HEADER_DIR}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
    ${THORIN_INSTALL_EXCLUDE}
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/thorin/version.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/thorin)
install(EXPORT thorinTargets NAMESPACE thorin:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/thorin)
export(EXPORT thorinTargets NAMESPACE thorin:: FILE ${CMAKE_CURRENT_BINARY_DIR}/thorinTargets.cmake)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/thorinConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/thorinConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/thorin
)
# 0.x: minor versions may break the API, so only the same minor version matches.
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/thorinConfigVersion.cmake
    COMPATIBILITY SameMinorVersion
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/thorinConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/thorinConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/thorin
)
# Examples and tests only build by default when thorin is the top-level project, not when an app pulls
# it in with add_subdirectory().
option(THORIN_BUILD_EXAMPLES "Build the examples" ${PROJECT_IS_TOP_LEVEL})
if(THORIN_BUILD_EXAMPLES)
    add_subdirectory(${THORIN_EXAMPLES_DIR})
endif()

option(THORIN_BUILD_TESTS "Build the headless tests" ${PROJECT_IS_TOP_LEVEL})
if(THORIN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()
