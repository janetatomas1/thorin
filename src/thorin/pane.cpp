#include <imgui_internal.h>

#include "thorin/pane.hpp"

namespace thorin {
    Pane::Pane(const std::string& title, Widget* parent): Widget(title, parent) {
        layout().set_origin(true);
        position_type(YGPositionTypeAbsolute);
    }

    Pane& Pane::set_initial_dock(ImGuiDir side, float ratio) {
        initialSide_ = side;
        initialRatio_ = ratio;
        return *this;
    }

    ImGuiDir Pane::initial_side() const {
        return initialSide_;
    }

    float Pane::initial_ratio() const {
        return initialRatio_;
    }

    Pane& Pane::set_dock_flags(ImGuiDockNodeFlags flags) {
        dockFlags_ = flags;
        return *this;
    }

    ImGuiDockNodeFlags Pane::dock_flags() const {
        return dockFlags_;
    }

    Pane& Pane::set_window_flags(ImGuiWindowFlags flags) {
        windowFlags_ = flags;
        return *this;
    }

    ImGuiWindowFlags Pane::window_flags() const {
        return windowFlags_;
    }

    bool Pane::show() {
        if (dockFlags_ != ImGuiDockNodeFlags_None) {
            ImGuiWindowClass windowClass;
            windowClass.DockNodeFlagsOverrideSet = dockFlags_;
            ImGui::SetNextWindowClass(&windowClass);
        }

        // Yoga padding is the only inset, as in Popup and ChildWindow. The ids change every run,
        // so saving the window's settings is pointless.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
        const bool visible = ImGui::Begin(title_id().c_str(), nullptr, windowFlags_ | ImGuiWindowFlags_NoSavedSettings);
        ImGui::PopStyleVar();

        bool changed = false;
        if (visible) {
            // The pane's box is the whole window; local (0, 0) is its top-left corner, so the
            // title bar (floating) or tab bar (docked) is kept clear with the top border.
            const ImGuiWindow* window = ImGui::GetCurrentWindow();
            border(window->DecoOuterSizeY1, YGEdgeTop);

            changed = Widget::show();
        }

        // Unlike most Begin/End pairs, End is required even when Begin returns false.
        ImGui::End();
        return changed;
    }
}
