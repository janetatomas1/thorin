
#include "thorin/button.hpp"

#include <algorithm>

namespace thorin {
    Button::Button(const std::string &title, Widget *parent)
    : Widget(title, parent) {}

    Button::Button(const std::string &title, fu2::unique_function<void()> callback, Widget *parent)
    : Widget(title, parent), callback_(std::move(callback)) {}

    bool Button::show() {
        if (ImGui::Button(title_id().c_str(), size())){
            if (callback_) {
                callback_();
            }

            return true;
        }

        return false;
    }

    void Button::set_callback(fu2::unique_function<void()> callback) {
        callback_ = std::move(callback);
    }
}
