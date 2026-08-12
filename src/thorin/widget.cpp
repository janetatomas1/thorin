
#include <format>

#include "thorin/widget.hpp"
#include "thorin/thorin.hpp"

namespace thorin {
    Widget::Widget(const std::string& title):
    id_(Thorin::random()),
    title_(title),
    titleID_(std::format("{}##{}", title, id_)) {
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
}
