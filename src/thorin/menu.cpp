
#include <libassert/assert.hpp>

#include "thorin/menu.hpp"

namespace thorin {
    MenuItem::MenuItem(
        const std::string& title,
        fu2::unique_function<void()> callback,
        const std::string& shortcut
    ) : Widget(title), shortcut_(shortcut), callback_(std::move(callback)) {}

    MenuItem& MenuItem::set_shortcut(const std::string& shortcut) {
        shortcut_ = shortcut;
        return *this;
    }

    const std::string& MenuItem::shortcut() const {
        return shortcut_;
    }

    MenuItem& MenuItem::set_callback(fu2::unique_function<void()> callback) {
        callback_ = std::move(callback);
        return *this;
    }

    MenuItem& MenuItem::set_enabled(bool enabled) {
        enabled_ = enabled;
        return *this;
    }

    bool MenuItem::enabled() const {
        return enabled_;
    }

    MenuItem& MenuItem::set_checkable(bool checkable) {
        checkable_ = checkable;
        return *this;
    }

    bool MenuItem::checkable() const {
        return checkable_;
    }

    MenuItem& MenuItem::set_checked(bool checked) {
        checked_ = checked;
        return *this;
    }

    bool MenuItem::checked() const {
        return checked_;
    }

    bool MenuItem::show() {
        const char* shortcut = shortcut_.empty() ? nullptr : shortcut_.c_str();

        if (ImGui::MenuItem(title_id().c_str(), shortcut, checkable_ ? &checked_ : nullptr, enabled_)) {
            if (callback_) {
                callback_();
            }

            return true;
        }

        return false;
    }

    bool MenuSeparator::show() {
        ImGui::Separator();
        return false;
    }

    Menu::Menu(const std::string& title)
    : Widget(title) {}

    MenuItem& Menu::add_item(
        const std::string& title,
        fu2::unique_function<void()> callback,
        const std::string& shortcut
    ) {
        auto item = std::make_unique<MenuItem>(title, std::move(callback), shortcut);
        auto& ref = *item;
        entries_.push_back(std::move(item));
        return ref;
    }

    Menu& Menu::add_menu(const std::string& title) {
        auto menu = std::make_unique<Menu>(title);
        auto& ref = *menu;
        entries_.push_back(std::move(menu));
        return ref;
    }

    Menu& Menu::add_separator() {
        entries_.push_back(std::make_unique<MenuSeparator>());
        return *this;
    }

    size_t Menu::count() const {
        return entries_.size();
    }

    Widget& Menu::at(size_t index) {
        DEBUG_ASSERT(index < entries_.size(), "Menu::at index out of range", index, entries_.size());
        return *entries_[index];
    }

    Menu& Menu::set_enabled(bool enabled) {
        enabled_ = enabled;
        return *this;
    }

    bool Menu::enabled() const {
        return enabled_;
    }

    bool Menu::show() {
        bool clicked = false;

        if (ImGui::BeginMenu(title_id().c_str(), enabled_)) {
            for (auto& entry : entries_) {
                clicked |= entry->show();
            }

            ImGui::EndMenu();
        }

        return clicked;
    }
}
