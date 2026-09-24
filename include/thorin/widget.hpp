
#pragma once

#include <cstdint>
#include <string>
#include <concepts>

#include "thorin/layout.hpp"

namespace thorin {
    class Window;
    class Thorin;

    class Widget {
        uint64_t id_ = 0;
        std::string title_;
        std::string titleID_;

        Window* window_ = nullptr;

        Layout layout_;

    protected:
        // Width of the visible label (text after "##" hidden), 0 when there is none. ImGui context required.
        [[nodiscard]] float label_width() const;
        // Space ImGui puts after a field for its label: ItemInnerSpacing.x + label, or 0 when there is none.
        [[nodiscard]] float label_extent() const;
        // Intrinsic field width for frame widgets that have no natural width (Slider, Drag, ColorEdit, ...).
        [[nodiscard]] static float default_field_width();

        // Measure for a one-line frame widget: default field width + labelExtent, frame height.
        [[nodiscard]] static ImVec2 measure_field(
            float labelExtent,
            float width,
            YGMeasureMode widthMode,
            float height,
            YGMeasureMode heightMode
        );
        // SetNextItemWidth for the field part of this widget's box, leaving labelExtent for the label.
        void set_next_field_width(float labelExtent) const;

    public:
        Widget(const std::string &title = "", Widget *parent = nullptr);

        Widget(const Widget& other) = delete;
        Widget &operator=(const Widget& other) = delete;

        Widget(Widget&& other) noexcept;
        Widget& operator=(Widget&& other) noexcept;

        virtual ~Widget() = default;
        virtual void init() {}
        virtual void destroy() {}
        virtual ImVec2 measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);
        [[nodiscard]] uint64_t id() const;
        [[nodiscard]] const std::string& title() const;
        [[nodiscard]] const std::string& title_id() const;
        Widget& set_title(const std::string& title);
        bool render();
        virtual bool show();
        Window* window();
        void set_window(Window* window);
        Widget* parent();
        void set_parent(Widget* parent);
        Thorin& app();
        Layout& layout();

        Widget& margin(LayoutValue value, YGEdge edge = YGEdgeAll);
        Widget& margin(float points, YGEdge edge = YGEdgeAll);

        Widget& padding(LayoutValue value, YGEdge edge = YGEdgeAll);
        Widget& padding(float points, YGEdge edge = YGEdgeAll);

        Widget& position(LayoutValue value, YGEdge edge);
        Widget& position(float points, YGEdge edge);

        Widget& width(LayoutValue value);
        Widget& width(float points);
        Widget& height(LayoutValue value);
        Widget& height(float points);

        Widget& gap(LayoutValue value, YGGutter gutter = YGGutterAll);
        Widget& gap(float points, YGGutter gutter = YGGutterAll);

        Widget& flex_direction(YGFlexDirection direction);
        Widget& flex_grow(float value);
        Widget& flex_shrink(float value);
        Widget& flex_basis(LayoutValue value);
        Widget& flex_basis(float points);
        Widget& flex(float value);
        Widget& flex_wrap(YGWrap wrap);

        Widget& align_items(YGAlign align);
        Widget& align_self(YGAlign align);
        Widget& align_content(YGAlign align);
        Widget& justify_content(YGJustify justify);

        Widget& min_width(LayoutValue value);
        Widget& min_width(float points);
        Widget& min_height(LayoutValue value);
        Widget& min_height(float points);
        Widget& max_width(LayoutValue value);
        Widget& max_width(float points);
        Widget& max_height(LayoutValue value);
        Widget& max_height(float points);

        Widget& border(float width, YGEdge edge = YGEdgeAll);
        Widget& display(YGDisplay display);
        Widget& overflow(YGOverflow overflow);
        Widget& aspect_ratio(float ratio);
        Widget& direction(YGDirection direction);
        Widget& position_type(YGPositionType type);

        Widget& fill_parent();
        Widget& row(float gap = 0.0f);
        Widget& column(float gap = 0.0f);
        Widget& center();

        [[nodiscard]] float x() const;
        [[nodiscard]] float y() const;
        [[nodiscard]] float width() const;
        [[nodiscard]] float height() const;
        [[nodiscard]] const ImVec2& position() const;
        [[nodiscard]] const ImVec2& size() const;
    };


    template <class W>
    concept WidgetConcept = std::derived_from<W, Widget>;
}
