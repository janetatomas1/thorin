
#pragma once

#include <yoga/Yoga.h>
#include <imgui.h>
#include <string>

namespace thorin {
    using LayoutValue = YGValue;

    constexpr LayoutValue points(float v){
        return LayoutValue{v, YGUnitPoint};
    }

    constexpr LayoutValue percent(float v) {
        return LayoutValue{v, YGUnitPercent};
    }
    constexpr LayoutValue auto_() {
        return LayoutValue{0.0f, YGUnitAuto};
    }

    namespace literals {
        constexpr LayoutValue operator""_pts(long double v) {
            return points(static_cast<float>(v));
        }

        constexpr LayoutValue operator""_pts(unsigned long long v) {
            return points(static_cast<float>(v));
        }

        constexpr LayoutValue operator""_pcts(long double v) {
            return percent(static_cast<float>(v));
        }

        constexpr LayoutValue operator""_pcts(unsigned long long v) {
            return percent(static_cast<float>(v));
        }
    }

    class Layout {
        YGNodeRef node_ = nullptr;
        ImVec2 position_ = {0.0f, 0.0f};
        ImVec2 size_ = {0.0f, 0.0f};

    public:
        Layout();
        ~Layout();

        Layout(Layout&& other) noexcept;
        Layout& operator=(Layout&& other) noexcept;

        Layout(const Layout& other) = delete;
        Layout& operator=(const Layout& other) = delete;

        YGNodeRef node();
        void add_child(Layout &child, size_t index = std::string::npos);
        void remove_from_parent();
        void calculate_layout(float width, float height);
        void calculate_position();

        [[nodiscard]] float x() const;
        [[nodiscard]] float y() const;
        [[nodiscard]] const ImVec2 &position() const;

        [[nodiscard]] float width() const;
        [[nodiscard]] float height() const;
        [[nodiscard]] const ImVec2 &size() const;

        Layout *parent();

        Layout& margin(LayoutValue value, YGEdge edge = YGEdgeAll);
        Layout& margin(float points, YGEdge edge = YGEdgeAll);

        Layout& padding(LayoutValue value, YGEdge edge = YGEdgeAll);
        Layout& padding(float points, YGEdge edge = YGEdgeAll);

        Layout& position(LayoutValue value, YGEdge edge);
        Layout& position(float points, YGEdge edge);

        Layout& width(LayoutValue value);
        Layout& width(float points);
        Layout& height(LayoutValue value);
        Layout& height(float points);

        Layout& gap(LayoutValue value, YGGutter gutter = YGGutterAll);
        Layout& gap(float points, YGGutter gutter = YGGutterAll);

        Layout& flex_direction(YGFlexDirection direction);
        Layout& position_type(YGPositionType type);

        Layout& flex_grow(float value);
        Layout& flex_shrink(float value);
        Layout& flex_basis(LayoutValue value);
        Layout& flex_basis(float points);
        Layout& flex(float value);
        Layout& flex_wrap(YGWrap wrap);

        Layout& align_items(YGAlign align);
        Layout& align_self(YGAlign align);
        Layout& align_content(YGAlign align);
        Layout& justify_content(YGJustify justify);

        Layout& min_width(LayoutValue value);
        Layout& min_width(float points);
        Layout& min_height(LayoutValue value);
        Layout& min_height(float points);
        Layout& max_width(LayoutValue value);
        Layout& max_width(float points);
        Layout& max_height(LayoutValue value);
        Layout& max_height(float points);

        Layout& border(float width, YGEdge edge = YGEdgeAll);
        Layout& display(YGDisplay display);
        Layout& overflow(YGOverflow overflow);
        Layout& aspect_ratio(float ratio);
        Layout& direction(YGDirection direction);

        Layout& fill_parent();
        Layout& row(float gap = 0.0f);
        Layout& column(float gap = 0.0f);
        Layout& center();

        float computed_margin(YGEdge edge);
        float computed_padding(YGEdge edge);
        float computed_border(YGEdge edge);
    };
}
