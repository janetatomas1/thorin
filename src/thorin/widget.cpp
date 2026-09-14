
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
        ImGui::SetCursorPos(layout().position());
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

    Widget& Widget::set_margin(LayoutValue value, YGEdge edge) {
        layout_.set_margin(value, edge);
        return *this;
    }

    Widget& Widget::set_margin(float points, YGEdge edge) {
        layout_.set_margin(points, edge);
        return *this;
    }

    Widget& Widget::set_padding(LayoutValue value, YGEdge edge) {
        layout_.set_padding(value, edge);
        return *this;
    }

    Widget& Widget::set_padding(float points, YGEdge edge) {
        layout_.set_padding(points, edge);
        return *this;
    }

    Widget& Widget::set_position(LayoutValue value, YGEdge edge) {
        layout_.set_position(value, edge);
        return *this;
    }

    Widget& Widget::set_position(float points, YGEdge edge) {
        layout_.set_position(points, edge);
        return *this;
    }

    Widget& Widget::set_width(LayoutValue value) {
        layout_.set_width(value);
        return *this;
    }

    Widget& Widget::set_width(float points) {
        layout_.set_width(points);
        return *this;
    }

    Widget& Widget::set_height(LayoutValue value) {
        layout_.set_height(value);
        return *this;
    }

    Widget& Widget::set_height(float points) {
        layout_.set_height(points);
        return *this;
    }

    Widget& Widget::set_gap(LayoutValue value, YGGutter gutter) {
        layout_.set_gap(value, gutter);
        return *this;
    }

    Widget& Widget::set_gap(float points, YGGutter gutter) {
        layout_.set_gap(points, gutter);
        return *this;
    }

    Widget& Widget::set_flex_direction(YGFlexDirection direction) {
        layout_.set_flex_direction(direction);
        return *this;
    }

    Widget& Widget::set_flex_grow(float value) {
        layout_.set_flex_grow(value);
        return *this;
    }

    Widget& Widget::set_flex_shrink(float value) {
        layout_.set_flex_shrink(value);
        return *this;
    }

    Widget& Widget::set_flex_basis(LayoutValue value) {
        layout_.set_flex_basis(value);
        return *this;
    }

    Widget& Widget::set_flex_basis(float points) {
        layout_.set_flex_basis(points);
        return *this;
    }

    Widget& Widget::set_flex(float value) {
        layout_.set_flex(value);
        return *this;
    }

    Widget& Widget::set_flex_wrap(YGWrap wrap) {
        layout_.set_flex_wrap(wrap);
        return *this;
    }

    Widget& Widget::set_align_items(YGAlign align) {
        layout_.set_align_items(align);
        return *this;
    }

    Widget& Widget::set_align_self(YGAlign align) {
        layout_.set_align_self(align);
        return *this;
    }

    Widget& Widget::set_align_content(YGAlign align) {
        layout_.set_align_content(align);
        return *this;
    }

    Widget& Widget::set_justify_content(YGJustify justify) {
        layout_.set_justify_content(justify);
        return *this;
    }

    Widget& Widget::set_min_width(LayoutValue value) {
        layout_.set_min_width(value);
        return *this;
    }

    Widget& Widget::set_min_width(float points) {
        layout_.set_min_width(points);
        return *this;
    }

    Widget& Widget::set_min_height(LayoutValue value) {
        layout_.set_min_height(value);
        return *this;
    }

    Widget& Widget::set_min_height(float points) {
        layout_.set_min_height(points);
        return *this;
    }

    Widget& Widget::set_max_width(LayoutValue value) {
        layout_.set_max_width(value);
        return *this;
    }

    Widget& Widget::set_max_width(float points) {
        layout_.set_max_width(points);
        return *this;
    }

    Widget& Widget::set_max_height(LayoutValue value) {
        layout_.set_max_height(value);
        return *this;
    }

    Widget& Widget::set_max_height(float points) {
        layout_.set_max_height(points);
        return *this;
    }

    Widget& Widget::set_border(float width, YGEdge edge) {
        layout_.set_border(width, edge);
        return *this;
    }

    Widget& Widget::set_display(YGDisplay display) {
        layout_.set_display(display);
        return *this;
    }

    Widget& Widget::set_overflow(YGOverflow overflow) {
        layout_.set_overflow(overflow);
        return *this;
    }

    Widget& Widget::set_aspect_ratio(float ratio) {
        layout_.set_aspect_ratio(ratio);
        return *this;
    }

    Widget& Widget::set_direction(YGDirection direction) {
        layout_.set_direction(direction);
        return *this;
    }

    Widget& Widget::set_position_type(YGPositionType type) {
        layout_.set_position_type(type);
        return *this;
    }

    Widget& Widget::fill_parent() {
        layout_.fill_parent();
        return *this;
    }

    Widget& Widget::row(float gap) {
        layout_.row(gap);
        return *this;
    }

    Widget& Widget::column(float gap) {
        layout_.column(gap);
        return *this;
    }

    Widget& Widget::center() {
        layout_.center();
        return *this;
    }

    float Widget::width() {
        return layout_.width();
    }

    float Widget::height() {
        return layout_.height();
    }

    const ImVec2& Widget::position() {
        return layout_.position();
    }

    const ImVec2& Widget::size() {
        return layout_.size();
    }
}
