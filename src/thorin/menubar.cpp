
#include <algorithm>

#include <libassert/assert.hpp>

#include "thorin/menubar.hpp"

using namespace thorin::literals;

namespace thorin {
    MenuBar::MenuBar(Widget* parent)
    : Widget("", parent) {
        layout().enable_measure();
        width(100_pcts);
    }

    Menu& MenuBar::add_menu(const std::string& title) {
        auto menu = std::make_unique<Menu>(title);
        auto& ref = *menu;
        menus_.push_back(std::move(menu));
        layout().mark_dirty();
        return ref;
    }

    size_t MenuBar::count() const {
        return menus_.size();
    }

    Menu& MenuBar::at(size_t index) {
        DEBUG_ASSERT(index < menus_.size(), "MenuBar::at index out of range", index, menus_.size());
        return *menus_[index];
    }

    bool MenuBar::show() {
        bool clicked = false;

        // BeginMenuBar needs a window with ImGuiWindowFlags_MenuBar, so the bar gets its own
        // child window sized to this widget's box.
        ImGui::BeginChild(
            title_id().c_str(),
            size(),
            ImGuiChildFlags_None,
            ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
        );

        if (ImGui::BeginMenuBar()) {
            for (auto& menu : menus_) {
                clicked |= menu->show();
            }

            ImGui::EndMenuBar();
        }

        ImGui::EndChild();
        return clicked;
    }

    ImVec2 MenuBar::measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
        // Mirrors ImGui's menu bar: MenuBarOffset.x on both ends, and each header advances by
        // its label plus the doubled ItemSpacing.x BeginMenu pushes inside a menu bar.
        const ImGuiStyle& style = ImGui::GetStyle();
        float intrinsic = std::max(style.WindowPadding.x, style.ItemSpacing.x) * 2.0f;

        for (const auto& menu : menus_) {
            intrinsic += ImGui::CalcTextSize(menu->title_id().c_str(), nullptr, true).x + style.ItemSpacing.x * 2.0f;
        }

        return ImVec2{
            fit_measure(intrinsic, width, widthMode),
            fit_measure(ImGui::GetFrameHeight(), height, heightMode)
        };
    }
}
