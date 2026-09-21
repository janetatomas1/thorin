#include <thorin/thorin.hpp>
#include <thorin/coloredit.hpp>
#include <array>
#include <initializer_list>
#include <iostream>

using namespace thorin;
using namespace literals;

struct ColorEditExample: Widget {
    ColorEdit<3> background = ColorEdit<3>("Background", this);
    ColorEdit<4> tint = ColorEdit<4>("Tint", this);
    ColorEdit<4> swatch = ColorEdit<4>("Swatch", this);

    ColorEditExample(const std::string& title): Widget(title) {
        column(8.0f).center();

        for (Widget* w: std::initializer_list<Widget*>{&background, &tint, &swatch}) {
            w->width(50_pcts).height(24.0f);
        }

        background
        .set_value({0.1f, 0.2f, 0.3f})
        .set_on_change([](const std::array<float, 3>& value) {
            std::cout << "Background: " << value[0] << ", " << value[1] << ", " << value[2] << std::endl;
        });

        tint
        .set_value({1.0f, 0.5f, 0.0f, 1.0f})
        .set_flags(ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayHex);

        swatch
        .set_value({0.0f, 0.8f, 0.4f, 1.0f})
        .set_flags(ImGuiColorEditFlags_NoInputs);
    }

    bool show() override {
        bool changed = false;
        changed |= background.render();
        changed |= tint.render();
        changed |= swatch.render();
        return changed;
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<ColorEditExample>(std::string("ColorEdit Example"));
    return app.exec();
}
