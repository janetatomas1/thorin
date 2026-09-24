
#include <thorin/thorin.hpp>
#include <thorin/checkbox.hpp>
#include <iostream>

using namespace thorin;
using namespace literals;

struct CheckboxExample: Widget {
    Checkbox checkbox = Checkbox("Enable feature", [](bool value) {
        std::cout << "Checkbox changed: " << (value ? "true" : "false") << std::endl;
    }, this);

    CheckboxExample(const std::string& title): Widget(title) {
        center();
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<CheckboxExample>(std::string("Checkbox Example"));
    return app.exec();
}
