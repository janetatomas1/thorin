
#include <thorin/thorin.hpp>
#include <thorin/button.hpp>
#include <iostream>

using namespace thorin;
using namespace literals;

struct ButtonExample: Widget {
    Button button = Button("Click me!", [this]() {
        std::cout << "Button clicked" << std::endl;
    }, this);

    ButtonExample(const std::string& title): Widget(title) {
        center();
        button
        .set_width(20_pcts)
        .set_height(10_pcts);
    }

    bool show() override {
        return button.render();
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<ButtonExample>(std::string("Button Example"));
    return app.exec();
}
