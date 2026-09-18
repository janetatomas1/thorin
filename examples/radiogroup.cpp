
#include <thorin/thorin.hpp>
#include <thorin/radiogroup.hpp>
#include <iostream>

using namespace thorin;
using namespace literals;

struct RadioGroupExample: Widget {
    RadioGroup group = RadioGroup(this);

    RadioGroupExample(const std::string& title): Widget(title) {
        center();

        group.add_option("Small");
        group.add_option("Medium");
        group.add_option("Large", []() {
            std::cout << "Large selected" << std::endl;
        });
    }

    bool show() override {
        return group.render();
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<RadioGroupExample>(std::string("RadioGroup Example"));
    return app.exec();
}
