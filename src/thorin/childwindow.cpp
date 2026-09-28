#include <algorithm>

#include <imgui_internal.h>

#include "thorin/childwindow.hpp"

namespace thorin {
    ChildWindow::ChildWindow(const std::string& title, Widget* parent): Widget(title, parent) {
        layout().set_origin(true);
        overflow(YGOverflowScroll);
    }

    ChildWindow& ChildWindow::set_child_flags(ImGuiChildFlags flags) {
        childFlags_ = flags;
        return *this;
    }

    ImGuiChildFlags ChildWindow::child_flags() const {
        return childFlags_;
    }

    ChildWindow& ChildWindow::set_window_flags(ImGuiWindowFlags flags) {
        windowFlags_ = flags;
        return *this;
    }

    ImGuiWindowFlags ChildWindow::window_flags() const {
        return windowFlags_;
    }

    bool ChildWindow::show() {
        const ImGuiStyle& style = ImGui::GetStyle();
        const float borderSize = (childFlags_ & ImGuiChildFlags_Borders) ? style.ChildBorderSize : 0.0f;

        // Scroll range from Yoga: the children's far edges plus the trailing padding and border
        // line, which ImGui wouldn't count from item positions alone. Child positions are
        // already local to this window (the layout is an origin).
        ImVec2 content{0.0f, 0.0f};
        for (size_t i = 0; i < layout().child_count(); ++i) {
            auto child = layout().child(i);
            if (!child->visible()) {
                continue;
            }

            // Through const: on a mutable object, margin(YGEdge) is ambiguous with the setter.
            const Layout& box = *child;
            content.x = std::max(content.x, box.x() + box.width() + box.margin(YGEdgeRight));
            content.y = std::max(content.y, box.y() + box.height() + box.margin(YGEdgeBottom));
        }

        const Widget& self = *this;
        content.x += self.padding(YGEdgeRight) + borderSize;
        content.y += self.padding(YGEdgeBottom) + borderSize;
        ImGui::SetNextWindowContentSize(content);

        // Yoga padding is the only inset: SetCursorPos ignores WindowPadding, but ImGui would
        // still shrink the clip rect by it.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
        // id(), not the title: renaming must not reset the scroll position.
        const bool visible = ImGui::BeginChild(static_cast<ImGuiID>(id()), size(), childFlags_, windowFlags_);
        ImGui::PopStyleVar();

        const ImGuiWindow* window = ImGui::GetCurrentWindow();
        border(borderSize, YGEdgeAll);
        border(borderSize + (window->ScrollbarY ? style.ScrollbarSize : 0.0f), YGEdgeRight);
        border(borderSize + (window->ScrollbarX ? style.ScrollbarSize : 0.0f), YGEdgeBottom);

        bool changed = false;
        if (visible) {
            changed = Widget::show();
        }

        // Unlike most Begin/End pairs, EndChild is required even when BeginChild returns false.
        ImGui::EndChild();
        return changed;
    }
}
