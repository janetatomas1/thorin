
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
        ImVec2 size_ = {0.0f, 0.0f};

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

        float width();
        float height();
        const ImVec2 &size();

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

        Layout& set_flex_grow(float value);
        Layout& set_flex_shrink(float value);
        Layout& set_flex_basis(LayoutValue value);
        Layout& set_flex_basis(float points);
        Layout& set_flex(float value);
        Layout& set_flex_wrap(YGWrap wrap);

        Layout& set_align_items(YGAlign align);
        Layout& set_align_self(YGAlign align);
        Layout& set_align_content(YGAlign align);
        Layout& set_justify_content(YGJustify justify);

        Layout& set_min_width(LayoutValue value);
        Layout& set_min_width(float points);
        Layout& set_min_height(LayoutValue value);
        Layout& set_min_height(float points);
        Layout& set_max_width(LayoutValue value);
        Layout& set_max_width(float points);
        Layout& set_max_height(LayoutValue value);
        Layout& set_max_height(float points);

        Layout& set_border(float width, YGEdge edge = YGEdgeAll);
        Layout& set_display(YGDisplay display);
        Layout& set_overflow(YGOverflow overflow);
        Layout& set_aspect_ratio(float ratio);
        Layout& set_direction(YGDirection direction);

        Layout& fill_parent();
        Layout& row(float gap = 0.0f);
        Layout& column(float gap = 0.0f);
        Layout& center();

        float computed_margin(YGEdge edge);
        float computed_padding(YGEdge edge);
        float computed_border(YGEdge edge);
    };
}
