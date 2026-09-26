#include <algorithm>

#include "thorin/table.hpp"

using namespace thorin::literals;

namespace thorin {
    Table::Table(std::vector<std::string> headers, Widget* parent)
    : Widget("", parent), headers_(std::move(headers)) {
        DEBUG_ASSERT(!headers_.empty(), "Table needs at least one column");
        layout().enable_measure();
        width(100_pcts);
    }

    Table& Table::add_cell(int value) {
        cells_.emplace_back(value);
        layout().mark_dirty();
        return *this;
    }

    Table& Table::add_cell(float value) {
        cells_.emplace_back(value);
        layout().mark_dirty();
        return *this;
    }

    size_t Table::count() const {
        return cells_.size();
    }

    size_t Table::columns() const {
        return headers_.size();
    }

    size_t Table::rows() const {
        return (count() + columns() - 1) / columns();
    }

    Table& Table::set_flags(ImGuiTableFlags flags) {
        flags_ = flags;
        layout().mark_dirty();
        return *this;
    }

    ImGuiTableFlags Table::flags() const {
        return flags_;
    }

    bool Table::show() {
        bool changed = false;

        if (ImGui::BeginTable(title_id().c_str(), static_cast<int>(columns()), flags_, size())) {
            for (auto& header : headers_) {
                ImGui::TableSetupColumn(header.c_str());
            }
            ImGui::TableHeadersRow();

            size_t index = 0;
            for (auto& cell : cells_) {
                if (index++ % columns() == 0) {
                    ImGui::TableNextRow();
                }
                ImGui::TableNextColumn();

                if (auto value = std::get_if<int>(&cell)) {
                    ImGui::Text("%d", *value);
                } else if (auto value = std::get_if<float>(&cell)) {
                    ImGui::Text("%.3f", *value);
                } else {
                    // Widgets are outside the Yoga tree, so they reach the app through the table's window.
                    auto widget = std::get<Widget*>(cell);
                    widget->set_window(window());
                    changed |= widget->draw();
                }
            }

            ImGui::EndTable();
        }

        return changed;
    }

    ImVec2 Table::measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
        // Mirrors ImGui: each row is its tallest cell plus CellPadding.y above and below.
        // Borders are drawn over the row edges and take no space.
        const float padding = ImGui::GetStyle().CellPadding.y * 2.0f;
        float intrinsic = ImGui::GetTextLineHeight() + padding;
        float rowHeight = 0.0f;

        size_t index = 0;
        for (auto& cell : cells_) {
            // Numbers are one text line; widgets report their own height.
            const float cellHeight = std::holds_alternative<Widget*>(cell)
                ? std::get<Widget*>(cell)->measure(0.0f, YGMeasureModeUndefined, 0.0f, YGMeasureModeUndefined).y
                : ImGui::GetTextLineHeight();
            rowHeight = std::max(rowHeight, cellHeight);

            if (++index % columns() == 0 || index == count()) {
                intrinsic += rowHeight + padding;
                rowHeight = 0.0f;
            }
        }

        return ImVec2{
            fit_measure(0.0f, width, widthMode),
            fit_measure(intrinsic, height, heightMode)
        };
    }
}
