
#include "thorin/checkbox.hpp"

namespace thorin {
    Checkbox::Checkbox(
        std::string title,
        std::function<void(bool)> on_change,
        Widget* parent
    ): Widget(title, parent),
    onChange_(std::move(on_change)) {
        layout().enable_measure();
    }

    bool Checkbox::show() {
        bool changed = ImGui::Checkbox(title_id().c_str(), &value_);

        if (changed && onChange_) {
            onChange_(value_);
        }

        return changed;
    }

    ImVec2 Checkbox::measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
        // Same as ImGui::Checkbox: a frame-height square, then the visible label after ItemInnerSpacing.
        const ImVec2 label = ImGui::CalcTextSize(title_id().c_str(), nullptr, true);
        const float square = ImGui::GetFrameHeight();

        return ImVec2{
            fit_measure(square + label_extent(), width, widthMode),
            fit_measure(label.y + ImGui::GetStyle().FramePadding.y * 2.0f, height, heightMode)
        };
    }

    bool Checkbox::value() const {
        return value_;
    }

    Checkbox& Checkbox::set_value(bool value) {
        value_ = value;
        return *this;
    }

    Checkbox& Checkbox::set_on_change(std::function<void(bool)> callback) {
        onChange_ = std::move(callback);
        return *this;
    }
}
