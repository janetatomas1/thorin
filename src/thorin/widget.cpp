
#include <format>

#include "thorin/widget.hpp"
#include "thorin/thorin.hpp"

namespace thorin {
    Widget::Widget(const std::string& title, Widget* parent):
    id_(Thorin::random()),
    title_(title),
    titleID_(std::format("{}##{}", title, id_)), parent_(parent) {
        if (parent != nullptr) {
            parent->layout().add_child(layout());
        }
    }

    uint64_t Widget::id() const {
        return id_;
    }

    std::string Widget::title() const {
        return title_;
    }

    std::string Widget::title_id() const {
        return titleID_;
    }

    bool Widget::render() {
        return show();
    }

    bool Widget::show() {
        return false;
    }

    Window* Widget::window() {
        if (window_ != nullptr) {
            return window_;
        }

        return parent_->window();
    }

    void Widget::set_window(Window* window) {
        window_ = window;
    }

    Widget* Widget::parent() {
        return parent_;
    }

    void Widget::set_parent(Widget* parent) {
        parent_ = parent;
    }

    Thorin& Widget::app() {
        if (window_ != nullptr) {
            return window_->app();
        }

        return parent_->app();
    }

    Layout& Widget::layout() {
        return layout_;
    }
}
