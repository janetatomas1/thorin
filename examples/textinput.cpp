
#include <thorin/thorin.hpp>
#include <thorin/textinput.hpp>
#include <iostream>

using namespace thorin;
using namespace literals;

struct TextInputExample: Widget {
    TextInput input = TextInput("", this);

    TextInputExample(const std::string& title): Widget(title) {
        center();

        input.set_width(30_pcts);
        input
        .set_hint("Type here...")
        .set_on_change([](const std::string& value) {
            std::cout << "Value: " << value << std::endl;
        });
    }

    bool show() override {
        return input.render();
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<TextInputExample>(std::string("TextInput Example"));
    return app.exec();
}
