#include <thorin/thorin.hpp>
#include <thorin/drag.hpp>
#include <array>
#include <initializer_list>
#include <iostream>

using namespace thorin;
using namespace literals;

struct DragExample: Widget {
    Drag<1, float> speed = Drag<1, float>("Speed", this);
    Drag<2, int> position = Drag<2, int>("Position", this);
    Drag<3, float> offset = Drag<3, float>("Offset", this);
    Drag<4, float> rect = Drag<4, float>("Rect", this);

    DragExample(const std::string& title): Widget(title) {
        column(8.0f).center();

        for (Widget* w: std::initializer_list<Widget*>{&speed, &position, &offset, &rect}) {
            w->width(50_pcts).height(24.0f);
        }

        speed
        .set_range(0.0f, 10.0f)
        .set_speed(0.05f)
        .set_format("%.2f")
        .set_on_change([](float value) {
            std::cout << "Speed: " << value << std::endl;
        });

        position
        .set_format("%d px")
        .set_on_change([](const std::array<int, 2>& value) {
            std::cout << "Position: " << value[0] << ", " << value[1] << std::endl;
        });

        rect.set_speed(0.1f);
    }

    bool show() override {
        bool changed = false;
        changed |= speed.render();
        changed |= position.render();
        changed |= offset.render();
        changed |= rect.render();
        return changed;
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<DragExample>(std::string("Drag Example"));
    return app.exec();
}
