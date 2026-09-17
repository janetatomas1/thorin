
#include <libassert/assert.hpp>

#include "thorin/radiobutton.hpp"
#include "thorin/thorin.hpp"

namespace thorin {
    RadioButton::RadioButton(const std::string& title, int* groupValue, int value, Widget* parent)
    : Widget(title, parent), groupValue_(groupValue), value_(value) {
        DEBUG_ASSERT(groupValue_ != nullptr, "RadioButton requires a non-null groupValue pointer", title);
    }

    RadioButton::RadioButton(const std::string& title, int* groupValue, int value,
                              std::function<void()> onSelect, Widget* parent)
    : Widget(title, parent), groupValue_(groupValue), value_(value), onSelect_(std::move(onSelect)) {
        DEBUG_ASSERT(groupValue_ != nullptr, "RadioButton requires a non-null groupValue pointer", title);
    }

    bool RadioButton::show() {
        DEBUG_ASSERT(groupValue_ != nullptr, "RadioButton::show called with no groupValue bound");

        bool wasSelected = selected();
        bool clicked = ImGui::RadioButton(title_id().c_str(), groupValue_, value_);

        if (clicked && !wasSelected && onSelect_) {
            onSelect_();
        }

        return clicked;
    }

    bool RadioButton::selected() const {
        DEBUG_ASSERT(groupValue_ != nullptr, "RadioButton::selected called with no groupValue bound");
        return *groupValue_ == value_;
    }

    void RadioButton::select() {
        DEBUG_ASSERT(groupValue_ != nullptr, "RadioButton::select called with no groupValue bound");
        *groupValue_ = value_;
    }

    RadioButton& RadioButton::set_on_select(std::function<void()> callback) {
        onSelect_ = std::move(callback);
        return *this;
    }
}