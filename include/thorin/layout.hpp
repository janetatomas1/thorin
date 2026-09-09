
#pragma once

#include <yoga/Yoga.h>
#include <imgui.h>

namespace thorin {
    class Layout {
        YGNodeRef node_ = nullptr;
        Layout* parent_;

        ImVec2 position_ = {0.0f, 0.0f};
    public:
        Layout();
        ~Layout();
        YGNodeRef node();
        void add_child(Layout &child, size_t index);
        void add_child(Layout &child);
        void calculate_layout(float width, float height);
        float x();
        float y();
        const ImVec2 &position();
        Layout *parent();
    };
}
