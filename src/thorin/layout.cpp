
#include "thorin/layout.hpp"


namespace thorin {
    Layout::Layout(): node_(YGNodeNew()) {
    }

    Layout::~Layout() {
        auto parent = YGNodeGetParent(node_);
        YGNodeRemoveChild(parent, node_);
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
}
