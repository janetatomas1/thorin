#include <thorin/thorin.hpp>
#include <thorin/colorpicker.hpp>
#include <iostream>

using namespace thorin;
using namespace literals;

struct ColorPickerExample: Widget {
    ColorPicker<> accent = ColorPicker<>("Accent", this);
    ColorPicker<3> background = ColorPicker<3>("Background", this);

    ColorPickerExample(const std::string& title): Widget(title) {
        row(16.0f).center();

        accent
        .set_value({1.0f, 0.5f, 0.0f, 1.0f})
        .set_reference({1.0f, 0.5f, 0.0f, 1.0f})
        .set_flags(ImGuiColorEditFlags_AlphaBar)
        .set_on_change([](const std::array<float, 4>& value) {
            std::cout << "Accent: " << value[0] << ", " << value[1] << ", " << value[2] << ", " << value[3] << std::endl;
        });

        background
        .set_value({0.1f, 0.2f, 0.3f})
        .set_flags(ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_DisplayHex);
    }

    bool show() override {
        bool changed = false;
        changed |= accent.render();
        changed |= background.render();
        return changed;
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<ColorPickerExample>(std::string("ColorPicker Example"));
    return app.exec();
}
