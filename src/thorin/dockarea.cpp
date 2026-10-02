#include <algorithm>

#include <imgui_internal.h>
#include <libassert/assert.hpp>

#include "thorin/dockarea.hpp"
#include "thorin/thorin.hpp"

namespace thorin {
    DockArea::DockArea(Widget* parent): Widget("", parent) {}

    void DockArea::attach_pane(std::unique_ptr<Pane> pane) {
        app().add_action([this, pane = std::move(pane)]() mutable {
            adopt(std::move(pane)).set_parent(this);
        });
    }

    void DockArea::remove_pane(size_t index) {
        app().add_action([this, index] {
            DEBUG_ASSERT(index < count(), "DockArea::remove_pane index out of range", index, count());
            // The pane's window is no longer submitted, so ImGui drops it from its node.
            remove(index);
        });
    }

    DockArea& DockArea::set_flags(ImGuiDockNodeFlags flags) {
        flags_ = flags;
        return *this;
    }

    ImGuiDockNodeFlags DockArea::flags() const {
        return flags_;
    }

    void DockArea::build() {
        const ImGuiID dockId = static_cast<ImGuiID>(id());

        ImGui::DockBuilderRemoveNode(dockId);
        ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockId, size());

        // Each sided pane is split off what is left, in add order; the rest is the middle. The
        // pane's ratio is of the whole area, so it is converted to a share of what is left.
        ImGuiID middle = dockId;
        ImVec2 remaining = size();
        for (Pane& pane : items()) {
            if (pane.initialSide_ != ImGuiDir_None) {
                const bool horizontal = pane.initialSide_ == ImGuiDir_Left || pane.initialSide_ == ImGuiDir_Right;
                float& remainingSize = horizontal ? remaining.x : remaining.y;
                const float wanted = pane.initialRatio_ * (horizontal ? width() : height());
                const float ratio = remainingSize > 0.0f ? std::min(wanted / remainingSize, 1.0f) : 0.0f;
                remainingSize = std::max(remainingSize - wanted, 0.0f);

                ImGuiID side = 0;
                ImGui::DockBuilderSplitNode(middle, pane.initialSide_, ratio, &side, &middle);
                pane.dockNode_ = side;
                pane.placed_ = true;
            }
        }

        for (Pane& pane : items()) {
            if (!pane.placed_) {
                pane.dockNode_ = middle;
                pane.placed_ = true;
            }
        }

        ImGui::DockBuilderFinish(dockId);
        built_ = true;
    }

    bool DockArea::show() {
        // No box yet: nodes built now would split nothing, and DockSpace() reads a zero size as
        // "fill the window".
        if (width() <= 0.0f || height() <= 0.0f) {
            return false;
        }

        const ImGuiID dockId = static_cast<ImGuiID>(id());
        if (!built_ || ImGui::DockBuilderGetNode(dockId) == nullptr) {
            build();
        }

        // The dock space fills this widget's box; render() put the cursor at its corner.
        ImGui::DockSpace(dockId, size(), flags_);

        // Panes added after the first layout go to the middle as tabs.
        const ImGuiDockNode* central = ImGui::DockBuilderGetCentralNode(dockId);
        const ImGuiID middle = central != nullptr ? central->ID : dockId;

        bool changed = false;
        for (Pane& pane : items()) {
            if (!pane.placed_) {
                pane.dockNode_ = middle;
                pane.placed_ = true;
            }

            if (pane.dockNode_ != 0) {
                ImGui::SetNextWindowDockID(pane.dockNode_);
                pane.dockNode_ = 0;
            }

            changed |= pane.draw();
        }

        return changed;
    }
}
