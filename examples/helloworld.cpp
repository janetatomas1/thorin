
#include <thorin/thorin.hpp>
#include <iostream>

using namespace thorin;
using namespace literals;

struct Button: public Widget {
    bool show() override {
        return ImGui::Button("Hello, Thorin", size());
    }

    Button(Widget* parent) : Widget("", parent) {
        set_width(10_pcts)
        .set_height(15_pcts);
    }
};

struct HelloWidget: public Widget {
    Button button = Button(this);
    HelloWidget(const std::string& title): Widget(title) {};

    bool show() override {
        center();
        return button.render();
    }
};
    
int main(const int argc, char **argv) {
    auto app = Thorin(argc, argv);
    app.add_window<HelloWidget>(std::string("Hello, Thorin"));
    return app.exec();
}
