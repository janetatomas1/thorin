
#include <thorin/thorin.hpp>
#include <thorin/button.hpp>
#include <iostream>

using namespace thorin;
using namespace literals;

struct HelloWidget: Widget {
    Button button = Button("Hey, this is neato !!!", [this]() {
        std::cout << title_id() << std::endl;
    }, this);

    HelloWidget(const std::string& title): Widget(title) {
        center();
        button
        .set_width(10_pcts)
        .set_height(10_pcts);
    };

    bool show() override {
        return button.render();
    }
};
    
int main(const int argc, char **argv) {
    auto app = Thorin(argc, argv);
    app.add_window<HelloWidget>(std::string("Hello, Thorin"));
    return app.exec();
}
