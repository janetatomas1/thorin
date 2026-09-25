
#pragma once

#include <memory>
#include <vector>

#include "thorin/menu.hpp"

namespace thorin {
    // A horizontal bar of menus. Yoga places and sizes the bar (full parent width by
    // default, frame height); the menus themselves are drawn in ImGui's menu bar flow.
    class MenuBar : public Widget {
        std::vector<std::unique_ptr<Menu>> menus_;

    public:
        MenuBar(Widget* parent = nullptr);

        // Returned reference stays valid for the MenuBar's lifetime.
        Menu& add_menu(const std::string& title);

        [[nodiscard]] size_t count() const;
        Menu& at(size_t index);

        bool show() override;
        ImVec2 measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) override;
    };
}
