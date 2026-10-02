#pragma once

#include "thorin/widget.hpp"

namespace thorin {
    class DockArea;

    // A panel drawn in its own ImGui window, titled with title() (the tab label when docked).
    // Added to a DockArea, it can be dragged, tabbed with other panes, split and floated.
    // Like Popups, panes are absolutely positioned origin layouts: they take no space in their
    // parent, and each frame they are sized to their window, so their children are laid out inside
    // it. A resize reaches Yoga on the next frame. The window scrolls when children overflow it.
    // Subclass it and add children as usual; use padding for spacing.
    class Pane : public Widget {
        friend class DockArea;

        ImGuiDockNodeFlags dockFlags_ = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags windowFlags_ = ImGuiWindowFlags_None;
        ImGuiDir initialSide_ = ImGuiDir_None;
        float initialRatio_ = 0.25f;
        // Set by DockArea once the pane has been docked into it.
        bool placed_ = false;

    public:
        Pane(const std::string& title = "", Widget* parent = nullptr);

        // Where the pane starts in its DockArea: attached to the side edge (ImGuiDir_Left, ...),
        // ratio of the whole area wide (left/right) or tall (up/down); or ImGuiDir_None (the
        // default) for a tab in the middle. Sided panes are split off in add order, each from what
        // the earlier ones left, so the first one spans the whole edge. Only read when the area
        // first shows; later the user decides where panes are.
        Pane& set_initial_dock(ImGuiDir side, float ratio = 0.25f);
        [[nodiscard]] ImGuiDir initial_side() const;
        [[nodiscard]] float initial_ratio() const;

        // Flags for the dock node hosting this pane, while it is docked. Besides the public
        // ImGuiDockNodeFlags, the ones in imgui_internal.h apply here, e.g. ImGuiDockNodeFlags_NoTabBar
        // (the pane can't be dragged out) or ImGuiDockNodeFlags_NoDocking (nothing docks over or
        // splits it).
        Pane& set_dock_flags(ImGuiDockNodeFlags flags);
        [[nodiscard]] ImGuiDockNodeFlags dock_flags() const;

        // ImGuiWindowFlags for the pane's window, e.g. ImGuiWindowFlags_NoScrollbar.
        // ImGuiWindowFlags_NoSavedSettings is always added.
        Pane& set_window_flags(ImGuiWindowFlags flags);
        [[nodiscard]] ImGuiWindowFlags window_flags() const;

        bool show() override;
    };
}
