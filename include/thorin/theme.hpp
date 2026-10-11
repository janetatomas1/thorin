#pragma once

#include <imgui.h>
#include <implot.h>
#include <implot3d.h>

#include "thorin/font.hpp"

namespace thorin {
    // App-wide look, copied into every window's ImGui / ImPlot / ImPlot3D context when it is created.
    // Sizes are unscaled; each window scales them for its display. Defaults are ImGui's dark style.
    // Plot colours default to auto, which follows the ImGui colours.
    struct Theme {
        ImGuiStyle style;
        ImPlotStyle plot;
        ImPlot3DStyle plot3d;
        // Each window's default font, replacing Window::set_font() when the theme is applied; its size is
        // style.FontSizeBase. Borrowed: from Thorin::add_font() or a bundled Font::inter() etc.
        // nullptr = ImGui's built-in font.
        const Font* font = nullptr;

        // ImGui's built-in colour presets.
        [[nodiscard]] static Theme dark();
        [[nodiscard]] static Theme light();
        [[nodiscard]] static Theme classic();
        // Phosphor green on black, square corners, thin borders.
        [[nodiscard]] static Theme matrix();
        // Arctic Nord palette: snow text, frost accents on polar-night greys, soft corners.
        [[nodiscard]] static Theme nord();
        // White lines on blueprint blue, square corners, borders everywhere.
        [[nodiscard]] static Theme blueprint();
        // Atom's One Dark: blue accent on blue-grey, soft corners.
        [[nodiscard]] static Theme one_dark();
    };
}
