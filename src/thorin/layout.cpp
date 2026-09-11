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
}