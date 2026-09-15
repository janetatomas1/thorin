#include "thorin/layout.hpp"

namespace thorin {
    Layout::Layout(): node_(YGNodeNew()) {
        YGNodeSetContext(node_, this);
    }

    Layout::~Layout() {
        auto parent = YGNodeGetParent(node_);

        if (parent != nullptr) {
            YGNodeRemoveChild(parent, node_);
        }

        YGNodeSetContext(node_, nullptr);
        YGNodeFree(node_);
    }

    YGNodeRef Layout::node() {
        return node_;
    }

    void Layout::add_child(Layout &child, size_t index) {
        size_t idx = index == std::string::npos ? YGNodeGetChildCount(node_) : index;
        YGNodeInsertChild(node_, child.node(), idx);
    }

    void Layout::calculate_layout(float width, float height) {
        YGNodeCalculateLayout(node_, width, height, YGDirectionLTR);
        calculate_position();
    }

    void Layout::calculate_position() {
        auto parentLayout = parent();

        position_.x = YGNodeLayoutGetLeft(node_);
        position_.y = YGNodeLayoutGetTop(node_);
        size_.x = YGNodeLayoutGetWidth(node_);
        size_.y = YGNodeLayoutGetHeight(node_);

        if (parentLayout != nullptr) {
            position_.x += parentLayout->position().x;
            position_.y += parentLayout->position().y;
        }

        size_t count = YGNodeGetChildCount(node_);
        for (size_t i = 0; i < count; ++i) {
            auto child = YGNodeGetChild(node_, i);
            if (child != nullptr) {
                auto layout = static_cast<Layout*>(YGNodeGetContext(child));
                layout->calculate_position();
            }
        }
    }

    float Layout::x() {
        return position_.x;
    }

    float Layout::y() {
        return position_.y;
    }

    const ImVec2& Layout::position() {
        return position_;
    }

    float Layout::width() {
        return size_.x;
    }

    float Layout::height() {
        return size_.y;
    }

    const ImVec2& Layout::size() {
        return size_;
    }

    Layout* Layout::parent() {
        auto parent = YGNodeGetParent(node_);

        if (parent != nullptr) {
            return static_cast<Layout*>(YGNodeGetContext(parent));
        }

        return nullptr;
    }

    Layout& Layout::set_margin(LayoutValue value, YGEdge edge) {
        switch (value.unit) {
            case YGUnitPoint:   YGNodeStyleSetMargin(node_, edge, value.value); break;
            case YGUnitPercent: YGNodeStyleSetMarginPercent(node_, edge, value.value); break;
            case YGUnitAuto:    YGNodeStyleSetMarginAuto(node_, edge); break;
            default: break;
        }
        return *this;
    }

    Layout& Layout::set_margin(float points, YGEdge edge) {
        return set_margin(thorin::points(points), edge);
    }

    Layout& Layout::set_padding(LayoutValue value, YGEdge edge) {
        switch (value.unit) {
            case YGUnitPoint:   YGNodeStyleSetPadding(node_, edge, value.value); break;
            case YGUnitPercent: YGNodeStyleSetPaddingPercent(node_, edge, value.value); break;
            default: break; // padding has no "auto" in Yoga
        }
        return *this;
    }

    Layout& Layout::set_padding(float points, YGEdge edge) {
        return set_padding(thorin::points(points), edge);
    }

    Layout& Layout::set_position(LayoutValue value, YGEdge edge) {
        switch (value.unit) {
            case YGUnitPoint:   YGNodeStyleSetPosition(node_, edge, value.value); break;
            case YGUnitPercent: YGNodeStyleSetPositionPercent(node_, edge, value.value); break;
            default: break; // position has no "auto" in Yoga
        }
        return *this;
    }

    Layout& Layout::set_position(float points, YGEdge edge) {
        return set_position(thorin::points(points), edge);
    }

    Layout& Layout::set_width(LayoutValue value) {
        switch (value.unit) {
            case YGUnitPoint:   YGNodeStyleSetWidth(node_, value.value); break;
            case YGUnitPercent: YGNodeStyleSetWidthPercent(node_, value.value); break;
            case YGUnitAuto:    YGNodeStyleSetWidthAuto(node_); break;
            default: break;
        }
        return *this;
    }

    Layout& Layout::set_width(float points) {
        return set_width(thorin::points(points));
    }

    Layout& Layout::set_height(LayoutValue value) {
        switch (value.unit) {
            case YGUnitPoint:   YGNodeStyleSetHeight(node_, value.value); break;
            case YGUnitPercent: YGNodeStyleSetHeightPercent(node_, value.value); break;
            case YGUnitAuto:    YGNodeStyleSetHeightAuto(node_); break;
            default: break;
        }
        return *this;
    }

    Layout& Layout::set_height(float points) {
        return set_height(thorin::points(points));
    }

    Layout& Layout::set_gap(LayoutValue value, YGGutter gutter) {
        switch (value.unit) {
            case YGUnitPoint:
            case YGUnitPercent:
                YGNodeStyleSetGap(node_, gutter, value.value);
                break;
            default: break; // gap has no "auto"
        }
        return *this;
    }

    Layout& Layout::set_gap(float points, YGGutter gutter) {
        return set_gap(thorin::points(points), gutter);
    }

    Layout& Layout::set_flex_direction(YGFlexDirection direction) {
        YGNodeStyleSetFlexDirection(node_, direction);
        return *this;
    }

    Layout& Layout::set_position_type(YGPositionType type) {
        YGNodeStyleSetPositionType(node_, type);
        return *this;
    }

    Layout& Layout::set_flex_grow(float value) {
        YGNodeStyleSetFlexGrow(node_, value);
        return *this;
    }

    Layout& Layout::set_flex_shrink(float value) {
        YGNodeStyleSetFlexShrink(node_, value);
        return *this;
    }

    Layout& Layout::set_flex_basis(LayoutValue value) {
        switch (value.unit) {
        case YGUnitPoint:   YGNodeStyleSetFlexBasis(node_, value.value); break;
        case YGUnitPercent: YGNodeStyleSetFlexBasisPercent(node_, value.value); break;
        case YGUnitAuto:    YGNodeStyleSetFlexBasisAuto(node_); break;
        default: break;
        }
        return *this;
    }

    Layout& Layout::set_flex_basis(float points) {
        return set_flex_basis(thorin::points(points));
    }

    Layout& Layout::set_flex(float value) {
        YGNodeStyleSetFlex(node_, value);
        return *this;
    }

    Layout& Layout::set_flex_wrap(YGWrap wrap) {
        YGNodeStyleSetFlexWrap(node_, wrap);
        return *this;
    }

    Layout& Layout::set_align_items(YGAlign align) {
        YGNodeStyleSetAlignItems(node_, align);
        return *this;
    }

    Layout& Layout::set_align_self(YGAlign align) {
        YGNodeStyleSetAlignSelf(node_, align);
        return *this;
    }

    Layout& Layout::set_align_content(YGAlign align) {
        YGNodeStyleSetAlignContent(node_, align);
        return *this;
    }

    Layout& Layout::set_justify_content(YGJustify justify) {
        YGNodeStyleSetJustifyContent(node_, justify);
        return *this;
    }

    Layout& Layout::set_min_width(LayoutValue value) {
        switch (value.unit) {
        case YGUnitPoint:   YGNodeStyleSetMinWidth(node_, value.value); break;
        case YGUnitPercent: YGNodeStyleSetMinWidthPercent(node_, value.value); break;
        default: break;
        }
        return *this;
    }

    Layout& Layout::set_min_width(float points) {
        return set_min_width(thorin::points(points));
    }

    Layout& Layout::set_min_height(LayoutValue value) {
        switch (value.unit) {
        case YGUnitPoint:   YGNodeStyleSetMinHeight(node_, value.value); break;
        case YGUnitPercent: YGNodeStyleSetMinHeightPercent(node_, value.value); break;
        default: break;
        }
        return *this;
    }

    Layout& Layout::set_min_height(float points) {
        return set_min_height(thorin::points(points));
    }

    Layout& Layout::set_max_width(LayoutValue value) {
        switch (value.unit) {
        case YGUnitPoint:   YGNodeStyleSetMaxWidth(node_, value.value); break;
        case YGUnitPercent: YGNodeStyleSetMaxWidthPercent(node_, value.value); break;
        default: break;
        }
        return *this;
    }

    Layout& Layout::set_max_width(float points) {
        return set_max_width(thorin::points(points));
    }

    Layout& Layout::set_max_height(LayoutValue value) {
        switch (value.unit) {
        case YGUnitPoint:   YGNodeStyleSetMaxHeight(node_, value.value); break;
        case YGUnitPercent: YGNodeStyleSetMaxHeightPercent(node_, value.value); break;
        default: break;
        }
        return *this;
    }

    Layout& Layout::set_max_height(float points) {
        return set_max_height(thorin::points(points));
    }

    Layout& Layout::set_border(float width, YGEdge edge) {
        YGNodeStyleSetBorder(node_, edge, width);
        return *this;
    }

    Layout& Layout::set_display(YGDisplay display) {
        YGNodeStyleSetDisplay(node_, display);
        return *this;
    }

    Layout& Layout::set_overflow(YGOverflow overflow) {
        YGNodeStyleSetOverflow(node_, overflow);
        return *this;
    }

    Layout& Layout::set_aspect_ratio(float ratio) {
        YGNodeStyleSetAspectRatio(node_, ratio);
        return *this;
    }

    Layout& Layout::set_direction(YGDirection direction) {
        YGNodeStyleSetDirection(node_, direction);
        return *this;
    }

    Layout& Layout::fill_parent() {
        return set_width(thorin::percent(100)).set_height(thorin::percent(100));
    }

    Layout& Layout::row(float gap) {
        return set_flex_direction(YGFlexDirectionRow).set_gap(gap);
    }

    Layout& Layout::column(float gap) {
        return set_flex_direction(YGFlexDirectionColumn).set_gap(gap);
    }

    Layout& Layout::center() {
        return set_align_items(YGAlignCenter).set_justify_content(YGJustifyCenter);
    }

    float Layout::computed_margin(YGEdge edge) {
        return YGNodeLayoutGetMargin(node_, edge);
    }

    float Layout::computed_padding(YGEdge edge) {
        return YGNodeLayoutGetPadding(node_, edge);
    }

    float Layout::computed_border(YGEdge edge) {
        return YGNodeLayoutGetBorder(node_, edge);
    }
}
