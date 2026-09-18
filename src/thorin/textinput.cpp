
#include "thorin/textinput.hpp"

#include <imgui_stdlib.h>

namespace thorin {
    TextInput::TextInput(const std::string& title, Widget* parent)
    : Widget(title, parent) {}

    bool TextInput::show() {
        ImGui::SetNextItemWidth(layout().width());

        bool changed;
        if (multiline_) {
            changed = ImGui::InputTextMultiline(title_id().c_str(), &value_, size(), flags_);
        } else if (hint_.empty()) {
            changed = ImGui::InputText(title_id().c_str(), &value_, flags_);
        } else {
            changed = ImGui::InputTextWithHint(title_id().c_str(), hint_.c_str(), &value_, flags_);
        }

        if (changed && onChange_) {
            onChange_(value_);
        }

        return changed;
    }

    const std::string& TextInput::value() const {
        return value_;
    }

    TextInput& TextInput::set_value(const std::string& value) {
        value_ = value;
        return *this;
    }

    const std::string& TextInput::hint() const {
        return hint_;
    }

    TextInput& TextInput::set_hint(const std::string& hint) {
        hint_ = hint;
        return *this;
    }

    TextInputFlags TextInput::flags() const {
        return flags_;
    }

    TextInput& TextInput::set_flags(TextInputFlags flags) {
        flags_ = flags;
        return *this;
    }

    bool TextInput::multiline() const {
        return multiline_;
    }

    TextInput& TextInput::set_multiline(bool multiline) {
        multiline_ = multiline;
        return *this;
    }

    TextInput& TextInput::set_on_change(std::function<void(const std::string&)> callback) {
        onChange_ = std::move(callback);
        return *this;
    }
}
