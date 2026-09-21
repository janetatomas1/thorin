
#include "thorin/checkbox.hpp"

namespace thorin {
    Checkbox::Checkbox(
        std::string title,
        std::function<void(bool)> on_change,
        Widget* parent
    ): Widget(title, parent),
    onChange_(std::move(on_change)) {}

    bool Checkbox::show() {
        bool changed = ImGui::Checkbox(title_id().c_str(), &value_);

        if (changed && onChange_) {
            onChange_(value_);
        }

        return changed;
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
