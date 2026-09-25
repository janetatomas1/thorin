
#include <libassert/assert.hpp>

#include "thorin/radiogroup.hpp"

using namespace thorin::literals;

namespace thorin {
    RadioGroup::RadioGroup(Widget* parent)
    : Widget("", parent) {
        column();
        gap(0.5_pcts);
    }

    RadioGroup::RadioGroup(RadioGroup&& other) noexcept
    : Widget(std::move(other)),
    Container(std::move(other)),
    selected_(other.selected_) {
        for (auto& option : items()) {
            option.groupValue_ = &selected_;
        }
    }

    RadioGroup& RadioGroup::operator=(RadioGroup&& other) noexcept {
        if (this != &other) {
            Widget::operator=(std::move(other));
            selected_ = other.selected_;
            Container::operator=(std::move(other));

            for (auto& option : items()) {
                option.groupValue_ = &selected_;
            }
        }

        return *this;
    }

    RadioButton& RadioGroup::add_option(const std::string& label) {
        auto& rb = add(label, &selected_, static_cast<int>(count()), this);
        rb.margin(10);
        return rb;
    }

    RadioButton& RadioGroup::add_option(const std::string& label, std::function<void()> onSelect) {
        auto& rb = add(
            label, &selected_, static_cast<int>(count()), std::move(onSelect), this
        );
        rb.margin(10);
        return rb;
    }

    int RadioGroup::selected() const {
        return selected_;
    }

    void RadioGroup::set_selected(int index) {
        DEBUG_ASSERT(index >= 0 && static_cast<size_t>(index) < count(),
                     "RadioGroup::set_selected index out of range", index, count());
        selected_ = index;
    }

    bool RadioGroup::show() {
        int before = selected_;
        Widget::show();
        return selected_ != before;
    }
}
