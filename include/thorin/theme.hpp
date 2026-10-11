#pragma once

#include <imgui.h>
#include <implot.h>
#include <implot3d.h>

namespace thorin {
    // App-wide look, copied into every window's ImGui / ImPlot / ImPlot3D context when it is created.
    // Sizes are unscaled; each window scales them for its display. Defaults are ImGui's dark style.
    // Plot colours default to auto, which follows the ImGui colours.
    struct Theme {
        ImGuiStyle style;
        ImPlotStyle plot;
        ImPlot3DStyle plot3d;

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
