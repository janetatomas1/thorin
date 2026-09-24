#pragma once

#include <string>

#include "thorin/widget.hpp"

namespace thorin {
    class Text : public Widget {
    public:
        Text(const std::string& text = "", Widget* parent = nullptr);

        bool show() override;
        ImVec2 measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) override;
    };
}
