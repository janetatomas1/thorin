
#pragma once

#include "thorin/menu.hpp"

namespace thorin {
    // A horizontal bar of menus. Yoga places and sizes the bar (full parent width by
    // default, frame height); the menus themselves are drawn in ImGui's menu bar flow.
    class MenuBar : public Widget, public Container<Menu> {
        // Menus are added through add_menu(), which also re-measures the bar.
        using Container::add;

    public:
        MenuBar(Widget* parent = nullptr);

        // Returned reference stays valid for the MenuBar's lifetime.
        Menu& add_menu(const std::string& title);

        bool show() override;
        ImVec2 measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) override;
    };
}
