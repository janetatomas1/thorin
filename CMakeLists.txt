
cmake_minimum_required(VERSION 3.28)
project(thorin VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)

set(THORIN_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/thorin)
set(THORIN_HEADER_DIR ${CMAKE_SOURCE_DIR}/include/)
set(THORIN_EXAMPLES_DIR ${CMAKE_SOURCE_DIR}/examples)
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
    ${THORIN_SOURCE_DIR}/gl/glbackend.cpp
    ${THORIN_SOURCE_DIR}/gl/renderwidget.cpp
    ${THORIN_SOURCE_DIR}/gl/imagebutton.cpp
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
find_package(glbinding REQUIRED)
find_package(globjects REQUIRED)
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

target_link_libraries(
    thorin
    PUBLIC
    sdl::sdl
    glbinding::glbinding
    globjects::globjects
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
        set(input ${CMAKE_SOURCE_DIR}/fonts/${file})
        set(output ${CMAKE_CURRENT_BINARY_DIR}/fonts/${name}.cpp)
        add_custom_command(
            OUTPUT ${output}
            COMMAND ${CMAKE_COMMAND} -DINPUT=${input} -DOUTPUT=${output} -DNAME=${name}
                -P ${CMAKE_SOURCE_DIR}/cmake/embed.cmake
            DEPENDS ${input} ${CMAKE_SOURCE_DIR}/cmake/embed.cmake
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
    ${THORIN_HEADER_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/include
)
add_subdirectory(${THORIN_EXAMPLES_DIR})

option(THORIN_BUILD_TESTS "Build the headless tests" ON)
if(THORIN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
endif()
