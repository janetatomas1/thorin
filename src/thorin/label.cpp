
#include "thorin/label.hpp"

namespace thorin {
    Label::Label(const std::string& text, Widget* parent)
    : Widget(text, parent) {}

    bool Label::show() {
        if (separator_) {
            ImGui::SeparatorText(title().c_str());
            return false;
        }

        if (wrapped_) {
            ImGui::PushTextWrapPos(0.0f);
        }

        ImGui::TextUnformatted(title().c_str());

        if (wrapped_) {
            ImGui::PopTextWrapPos();
        }

        return false;
    }

    bool Label::wrapped() const {
        return wrapped_;
    }

    Label& Label::set_wrapped(bool wrapped) {
        wrapped_ = wrapped;
        return *this;
    }

    bool Label::separator() const {
        return separator_;
    }

    Label& Label::set_separator(bool separator) {
        separator_ = separator;
        return *this;
    }
}
