
#pragma once

#include <string>
#include <functional>

#include "thorin/widget.hpp"

namespace thorin {
    class RadioButton : public Widget {
        int* groupValue_ = nullptr;
        int value_ = 0;
        std::function<void()> onSelect_;

    public:
        RadioButton(const std::string& title, int* groupValue, int value, Widget* parent = nullptr);
        RadioButton(const std::string& title, int* groupValue, int value,
                    std::function<void()> onSelect, Widget* parent = nullptr);

        bool show() override;
        ImVec2 measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) override;

        [[nodiscard]] bool selected() const;
        void select();

        RadioButton& set_on_select(std::function<void()> callback);
    };
}
