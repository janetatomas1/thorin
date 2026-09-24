
#include <thorin/thorin.hpp>
#include <thorin/dropdown.hpp>
#include <iostream>

using namespace thorin;
using namespace literals;

struct DropdownExample: Widget {
    Dropdown dropdown = Dropdown("", this);

    DropdownExample(const std::string& title): Widget(title) {
        center();

        dropdown
        .width(20_pcts)
        .height(10_pcts);

        dropdown.set_flags(ImGuiComboFlags_HeightLarge);
        dropdown.set_placeholder("Choose a color...");

        dropdown.add_option("Red");
        dropdown.add_option("Green");
        dropdown.add_option("Blue");
        dropdown.add_option("Yellow");
        dropdown.add_option("Purple");

        dropdown.set_on_change([this](int index) {
            std::cout << "Selected: " << dropdown.at(index) << std::endl;
        });
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<DropdownExample>(std::string("Dropdown Example"));
    return app.exec();
}
