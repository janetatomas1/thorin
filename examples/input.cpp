#include <thorin/thorin.hpp>
#include <thorin/input.hpp>
#include <array>
#include <initializer_list>
#include <iostream>

using namespace thorin;
using namespace literals;

struct InputExample: Widget {
    InputInt<> count = InputInt<>("Count", this);
    InputInt<2> position = InputInt<2>("Position", this);
    InputFloat<> weight = InputFloat<>("Weight", this);
    InputFloat<3> offset = InputFloat<3>("Offset", this);

    InputExample(const std::string& title): Widget(title) {
        column(8.0f).center();

        for (Widget* w: std::initializer_list<Widget*>{&count, &position, &weight, &offset}) {
            w->width(50_pcts);
        }

        count
        .set_step(5, 50)
        .set_on_change([](int value) {
            std::cout << "Count: " << value << std::endl;
        });

        position
        .set_format("%d px")
        .set_on_change([](const std::array<int, 2>& value) {
            std::cout << "Position: " << value[0] << ", " << value[1] << std::endl;
        });

        weight
        .set_step(0.1f, 1.0f)
        .set_format("%.2f")
        .set_on_change([](float value) {
            std::cout << "Weight: " << value << std::endl;
        });
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<InputExample>(std::string("Input Example"));
    return app.exec();
}
