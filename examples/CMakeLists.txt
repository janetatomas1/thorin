
set(
    THORIN_EXAMPLE_NAMES
    helloworld
    button
    checkbox
    dropdown
    radiogroup
    textinput
    text
    drag
    slider
    input
    coloredit
    colorpicker
    menubar
    table
    tabwidget
    childwindow
    selectable
    modal
    popup
    progressbar
)

foreach(example_name ${THORIN_EXAMPLE_NAMES})
    add_executable(${example_name} ${THORIN_EXAMPLES_DIR}/${example_name}.cpp)
    target_link_libraries(${example_name} PRIVATE thorin)
endforeach()
