#include <thorin/thorin.hpp>
#include <thorin/slider.hpp>
#include <array>
#include <initializer_list>
#include <iostream>

using namespace thorin;
using namespace literals;

struct SliderExample: Widget {
    SliderFloat<1> volume = SliderFloat<1>("Volume", this);
    SliderInt<2> range = SliderInt<2>("Range", this);
    SliderFloat<3> color = SliderFloat<3>("Color", this);
    SliderFloat<4> rect = SliderFloat<4>("Rect", this);

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
