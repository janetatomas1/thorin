
#pragma once

#include <yoga/Yoga.h>
#include <imgui.h>
#include <string>

namespace thorin {
    using LayoutValue = YGValue;
    
    constexpr LayoutValue points(float v)  { return LayoutValue{v, YGUnitPoint}; }
    constexpr LayoutValue percent(float v) { return LayoutValue{v, YGUnitPercent}; }
    constexpr LayoutValue auto_()          { return LayoutValue{0.0f, YGUnitAuto}; }

    class Layout {
        YGNodeRef node_ = nullptr;
        ImVec2 position_ = {0.0f, 0.0f};

    public:
        Layout();
        ~Layout();

        YGNodeRef node();
        void add_child(Layout &child, size_t index = std::string::npos);
        void calculate_layout(float width, float height);
        void calculate_position();
        float x();
        float y();
        const ImVec2 &position();
        Layout *parent();

        Layout& set_margin(LayoutValue value, YGEdge edge = YGEdgeAll);
        Layout& set_margin(float points, YGEdge edge = YGEdgeAll);

        Layout& set_padding(LayoutValue value, YGEdge edge = YGEdgeAll);
        Layout& set_padding(float points, YGEdge edge = YGEdgeAll);

        Layout& set_position(LayoutValue value, YGEdge edge);
        Layout& set_position(float points, YGEdge edge);

        Layout& set_width(LayoutValue value);
        Layout& set_width(float points);
        Layout& set_height(LayoutValue value);
        Layout& set_height(float points);

        Layout& set_gap(LayoutValue value, YGGutter gutter = YGGutterAll);
        Layout& set_gap(float points, YGGutter gutter = YGGutterAll);

        Layout& set_flex_direction(YGFlexDirection direction);
        Layout& set_position_type(YGPositionType type);
    };
}
