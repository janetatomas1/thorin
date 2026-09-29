#include <imgui_internal.h>

#include "thorin/modal.hpp"

namespace thorin {
    Modal::Modal(const std::string& title, Widget* parent): Widget(title, parent) {
        layout().set_origin(true);
        position_type(YGPositionTypeAbsolute);
    }

    Modal& Modal::set_window_flags(ImGuiWindowFlags flags) {
        flags_ = flags;
        return *this;
    }

    ImGuiWindowFlags Modal::flags() const {
        return flags_;
    }

    void Modal::open() {
        pendingOpen_ = true;
        pendingClose_ = false;
    }

    void Modal::close() {
        pendingClose_ = true;
        pendingOpen_ = false;
    }

    bool Modal::is_open() const {
        return open_;
    }

    bool Modal::show() {
        // OpenPopup and BeginPopupModal must see the same ID stack, so both happen here.
        if (pendingOpen_) {
            ImGui::OpenPopup(title_id().c_str());
            pendingOpen_ = false;
        }

        // render() put the cursor at this widget's box; the popup is a separate window,
        // so its position is given in screen space.
        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
        ImGui::SetNextWindowSize(size());
        // No item follows that cursor move in the parent window (the modal takes no space there),
        // which ImGui would report as using SetCursorPos to extend the window's boundaries.
        ImGui::GetCurrentWindow()->DC.IsSetPos = false;

        // Yoga padding is the only inset: SetCursorPos ignores WindowPadding, but ImGui would
        // still shrink the clip rect by it.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
        open_ = ImGui::BeginPopupModal(title_id().c_str(), nullptr, flags_);
        ImGui::PopStyleVar();

        if (!open_) {
            pendingClose_ = false;
            return false;
        }

        // Local (0, 0) is the window's top-left corner, under the title bar: keep children below it.
        const ImGuiWindow* window = ImGui::GetCurrentWindow();
        const float borderSize = ImGui::GetStyle().PopupBorderSize;
        border(borderSize, YGEdgeAll);
        border(borderSize + window->DecoOuterSizeY1, YGEdgeTop);

        const bool changed = Widget::show();

        if (pendingClose_) {
            ImGui::CloseCurrentPopup();
            pendingClose_ = false;
        }

        ImGui::EndPopup();
        return changed;
    }
}
