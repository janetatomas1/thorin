
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
        Widget* parent_ = nullptr;

        Layout layout_;
    public:
        Widget(const std::string &title = "", Widget *parent = nullptr);
        Widget(const Widget& widget) = delete;
        Widget &operator=(const Widget& widget) = delete;
        virtual ~Widget() = default;
        virtual void init() {}
        virtual void destroy() {}
        [[nodiscard]] uint64_t id() const;
        [[nodiscard]] std::string title() const;
        [[nodiscard]] std::string title_id() const;
        bool render();
        virtual bool show();
        Window* window();
        void set_window(Window* window);
        Widget* parent();
        void set_parent(Widget* parent);
        Thorin& app();
        Layout& layout();

        Widget& set_margin(LayoutValue value, YGEdge edge = YGEdgeAll);
        Widget& set_margin(float points, YGEdge edge = YGEdgeAll);

        Widget& set_padding(LayoutValue value, YGEdge edge = YGEdgeAll);
        Widget& set_padding(float points, YGEdge edge = YGEdgeAll);

        Widget& set_position(LayoutValue value, YGEdge edge);
        Widget& set_position(float points, YGEdge edge);

        Widget& set_width(LayoutValue value);
        Widget& set_width(float points);
        Widget& set_height(LayoutValue value);
        Widget& set_height(float points);

        Widget& set_gap(LayoutValue value, YGGutter gutter = YGGutterAll);
        Widget& set_gap(float points, YGGutter gutter = YGGutterAll);

        Widget& set_flex_direction(YGFlexDirection direction);
        Widget& set_flex_grow(float value);
        Widget& set_flex_shrink(float value);
        Widget& set_flex_basis(LayoutValue value);
        Widget& set_flex_basis(float points);
        Widget& set_flex(float value);
        Widget& set_flex_wrap(YGWrap wrap);

        Widget& set_align_items(YGAlign align);
        Widget& set_align_self(YGAlign align);
        Widget& set_align_content(YGAlign align);
        Widget& set_justify_content(YGJustify justify);

        Widget& set_min_width(LayoutValue value);
        Widget& set_min_width(float points);
        Widget& set_min_height(LayoutValue value);
        Widget& set_min_height(float points);
        Widget& set_max_width(LayoutValue value);
        Widget& set_max_width(float points);
        Widget& set_max_height(LayoutValue value);
        Widget& set_max_height(float points);

        Widget& set_border(float width, YGEdge edge = YGEdgeAll);
        Widget& set_display(YGDisplay display);
        Widget& set_overflow(YGOverflow overflow);
        Widget& set_aspect_ratio(float ratio);
        Widget& set_direction(YGDirection direction);
        Widget& set_position_type(YGPositionType type);

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
