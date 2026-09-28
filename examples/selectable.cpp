#include <thorin/thorin.hpp>
#include <thorin/selectable.hpp>
#include <thorin/text.hpp>
#include <iostream>
#include <string>

using namespace thorin;
using namespace literals;

// Click selects one row, ctrl+click toggles, shift+click selects a range, ctrl+A selects all.
struct SelectableExample: Widget {
    Text selection = Text("Selected: none", this);
    Selectable list = Selectable([this](std::vector<size_t> indices) {
        std::string text = "Selected:";
        for (size_t index : indices) {
            text += " " + list.label(index);
        }

        std::cout << text << std::endl;
        selection.set_title(indices.empty() ? "Selected: none" : text);
    }, this);

    SelectableExample(const std::string& title): Widget(title) {
        column(10);
        padding(10);

        list.add_item("Apple")
            .add_item("Banana")
            .add_item("Cherry")
            .add_item("Date")
            .add_item("Elderberry");
        list.width(200);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<SelectableExample>(std::string("Selectable Example"));
    return app.exec();
}
