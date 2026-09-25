
#pragma once

#include <memory>
#include <string>
#include <vector>

#include <function2/function2.hpp>

#include "thorin/widget.hpp"

namespace thorin {
    // Entries of a Menu are drawn in ImGui's popup flow, not placed by Yoga:
    // they are owned by the Menu and never added to the Yoga tree.

    class MenuItem : public Widget {
        std::string shortcut_;
        fu2::unique_function<void()> callback_;
        bool enabled_ = true;
        bool checkable_ = false;
        bool checked_ = false;

    public:
        MenuItem(
            const std::string& title,
            fu2::unique_function<void()> callback = nullptr,
            const std::string& shortcut = ""
        );

        // The shortcut is only displayed; ImGui does not bind it.
        MenuItem& set_shortcut(const std::string& shortcut);
        [[nodiscard]] const std::string& shortcut() const;

        MenuItem& set_callback(fu2::unique_function<void()> callback);

        MenuItem& set_enabled(bool enabled);
        [[nodiscard]] bool enabled() const;

        // A checkable item toggles checked() when clicked, before the callback runs.
        MenuItem& set_checkable(bool checkable);
        [[nodiscard]] bool checkable() const;
        MenuItem& set_checked(bool checked);
        [[nodiscard]] bool checked() const;

        bool show() override;
    };

    class MenuSeparator : public Widget {
    public:
        bool show() override;
    };

    class Menu : public Widget {
        std::vector<std::unique_ptr<Widget>> entries_;
        bool enabled_ = true;

    public:
        Menu(const std::string& title);

        // Returned references stay valid for the Menu's lifetime.
        MenuItem& add_item(
            const std::string& title,
            fu2::unique_function<void()> callback = nullptr,
            const std::string& shortcut = ""
        );
        Menu& add_menu(const std::string& title);
        Menu& add_separator();

        [[nodiscard]] size_t count() const;
        Widget& at(size_t index);

        Menu& set_enabled(bool enabled);
        [[nodiscard]] bool enabled() const;

        // Draws the menu header and, while open, its entries. Must be called inside a
        // menu bar or another open menu.
        bool show() override;
    };
}
