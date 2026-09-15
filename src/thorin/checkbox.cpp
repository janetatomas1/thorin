
#include "thorin/checkbox.hpp"

namespace thorin {
    Checkbox::Checkbox(
        std::string title,
        std::function<void(bool)> on_change,
        Widget* parent
    ): Widget(title, parent),
    onChange_(std::move(on_change)),
    label_(std::move(title)) {}

    bool Checkbox::show() {
        bool changed = ImGui::Checkbox(label_.c_str(), &value_);

        if (changed && onChange_) {
            onChange_(value_);
        }

        return changed;
    }

    bool Checkbox::value() const {
        return value_;
    }
}
