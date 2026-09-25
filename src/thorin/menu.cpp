
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
        return entries_.add<MenuItem>(title, std::move(callback), shortcut);
    }

    Menu& Menu::add_menu(const std::string& title) {
        return entries_.add<Menu>(title);
    }

    Menu& Menu::add_separator() {
        entries_.add<MenuSeparator>();
        return *this;
    }

    size_t Menu::count() const {
        return entries_.count();
    }

    Widget& Menu::at(size_t index) {
        return entries_.at(index);
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
            for (auto& entry : entries_.items()) {
                clicked |= entry.show();
            }

            ImGui::EndMenu();
        }

        return clicked;
    }
}
