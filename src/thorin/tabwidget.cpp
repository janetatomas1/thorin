#include <imgui_internal.h>
#include <libassert/assert.hpp>

#include "thorin/tabwidget.hpp"

using namespace thorin::literals;

namespace thorin {
    TabWidget::TabBar::TabBar(TabWidget* parent): Widget("", parent), tabWidget(parent) {
        layout().enable_measure();
    }

    bool TabWidget::TabBar::show() {
        size_t before = selected_;
        if (ImGui::BeginTabBar(title_id().c_str(), flags_)){
            auto tabs = tabWidget->items();
            size_t index = 0;
            for (const auto& tab : tabs) {
                if (ImGui::BeginTabItem(tab.title_id().c_str())) {
                    ImGui::EndTabItem();
                    selected_ = index;
                }
                index++;
            }

            ImGui::EndTabBar();
            return before == selected_;
        }

        return false;
    }

    ImVec2 TabWidget::TabBar::measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
        // Intrinsic width is the tabs side by side, as BeginTabBar lays them out without
        // shrinking; tab items are frame height tall.
        float tabsWidth = 0.0f;
        size_t count = 0;
        for (const auto& tab : tabWidget->items()) {
            tabsWidth += ImGui::TabItemCalcSize(tab.title_id().c_str(), false).x;
            ++count;
        }

        if (count > 1) {
            tabsWidth += ImGui::GetStyle().ItemInnerSpacing.x * static_cast<float>(count - 1);
        }

        // The list button sits left of the tabs, at the width TabBarTabListPopupButton() takes.
        // Scroll buttons and resize-down only apply when the tabs don't fit, so they don't count.
        if (flags_ & ImGuiTabBarFlags_TabListPopupButton) {
            tabsWidth += ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y;
        }

        return ImVec2{
            fit_measure(tabsWidth, width, widthMode),
            fit_measure(ImGui::GetFrameHeight(), height, heightMode)
        };
    }

    TabWidget::TabWidget(Widget* parent): Widget("", parent) {}

    size_t TabWidget::selected() const {
        return tabBar.selected_;
    }

    TabWidget& TabWidget::set_selected(size_t index) {
        tabBar.selected_ = index;
        return *this;
    }

    TabWidget& TabWidget::set_flags(ImGuiTabBarFlags flags) {
        tabBar.flags_ = flags;
        tabBar.layout().mark_dirty();
        return *this;
    }

    ImGuiTabBarFlags TabWidget::flags() const {
        return tabBar.flags_;
    }

    bool TabWidget::show() {
        tabBar.render();
        items()[tabBar.selected_].render();
        return false;
    }
}
