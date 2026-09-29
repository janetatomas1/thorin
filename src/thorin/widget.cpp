
#include <algorithm>
#include <cmath>
#include <format>

#include <libassert/assert.hpp>

#include "thorin/widget.hpp"
#include "thorin/thorin.hpp"

namespace thorin {
    Widget::Widget(const std::string& title, Widget* parent):
    id_(Thorin::random()),
    title_(title),
    titleID_(std::format("{}##{}", title, id_)) {
        layout_.set_owner(this);
        if (parent != nullptr) {
            parent->layout().add_child(layout());
        }
    }

    Widget::Widget(Widget&& other) noexcept :
        id_(other.id_),
        title_(std::move(other.title_)),
        titleID_(std::move(other.titleID_)),
        tooltip_(std::move(other.tooltip_)),
        enabled_(other.enabled_),
        window_(other.window_),
        layout_(std::move(other.layout_)) {
        layout_.set_owner(this);
    }

    Widget& Widget::operator=(Widget&& other) noexcept {
        if (this != &other) {
            id_ = other.id_;
            title_ = std::move(other.title_);
            titleID_ = std::move(other.titleID_);
            tooltip_ = std::move(other.tooltip_);
            enabled_ = other.enabled_;
            window_ = other.window_;
            layout_ = std::move(other.layout_);
            layout_.set_owner(this);
        }

        return *this;
    }

    void Widget::init() {
        for (size_t i = 0; i < layout_.child_count(); ++i) {
            auto owner = layout_.child(i)->owner();
            DEBUG_ASSERT(owner != nullptr, "Widget::init: child Layout has no owner", i);
            owner->init();
        }
    }

    void Widget::destroy() {
        // Reverse of init(): later children may depend on earlier ones.
        for (size_t i = layout_.child_count(); i-- > 0;) {
            auto owner = layout_.child(i)->owner();
            DEBUG_ASSERT(owner != nullptr, "Widget::destroy: child Layout has no owner", i);
            owner->destroy();
        }
    }

    ImVec2 Widget::measure(float, YGMeasureMode, float, YGMeasureMode) {
        return ImVec2{0.0f, 0.0f};
    }

    float Widget::label_width() const {
        return ImGui::CalcTextSize(titleID_.c_str(), nullptr, true).x;
    }

    float Widget::label_extent() const {
        const float label = label_width();
        return label > 0.0f ? ImGui::GetStyle().ItemInnerSpacing.x + label : 0.0f;
    }

    float Widget::default_field_width() {
        // ImGui's own fallback item width for auto-resizing windows.
        return ImGui::GetFontSize() * 16.0f;
    }

    ImVec2 Widget::measure_field(
        float labelExtent,
        float width,
        YGMeasureMode widthMode,
        float height,
        YGMeasureMode heightMode
    ) {
        return ImVec2{
            fit_measure(default_field_width() + labelExtent, width, widthMode),
            fit_measure(ImGui::GetFrameHeight(), height, heightMode)
        };
    }

    void Widget::set_next_field_width(float labelExtent) const {
        // ImGui draws the label outside the item width, so the field gets what is left of the box.
        // Must stay positive: ImGui treats a width <= 0 as relative to the window's right edge.
        if (width() > 0.0f) {
            ImGui::SetNextItemWidth(std::max(width() - labelExtent, 1.0f));
        }
    }

    bool Widget::push_frame_height() const {
        // Measured heights already equal the frame height; only explicit, stretched or
        // flexed heights need a push. Sub-pixel differences come from Yoga's rounding.
        if (height() <= 0.0f || std::abs(height() - ImGui::GetFrameHeight()) < 0.5f) {
            return false;
        }

        // A box shorter than the font can't be honoured; the frame gets no vertical padding.
        const ImVec2 padding = ImGui::GetStyle().FramePadding;
        const float paddingY = std::max((height() - ImGui::GetFontSize()) * 0.5f, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{padding.x, paddingY});
        return true;
    }

    void Widget::pop_frame_height(bool pushed) {
        if (pushed) {
            ImGui::PopStyleVar();
        }
    }

    float Widget::frame_height(float height, YGMeasureMode heightMode) {
        if (heightMode != YGMeasureModeExactly || std::abs(height - ImGui::GetFrameHeight()) < 0.5f) {
            return ImGui::GetFrameHeight();
        }

        return std::max(height, ImGui::GetFontSize());
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
        layout_.mark_dirty();
        return *this;
    }

    const std::string& Widget::tooltip() const {
        return tooltip_;
    }

    Widget& Widget::set_tooltip(const std::string& tooltip) {
        tooltip_ = tooltip;
        return *this;
    }

    bool Widget::enabled() const {
        return enabled_;
    }

    Widget& Widget::set_enabled(bool enabled) {
        enabled_ = enabled;
        return *this;
    }

    bool Widget::render() {
        ImGui::SetCursorPos(layout().position());
        return draw();
    }

    bool Widget::draw() {
        if (!enabled_) {
            ImGui::BeginDisabled();
        }

        bool changed = false;

        if (tooltip_.empty()) {
            changed = show();
        } else {
            // The group makes the whole widget one item, so hovering works for containers too,
            // not just for their last child.
            ImGui::BeginGroup();
            changed = show();
            ImGui::EndGroup();
        }

        // Ended before the tooltip, which would otherwise be drawn greyed out too.
        if (!enabled_) {
            ImGui::EndDisabled();
        }

        if (!tooltip_.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip | ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("%s", tooltip_.c_str());
        }

        return changed;
    }

    bool Widget::show() {
        bool changed = false;

        // Count re-read each step: a child's callback may change the tree (actions are
        // normally deferred, but don't rely on it for bounds).
        for (size_t i = 0; i < layout_.child_count(); ++i) {
            auto child = layout_.child(i);
            if (!child->visible()) {
                continue; // Yoga gave it no box; drawing it would land on a stale position.
            }

            auto owner = child->owner();
            DEBUG_ASSERT(owner != nullptr, "Widget::show: child Layout has no owner", i);
            changed |= owner->render();
        }

        return changed;
    }

    Window* Widget::window() {
        if (window_ != nullptr) {
            return window_;
        }

        auto p = parent();
        DEBUG_ASSERT(p != nullptr, "Widget::window: no window_ and no parent to fall back to", title_);
        return p->window();
    }

    void Widget::set_window(Window* window) {
        window_ = window;
    }

    Widget* Widget::parent() {
        auto parentLayout = layout_.parent();
        return parentLayout != nullptr ? parentLayout->owner() : nullptr;
    }

    void Widget::set_parent(Widget* parent) {
        DEBUG_ASSERT(parent != this, "set_parent: cannot set a Widget as its own parent");

        // The Yoga tree is the only record of parenthood. Only touch it when
        // the parent actually changes, so re-asserting the same parent
        // doesn't silently reorder this child to the end of the children list.
        auto currentParentNode = YGNodeGetParent(layout_.node());
        auto newParentNode = parent != nullptr ? parent->layout().node() : nullptr;

        if (currentParentNode != newParentNode) {
            layout_.remove_from_parent();
            if (parent != nullptr) {
                parent->layout().add_child(layout_);
            }

            // The new tree may belong to a window with a different style.
            layout_.mark_dirty();
        }
    }

    Thorin& Widget::app() {
        return Thorin::current();
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

    float Widget::gap(YGGutter gutter) const {
        return layout_.gap(gutter);
    }

    YGFlexDirection Widget::flex_direction() const {
        return layout_.flex_direction();
    }

    float Widget::flex_grow() const {
        return layout_.flex_grow();
    }

    float Widget::flex_shrink() const {
        return layout_.flex_shrink();
    }

    LayoutValue Widget::flex_basis() const {
        return layout_.flex_basis();
    }

    float Widget::flex() const {
        return layout_.flex();
    }

    YGWrap Widget::flex_wrap() const {
        return layout_.flex_wrap();
    }

    YGAlign Widget::align_items() const {
        return layout_.align_items();
    }

    YGAlign Widget::align_self() const {
        return layout_.align_self();
    }

    YGAlign Widget::align_content() const {
        return layout_.align_content();
    }

    YGJustify Widget::justify_content() const {
        return layout_.justify_content();
    }

    LayoutValue Widget::min_width() const {
        return layout_.min_width();
    }

    LayoutValue Widget::min_height() const {
        return layout_.min_height();
    }

    LayoutValue Widget::max_width() const {
        return layout_.max_width();
    }

    LayoutValue Widget::max_height() const {
        return layout_.max_height();
    }

    YGDisplay Widget::display() const {
        return layout_.display();
    }

    YGOverflow Widget::overflow() const {
        return layout_.overflow();
    }

    float Widget::aspect_ratio() const {
        return layout_.aspect_ratio();
    }

    YGDirection Widget::direction() const {
        return layout_.direction();
    }

    YGPositionType Widget::position_type() const {
        return layout_.position_type();
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

    float Widget::margin(YGEdge edge) const {
        return layout_.margin(edge);
    }

    float Widget::padding(YGEdge edge) const {
        return layout_.padding(edge);
    }

    float Widget::border(YGEdge edge) const {
        return layout_.border(edge);
    }
}
