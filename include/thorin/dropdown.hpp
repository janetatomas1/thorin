
#pragma once

#include <string>
#include <vector>
#include <functional>

#include "thorin/widget.hpp"

namespace thorin {
    using DropdownFlags = ImGuiComboFlags_;

    class Dropdown : public Widget {
        std::vector<std::string> options_;
        int selected_ = -1;
        std::function<void(int)> onChange_;
        std::string placeholder_ = "Select...";
        DropdownFlags flags_;

    public:
        Dropdown(const std::string& title = "", Widget* parent = nullptr);

        Dropdown& add_option(const std::string& label);
        Dropdown& insert_option(size_t index, const std::string& label);
        void remove_option(size_t index);
        void clear_options();

        [[nodiscard]] int selected() const;
        void set_selected(int index);

        [[nodiscard]] const std::string& selected_label() const;
        [[nodiscard]] bool has_selection() const;

        [[nodiscard]] size_t count() const;
        [[nodiscard]] const std::string& at(size_t index) const;

        Dropdown& set_placeholder(const std::string& text);
        [[nodiscard]] const std::string& placeholder() const;

        [[nodiscard]] DropdownFlags flags() const;
        Dropdown& set_flags(DropdownFlags flags);

        Dropdown& set_on_change(std::function<void(int)> callback);

        bool show() override;
    };
}
