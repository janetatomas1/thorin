#include <algorithm>
#include <string>

#include "thorin/assert.hpp"
#include "thorin/table.hpp"

using namespace thorin::literals;

namespace thorin {
    Table::Table(std::vector<std::string> headers, Widget* parent)
    : Widget("", parent), headers_(std::move(headers)), sizing_(headers_.size()) {
        THORIN_DEBUG_ASSERT(!headers_.empty(), "Table needs at least one column");
        layout().enable_measure();
        width(100_pcts);
    }

    Table::Table(size_t columns, Widget* parent): Widget("", parent), sizing_(columns), showHeaders_(false) {
        THORIN_DEBUG_ASSERT(columns > 0, "Table needs at least one column");
        // ImGui still wants a name per column; "##" ones are ids only.
        for (size_t i = 0; i < columns; ++i) {
            headers_.push_back("##" + std::to_string(i));
        }
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

    Table& Table::set_column_width(size_t index, float width) {
        THORIN_DEBUG_ASSERT(index < columns(), "Table::set_column_width index out of range", index, columns());
        THORIN_DEBUG_ASSERT(width > 0.0f, "Table::set_column_width needs a positive width", width);
        sizing_[index] = {ImGuiTableColumnFlags_WidthFixed, width};
        layout().mark_dirty();
        return *this;
    }

    Table& Table::set_column_stretch(size_t index, float weight) {
        THORIN_DEBUG_ASSERT(index < columns(), "Table::set_column_stretch index out of range", index, columns());
        THORIN_DEBUG_ASSERT(weight > 0.0f, "Table::set_column_stretch needs a positive weight", weight);
        sizing_[index] = {ImGuiTableColumnFlags_WidthStretch, weight};
        layout().mark_dirty();
        return *this;
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

        // Nothing to draw: BeginTable would still draw the outer border as a line. An empty item stands in,
        // because render() moved the cursor here and ImGui wants an item after that.
        if (cells_.empty() && !showHeaders_) {
            ImGui::Dummy(size());
            return false;
        }

        if (ImGui::BeginTable(title_id().c_str(), static_cast<int>(columns()), flags_, size())) {
            for (size_t i = 0; i < columns(); ++i) {
                ImGui::TableSetupColumn(headers_[i].c_str(), sizing_[i].flags, sizing_[i].value);
            }
            if (showHeaders_) {
                ImGui::TableHeadersRow();
            }

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
        float intrinsic = showHeaders_ ? ImGui::GetTextLineHeight() + padding : 0.0f;
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

        // Fixed columns set the least width the table needs; the rest have none of their own.
        float intrinsicWidth = 0.0f;

        for (const auto& column : sizing_) {
            if (column.flags & ImGuiTableColumnFlags_WidthFixed) {
                intrinsicWidth += column.value + ImGui::GetStyle().CellPadding.x * 2.0f;
            }
        }

        return ImVec2{
            fit_measure(intrinsicWidth, width, widthMode),
            fit_measure(intrinsic, height, heightMode)
        };
    }
}
