
#include <libassert/assert.hpp>

#include "thorin/dropdown.hpp"

namespace thorin {
    Dropdown::Dropdown(const std::string& title, Widget* parent)
    : Widget(title, parent) {}

    Dropdown& Dropdown::add_option(const std::string& label) {
        options_.push_back(label);
        return *this;
    }

    Dropdown& Dropdown::insert_option(size_t index, const std::string& label) {
        DEBUG_ASSERT(index <= options_.size(), "insert_option index out of range", index, options_.size());

        options_.insert(options_.begin() + index, label);

        if (selected_ >= 0 && static_cast<size_t>(selected_) >= index) {
            selected_ += 1;
        }

        return *this;
    }

    void Dropdown::remove_option(size_t index) {
        DEBUG_ASSERT(index < options_.size(), "remove_option index out of range", index, options_.size());

        options_.erase(options_.begin() + index);

        if (selected_ == static_cast<int>(index)) {
            selected_ = -1;
        } else if (selected_ > static_cast<int>(index)) {
            selected_ -= 1;
        }
    }

    void Dropdown::clear_options() {
        options_.clear();
        selected_ = -1;
    }

    int Dropdown::selected() const {
        return selected_;
    }

    void Dropdown::set_selected(int index) {
        DEBUG_ASSERT(index >= -1 && static_cast<size_t>(index) < options_.size() || index == -1,
                     "Dropdown::set_selected index out of range", index, options_.size());
        selected_ = index;
    }

    const std::string& Dropdown::selected_label() const {
        DEBUG_ASSERT(has_selection(), "selected_label called with no selection");
        return options_[selected_];
    }

    bool Dropdown::has_selection() const {
        return selected_ >= 0 && static_cast<size_t>(selected_) < options_.size();
    }

    size_t Dropdown::count() const {
        return options_.size();
    }

    const std::string& Dropdown::at(size_t index) const {
        DEBUG_ASSERT(index < options_.size(), "option_at index out of range", index, options_.size());
        return options_[index];
    }

    Dropdown& Dropdown::set_placeholder(const std::string& text) {
        placeholder_ = text;
        return *this;
    }

    const std::string& Dropdown::placeholder() const {
        return placeholder_;
    }

    DropdownFlags Dropdown::flags() const {
        return flags_;
    }

    Dropdown& Dropdown::set_flags(DropdownFlags flags) {
        flags_ = flags;
        return *this;
    }

    Dropdown& Dropdown::set_on_change(std::function<void(int)> callback) {
        onChange_ = std::move(callback);
        return *this;
    }

    bool Dropdown::show() {
        const char* preview = has_selection() ? options_[selected_].c_str() : placeholder_.c_str();
        bool changed = false;

        ImGui::SetNextItemWidth(layout().width());
        if (ImGui::BeginCombo(title_id().c_str(), preview, flags_)) {
            for (size_t i = 0; i < options_.size(); ++i) {
                bool isSelected = (static_cast<int>(i) == selected_);

                ImGui::PushID(static_cast<int>(i));
                if (ImGui::Selectable(options_[i].c_str(), isSelected)) {
                    if (selected_ != static_cast<int>(i)) {
                        selected_ = static_cast<int>(i);
                        changed = true;
                    }
                }
                ImGui::PopID();

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (changed && onChange_) {
            onChange_(selected_);
        }

        return changed;
    }
}
