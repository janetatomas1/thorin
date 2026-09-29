#pragma once

#include "thorin/widget.hpp"

namespace thorin {
    // An ImGui modal popup at this widget's Yoga box. It is absolutely positioned, so it takes
    // no space in its parent, and drawn above everything, blocking input to the rest of the window.
    // Children are ordinary Yoga children, laid out inside its padding; the layout is an origin,
    // so their positions are relative to the modal. The Yoga border is managed here (title bar
    // + border line); use padding for spacing.
    // The popup is identified by title_id(), so renaming the modal closes it.
    class Modal : public Widget {
        // Yoga owns the box: moving or resizing would be undone the next frame.
        ImGuiWindowFlags flags_ = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
        // Wanted state; show() opens or closes the ImGui popup to match it.
        bool open_ = false;

    public:
        Modal(const std::string& title = "", Widget* parent = nullptr);

        Modal& set_window_flags(ImGuiWindowFlags flags);
        [[nodiscard]] ImGuiWindowFlags flags() const;

        // Take effect on the next show().
        void open();
        void close();
        [[nodiscard]] bool is_open() const;

        bool show() override;
    };
}
