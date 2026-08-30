
#pragma once

#include <yoga/Yoga.h>

namespace thorin {
    class Layout {
        YGNodeRef node_ = nullptr;

    public:
        Layout();
        ~Layout();
        YGNodeRef node();
        void add_child(Layout &child, size_t index);
        void add_child(Layout &child);
        void calculate_layout(float width, float height);
    };
}
