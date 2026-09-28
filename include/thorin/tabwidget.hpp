#pragma once

#include <string>

#include "thorin/container.hpp"

namespace thorin {
    // Pages with a tabbar above them, one tab per page. The strip is the first Yoga
    // child, the pages follow; only the selected page is displayed, the others get
    // display(YGDisplayNone), so Yoga gives them no space.
    // Container is protected: pages are added through add_tab(), which parents them to the widget.
    class TabWidget : public Widget, protected Container<Widget> {
        struct TabBar: Widget {
            ImGuiTabBarFlags flags_ = ImGuiTabBarFlags_None;
            TabWidget *tabWidget;
            size_t selected_ = 0;
            std::string groupID;

            TabBar(TabWidget* parent);
            bool show() override;
            ImVec2 measure(
                float width,
                YGMeasureMode widthMode,
                float height,
                YGMeasureMode heightMode
            ) override;
        };

        TabBar tabBar = TabBar(this);

    public:
        using Container::count;
        using Container::at;
        using Container::items;

        TabWidget(Widget* parent = nullptr);

        // Adds a page; its title() is the tab label. Returned reference stays valid for the TabWidget's lifetime.
        template <WidgetConcept W = Widget, class... Args>
        W& add_tab(Args&&... args) {
            auto& page = add<W>(std::forward<Args>(args)...);
            page.set_parent(this);
            // A new tab widens the strip.
            tabBar.layout().mark_dirty();
            return page;
        }

        [[nodiscard]] size_t selected() const;
        TabWidget& set_selected(size_t index);

        TabWidget& set_flags(ImGuiTabBarFlags flags);
        [[nodiscard]] ImGuiTabBarFlags flags() const;

        bool show() override;
    };
}
