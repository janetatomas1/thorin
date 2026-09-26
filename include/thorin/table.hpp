#pragma once

#include <string>
#include <variant>
#include <vector>

#include "thorin/container.hpp"

namespace thorin {
    // A grid of cells with a header row. Yoga places and sizes the table (full parent width
    // by default, height from the rows); the cells are drawn in ImGui's table flow and are
    // never added to the Yoga tree.
    // A cell is a plain number or a widget (use Text for strings). Widget cells are owned by
    // the protected Container; cells_ points into it.
    class Table : public Widget, protected Container<Widget> {
        using Cell = std::variant<int, float, Widget*>;

        std::vector<std::string> headers_;
        std::vector<Cell> cells_;
        ImGuiTableFlags flags_ = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

    public:
        // One column per header.
        Table(std::vector<std::string> headers, Widget* parent = nullptr);

        // Cells fill the table row by row.
        Table& add_cell(int value);
        Table& add_cell(float value);

        // Returned reference stays valid for the Table's lifetime.
        template <WidgetConcept W, class... Args>
        W& add_cell(Args&&... args) {
            auto& cell = add<W>(std::forward<Args>(args)...);
            cells_.emplace_back(&cell);
            layout().mark_dirty();
            return cell;
        }

        [[nodiscard]] size_t count() const;
        [[nodiscard]] size_t columns() const;
        [[nodiscard]] size_t rows() const;

        Table& set_flags(ImGuiTableFlags flags);
        [[nodiscard]] ImGuiTableFlags flags() const;

        bool show() override;
        ImVec2 measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) override;
    };
}
