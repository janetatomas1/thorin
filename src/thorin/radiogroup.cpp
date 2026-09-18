
#include <libassert/assert.hpp>

#include "thorin/radiogroup.hpp"

using namespace thorin::literals;

namespace thorin {
    RadioGroup::RadioGroup(Widget* parent)
    : Widget("", parent) {
        column();
        set_gap(0.5_pcts);
    }

    RadioButton& RadioGroup::add_option(const std::string& label) {
        auto rb = RadioButton(label, &selected_, static_cast<int>(options_.size()), this);
        rb.set_margin(10);
        options_.push_back(std::move(rb));
        return options_.back();
    }

    RadioButton& RadioGroup::add_option(const std::string& label, std::function<void()> onSelect) {
        auto rb = RadioButton(
            label, &selected_, static_cast<int>(options_.size()), std::move(onSelect), this
        );
        rb.set_margin(10);
        options_.push_back(std::move(rb));
        return options_.back();
    }

    int RadioGroup::selected() const {
        return selected_;
    }

    void RadioGroup::set_selected(int index) {
        DEBUG_ASSERT(index >= 0 && static_cast<size_t>(index) < options_.size(),
                     "RadioGroup::set_selected index out of range", index, options_.size());
        selected_ = index;
    }

    size_t RadioGroup::count() const {
        return options_.size();
    }

    RadioButton& RadioGroup::at(size_t index) {
        DEBUG_ASSERT(index < options_.size(), "RadioGroup::at index out of range", index, options_.size());
        return options_[index];
    }

    bool RadioGroup::show() {
        int before = selected_;

        for (auto& option : options_) {
            option.render();
        }

        bool changed = selected_ != before;
        return changed;
    }
}
