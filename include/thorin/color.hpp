#pragma once

#include <cstddef>
#include <optional>
#include <string_view>

#include <imgui.h>

namespace thorin {
    // Parses "#rgb", "#rgba", "#rrggbb", "#rrggbbaa" (or with "0x" instead of '#'), "rgb[a](r, g, b[, a])",
    // "hsv(h, s, v[, a])" or "hsl[a](h, s, l[, a])". r, g, b 0-255; h 0-360; s, v, l 0-100; a 0-1.
    // std::nullopt if malformed or out of range.
    [[nodiscard]] std::optional<ImVec4> parse_color(std::string_view s);

    namespace literals {
        // "#ff8800"_rgb, "255, 136, 0"_rgb, "255, 136, 0, 0.5"_rgb. Same formats and ranges as parse_color.
        // Invalid input asserts in debug and gives transparent black in release.
        ImVec4 operator""_rgb(const char* s, std::size_t n);

        // "32, 100, 100"_hsv, "32, 100, 100, 0.5"_hsv. Same formats and ranges as parse_color.
        // Invalid input asserts in debug and gives transparent black in release.
        ImVec4 operator""_hsv(const char* s, std::size_t n);

        // "32, 100, 50"_hsl, "32, 100, 50, 0.5"_hsl. Same formats and ranges as parse_color.
        // Invalid input asserts in debug and gives transparent black in release.
        ImVec4 operator""_hsl(const char* s, std::size_t n);
    }
}
