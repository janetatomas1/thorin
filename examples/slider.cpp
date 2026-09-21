#include <thorin/thorin.hpp>
#include <thorin/slider.hpp>
#include <array>
#include <initializer_list>
#include <iostream>

using namespace thorin;
using namespace literals;

struct SliderExample: Widget {
    Slider<1, float> volume = Slider<1, float>("Volume", this);
    Slider<2, int> range = Slider<2, int>("Range", this);
    Slider<3, float> color = Slider<3, float>("Color", this);
    Slider<4, float> rect = Slider<4, float>("Rect", this);

    SliderExample(const std::string& title): Widget(title) {
        column(8.0f).center();

        for (Widget* w: std::initializer_list<Widget*>{&volume, &range, &color, &rect}) {
            w->width(50_pcts).height(24.0f);
        }

        volume
        .set_range(0.0f, 1.0f)
        .set_format("%.2f")
        .set_on_change([](float value) {
            std::cout << "Volume: " << value << std::endl;
        });

        range
        .set_range(0, 50)
        .set_on_change([](const std::array<int, 2>& value) {
            std::cout << "Range: " << value[0] << ", " << value[1] << std::endl;
        });

        color.set_range(0.0f, 1.0f);
        rect.set_range(-1.0f, 1.0f);
    }

    bool show() override {
        bool changed = false;
        changed |= volume.render();
        changed |= range.render();
        changed |= color.render();
        changed |= rect.render();
        return changed;
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<SliderExample>(std::string("Slider Example"));
    return app.exec();
}
