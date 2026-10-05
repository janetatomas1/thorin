
#pragma once

#include <cstdint>
#include <string>
#include <concepts>
#include <unordered_map>

#include "thorin/layout.hpp"
#include "thorin/style.hpp"

namespace thorin {
    class Window;
    class Thorin;

    class Widget {
        uint64_t id_ = 0;
        std::string title_;
        std::string titleID_;
        std::string tooltip_;
        bool enabled_ = true;

        Window* window_ = nullptr;

        Layout layout_;
        Style style_{this};

        // Live widgets by id, for post() and find().
        static inline std::unordered_map<uint64_t, Widget*> registry_;

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

        // One-line frame widgets take no height from ImGui: they are drawn at GetFrameHeight().
        // When this widget's box is taller or shorter, push FramePadding.y so the frame fills it.
        // Returns whether a style var was pushed; pass the result to pop_frame_height().
        [[nodiscard]] bool push_frame_height() const;
        static void pop_frame_height(bool pushed);
        // Frame height push_frame_height() will draw at, for measuring frame-height squares
        // (combo arrow, color swatch) when Yoga passes an exact height.
        [[nodiscard]] static float frame_height(float height, YGMeasureMode heightMode);

    public:
        Widget(const std::string &title = "", Widget *parent = nullptr);

        Widget(const Widget& other) = delete;
        Widget &operator=(const Widget& other) = delete;

        Widget(Widget&& other) noexcept;
        Widget& operator=(Widget&& other) noexcept;

        virtual ~Widget();
        // Default: calls init() on the children in Yoga order, destroy() in reverse order.
        // Overrides call the base version to keep reaching the children.
        virtual void init();
        virtual void destroy();
        virtual ImVec2 measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode);
        [[nodiscard]] uint64_t id() const;
        // The live widget with this id, or nullptr once it is destroyed.
        [[nodiscard]] static Widget* find(uint64_t id);

        // Runs fn on the next dispatch with this widget, as its own type, if it still exists then;
        // the action is dropped if the widget was destroyed meanwhile. Actions run in call order.
        // A moved widget keeps its id, so fn gets the moved-to object. Use self in fn, never a
        // captured this: without this captured, any member access through it doesn't compile.
        template <class Self, std::invocable<Self&> F>
        void post(this Self& self, F fn) {
            self.app().add_action([id = self.id(), fn = std::move(fn)]() mutable {
                if (Widget* widget = find(id)) {
                    fn(static_cast<Self&>(*widget));
                }
            });
        }
        [[nodiscard]] const std::string& title() const;
        [[nodiscard]] const std::string& title_id() const;
        Widget& set_title(const std::string& title);
        // Text shown while the widget's box is hovered. Empty means no tooltip.
        [[nodiscard]] const std::string& tooltip() const;
        Widget& set_tooltip(const std::string& tooltip);
        // A disabled widget is greyed out and ignores input; so is everything drawn inside it.
        [[nodiscard]] bool enabled() const;
        Widget& set_enabled(bool enabled);
        // Positions the widget at its Yoga box, then draw()s it.
        bool render();
        // show() with the per-widget state applied (disabled, tooltip). Owners that draw
        // widgets outside the Yoga tree (menus, table cells) call this instead of render().
        bool draw();
        // Default: renders the children in Yoga order. Leaves override it to draw themselves.
        virtual bool show();
        Window* window();
        void set_window(Window* window);
        Widget* parent();
        void set_parent(Widget* parent);
        Thorin& app();
        Layout& layout();
        // Pushed around show(), so it applies to the children too; measure sees it as well.
        Style& style();

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

        // Style values as set.
        [[nodiscard]] float gap(YGGutter gutter = YGGutterAll) const;
        [[nodiscard]] YGFlexDirection flex_direction() const;
        [[nodiscard]] float flex_grow() const;
        [[nodiscard]] float flex_shrink() const;
        [[nodiscard]] LayoutValue flex_basis() const;
        [[nodiscard]] float flex() const;
        [[nodiscard]] YGWrap flex_wrap() const;
        [[nodiscard]] YGAlign align_items() const;
        [[nodiscard]] YGAlign align_self() const;
        [[nodiscard]] YGAlign align_content() const;
        [[nodiscard]] YGJustify justify_content() const;
        [[nodiscard]] LayoutValue min_width() const;
        [[nodiscard]] LayoutValue min_height() const;
        [[nodiscard]] LayoutValue max_width() const;
        [[nodiscard]] LayoutValue max_height() const;
        [[nodiscard]] YGDisplay display() const;
        [[nodiscard]] YGOverflow overflow() const;
        [[nodiscard]] float aspect_ratio() const;
        [[nodiscard]] YGDirection direction() const;
        [[nodiscard]] YGPositionType position_type() const;

        [[nodiscard]] float x() const;
        [[nodiscard]] float y() const;
        [[nodiscard]] float width() const;
        [[nodiscard]] float height() const;
        [[nodiscard]] const ImVec2& position() const;
        [[nodiscard]] const ImVec2& size() const;
        [[nodiscard]] float margin(YGEdge edge) const;
        [[nodiscard]] float padding(YGEdge edge) const;
        [[nodiscard]] float border(YGEdge edge) const;
    };


    template <class W>
    concept WidgetConcept = std::derived_from<W, Widget>;
}
