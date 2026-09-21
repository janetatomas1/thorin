
#include <format>

#include <libassert/assert.hpp>

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

    Widget::Widget(Widget&& other) noexcept :
        id_(other.id_),
        title_(std::move(other.title_)),
        titleID_(std::move(other.titleID_)),
        window_(other.window_),
        parent_(other.parent_),
        layout_(std::move(other.layout_)) {}

    Widget& Widget::operator=(Widget&& other) noexcept {
        if (this != &other) {
            id_ = other.id_;
            title_ = std::move(other.title_);
            titleID_ = std::move(other.titleID_);
            window_ = other.window_;
            parent_ = other.parent_;
            layout_ = std::move(other.layout_);
        }

        return *this;
    }

    uint64_t Widget::id() const {
        return id_;
    }

    const std::string& Widget::title() const {
        return title_;
    }

    const std::string& Widget::title_id() const {
        return titleID_;
    }

    Widget& Widget::set_title(const std::string& title) {
        title_ = title;
        titleID_ = std::format("{}##{}", title_, id_);
        return *this;
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

        DEBUG_ASSERT(parent_ != nullptr, "Widget::window: no window_ and no parent to fall back to", title_);
        return parent_->window();
    }

    void Widget::set_window(Window* window) {
        window_ = window;
    }

    Widget* Widget::parent() {
        return parent_;
    }

    void Widget::set_parent(Widget* parent) {
        DEBUG_ASSERT(parent != this, "set_parent: cannot set a Widget as its own parent");

        layout_.remove_from_parent();
        if (parent != nullptr) {
            parent->layout().add_child(layout_);
        }

        parent_ = parent;
    }

    Thorin& Widget::app() {
        if (window_ != nullptr) {
            return window_->app();
        }

        DEBUG_ASSERT(parent_ != nullptr, "Widget::app: no window_ and no parent to fall back to", title_);
        return parent_->app();
    }

    Layout& Widget::layout() {
        return layout_;
    }

    Widget& Widget::margin(LayoutValue value, YGEdge edge) {
        layout_.margin(value, edge);
        return *this;
    }

    Widget& Widget::margin(float points, YGEdge edge) {
        layout_.margin(points, edge);
        return *this;
    }

    Widget& Widget::padding(LayoutValue value, YGEdge edge) {
        layout_.padding(value, edge);
        return *this;
    }

    Widget& Widget::padding(float points, YGEdge edge) {
        layout_.padding(points, edge);
        return *this;
    }

    Widget& Widget::position(LayoutValue value, YGEdge edge) {
        layout_.position(value, edge);
        return *this;
    }

    Widget& Widget::position(float points, YGEdge edge) {
        layout_.position(points, edge);
        return *this;
    }

    Widget& Widget::width(LayoutValue value) {
        layout_.width(value);
        return *this;
    }

    Widget& Widget::width(float points) {
        layout_.width(points);
        return *this;
    }

    Widget& Widget::height(LayoutValue value) {
        layout_.height(value);
        return *this;
    }

    Widget& Widget::height(float points) {
        layout_.height(points);
        return *this;
    }

    Widget& Widget::gap(LayoutValue value, YGGutter gutter) {
        layout_.gap(value, gutter);
        return *this;
    }

    Widget& Widget::gap(float points, YGGutter gutter) {
        layout_.gap(points, gutter);
        return *this;
    }

    Widget& Widget::flex_direction(YGFlexDirection direction) {
        layout_.flex_direction(direction);
        return *this;
    }

    Widget& Widget::flex_grow(float value) {
        layout_.flex_grow(value);
        return *this;
    }

    Widget& Widget::flex_shrink(float value) {
        layout_.flex_shrink(value);
        return *this;
    }

    Widget& Widget::flex_basis(LayoutValue value) {
        layout_.flex_basis(value);
        return *this;
    }

    Widget& Widget::flex_basis(float points) {
        layout_.flex_basis(points);
        return *this;
    }

    Widget& Widget::flex(float value) {
        layout_.flex(value);
        return *this;
    }

    Widget& Widget::flex_wrap(YGWrap wrap) {
        layout_.flex_wrap(wrap);
        return *this;
    }

    Widget& Widget::align_items(YGAlign align) {
        layout_.align_items(align);
        return *this;
    }

    Widget& Widget::align_self(YGAlign align) {
        layout_.align_self(align);
        return *this;
    }

    Widget& Widget::align_content(YGAlign align) {
        layout_.align_content(align);
        return *this;
    }

    Widget& Widget::justify_content(YGJustify justify) {
        layout_.justify_content(justify);
        return *this;
    }

    Widget& Widget::min_width(LayoutValue value) {
        layout_.min_width(value);
        return *this;
    }

    Widget& Widget::min_width(float points) {
        layout_.min_width(points);
        return *this;
    }

    Widget& Widget::min_height(LayoutValue value) {
        layout_.min_height(value);
        return *this;
    }

    Widget& Widget::min_height(float points) {
        layout_.min_height(points);
        return *this;
    }

    Widget& Widget::max_width(LayoutValue value) {
        layout_.max_width(value);
        return *this;
    }

    Widget& Widget::max_width(float points) {
        layout_.max_width(points);
        return *this;
    }

    Widget& Widget::max_height(LayoutValue value) {
        layout_.max_height(value);
        return *this;
    }

    Widget& Widget::max_height(float points) {
        layout_.max_height(points);
        return *this;
    }

    Widget& Widget::border(float width, YGEdge edge) {
        layout_.border(width, edge);
        return *this;
    }

    Widget& Widget::display(YGDisplay display) {
        layout_.display(display);
        return *this;
    }

    Widget& Widget::overflow(YGOverflow overflow) {
        layout_.overflow(overflow);
        return *this;
    }

    Widget& Widget::aspect_ratio(float ratio) {
        layout_.aspect_ratio(ratio);
        return *this;
    }

    Widget& Widget::direction(YGDirection direction) {
        layout_.direction(direction);
        return *this;
    }

    Widget& Widget::position_type(YGPositionType type) {
        layout_.position_type(type);
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

    float Widget::x() const {
        return layout_.x();
    }

    float Widget::y() const {
        return layout_.y();
    }

    float Widget::width() const {
        return layout_.width();
    }

    float Widget::height() const {
        return layout_.height();
    }

    const ImVec2& Widget::position() const {
        return layout_.position();
    }

    const ImVec2& Widget::size() const {
        return layout_.size();
    }
}
