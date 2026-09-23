
#pragma once

#include <functional>
#include <string>

#include "thorin/widget.hpp"

namespace thorin {
    class Checkbox : public Widget {
        bool value_ = false;
        std::function<void(bool)> onChange_;

    public:
        Checkbox(
            std::string label,
            std::function<void(bool)> on_change = nullptr,
            Widget* parent = nullptr
        );

        bool show() override;
        ImVec2 measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) override;
        [[nodiscard]] bool value() const;
        Checkbox& set_value(bool value);
        Checkbox& set_on_change(std::function<void(bool)> callback);
    };
}
