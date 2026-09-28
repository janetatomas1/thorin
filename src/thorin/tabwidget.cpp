#include <algorithm>

#include <imgui_internal.h>
#include <libassert/assert.hpp>

#include "thorin/tabwidget.hpp"
#include "thorin/thorin.hpp"

using namespace thorin::literals;

namespace thorin {
    TabWidget::TabBar::TabBar(TabWidget* parent): Widget("", parent), tabWidget(parent) {
        layout().enable_measure();
    }

    bool TabWidget::TabBar::show() {
        bool changed = false;
        if (ImGui::BeginTabBar(title_id().c_str(), flags_)){
            auto tabs = tabWidget->items();
            size_t index = 0;
            for (const auto& tab : tabs) {
                // selected_ is the source of truth: ImGui is told every frame (a no-op once it
                // matches), so set_selected() needs nothing else to switch the tab.
                const ImGuiTabItemFlags itemFlags = index == selected_
                    ? ImGuiTabItemFlags_SetSelected
                    : ImGuiTabItemFlags_None;

                if (ImGui::BeginTabItem(tab.title_id().c_str(), nullptr, itemFlags)) {
                    ImGui::EndTabItem();
                }

                // Deferred: the page boxes for this frame are already laid out, and the switch
                // must land before the next frame passes SetSelected for the old tab.
                if (ImGui::IsItemClicked() && index != selected_) {
                    app().add_action([tabWidget = tabWidget, index] {
                        tabWidget->display_page(index);
                    });
                    changed = true;
                }
                index++;
            }

            ImGui::EndTabBar();
        }

        return changed;
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

    void TabWidget::display_page(size_t index) {
        if (index == tabBar.selected_) {
            return;
        }

        items()[tabBar.selected_].display(YGDisplayNone);
        tabBar.selected_ = index;
        items()[tabBar.selected_].display(YGDisplayFlex);
    }

    void TabWidget::attach_page(std::unique_ptr<Widget> page) {
        app().add_action([this, page = std::move(page)]() mutable {
            auto& ref = adopt(std::move(page));
            ref.set_parent(this);
            // The first page starts selected; later ones stay hidden until chosen.
            if (count() > 1) {
                ref.display(YGDisplayNone);
            }
            // A new tab widens the strip.
            tabBar.layout().mark_dirty();
        });
    }

    void TabWidget::remove_tab(size_t index) {
        app().add_action([this, index] {
            DEBUG_ASSERT(index < count(), "TabWidget::remove_tab index out of range", index, count());

            size_t& selected = tabBar.selected_;
            const bool wasSelected = index == selected;
            remove(index);

            // Keep the same page selected; if it was the removed one, its successor (or the new
            // last page) takes over and has to be shown.
            if (index < selected) {
                --selected;
            } else if (wasSelected && count() > 0) {
                selected = std::min(selected, count() - 1);
                at(selected).display(YGDisplayFlex);
            } else if (count() == 0) {
                selected = 0;
            }

            tabBar.layout().mark_dirty();
        });
    }

    TabWidget& TabWidget::set_selected(size_t index) {
        DEBUG_ASSERT(index < count(), "TabWidget::set_selected index out of range", index, count());
        display_page(index);
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

        if (count() > 0) {
            items()[tabBar.selected_].render();
        }

        return false;
    }
}
