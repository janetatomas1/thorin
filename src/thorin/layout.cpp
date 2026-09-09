
#include "thorin/layout.hpp"


namespace thorin {
    Layout::Layout(): node_(YGNodeNew()) {
    }

    Layout::~Layout() {
        if (parent_ != nullptr) {
            YGNodeRemoveChild(parent_->node(), node_);
        }

        YGNodeFree(node_);
    }

    YGNodeRef Layout::node() {
        return node_;
    }

    void Layout::add_child(Layout &child, size_t index) {
        YGNodeInsertChild(node_, child.node(), index);
    }

    void Layout::add_child(Layout &child) {
        auto childCount = YGNodeGetChildCount(node_);
        add_child(child, childCount);
    }

    void Layout::calculate_layout(float width, float height) {
        YGNodeCalculateLayout(node_, width, height, YGDirectionLTR);
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
        return parent_;
    }
}
