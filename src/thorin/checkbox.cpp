
#include "thorin/checkbox.hpp"

namespace thorin {
    Checkbox::Checkbox(
        std::string title,
        std::function<void(bool)> on_change,
        Widget* parent
    ): Widget(title, parent),
    label_(std::move(title)),
    onChange_(std::move(on_change)) {}

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

    void Checkbox::set_value(bool value) {
        value_ = value;
    }

    Checkbox& Checkbox::set_on_change(std::function<void(bool)> callback) {
        onChange_ = std::move(callback);
        return *this;
    }
}
