#include <algorithm>

#include "thorin/chartwidget.hpp"

namespace thorin {
    namespace {
        void setup_axis(ImAxis id, const Axis& axis) {
            ImPlot::SetupAxis(id, axis.label.empty() ? nullptr : axis.label.c_str(), axis.flags);
            ImPlot::SetupAxisScale(id, axis.scale);
            if (axis.limits) {
                ImPlot::SetupAxisLimits(id, axis.limits->Min, axis.limits->Max, ImPlotCond_Always);
            }
            if (!axis.format.empty()) {
                ImPlot::SetupAxisFormat(id, axis.format.c_str());
            }
        }
    }

    ChartWidget::ChartWidget(const std::string& title, Widget* parent): Widget(title, parent) {
        layout().enable_measure();
    }

    Series& ChartWidget::add_series(std::unique_ptr<Series> series) {
        series_.push_back(std::move(series));
        return *series_.back();
    }

    void ChartWidget::remove_series(const Series& series) {
        std::erase_if(series_, [&](const auto& s) { return s.get() == &series; });
    }

    void ChartWidget::remove_series(const std::string& label) {
        std::erase_if(series_, [&](const auto& s) { return s->label() == label; });
    }

    Series* ChartWidget::get_series(const std::string& label) {
        const auto it = std::ranges::find_if(series_, [&](const auto& s) { return s->label() == label; });
        return it == series_.end() ? nullptr : it->get();
    }

    void ChartWidget::remove_series(uint64_t id) {
        std::erase_if(series_, [&](const auto& s) { return s->id() == id; });
    }

    Series* ChartWidget::get_series(uint64_t id) {
        const auto it = std::ranges::find_if(series_, [&](const auto& s) { return s->id() == id; });
        return it == series_.end() ? nullptr : it->get();
    }

    void ChartWidget::clear_series() {
        series_.clear();
    }

    const std::vector<std::unique_ptr<Series>>& ChartWidget::series() const {
        return series_;
    }

    Axis& ChartWidget::x_axis() {
        return xAxis_;
    }

    const Axis& ChartWidget::x_axis() const {
        return xAxis_;
    }

    ChartWidget& ChartWidget::set_x_axis(Axis axis) {
        xAxis_ = std::move(axis);
        return *this;
    }

    Axis& ChartWidget::y_axis() {
        return yAxis_;
    }

    const Axis& ChartWidget::y_axis() const {
        return yAxis_;
    }

    ChartWidget& ChartWidget::set_y_axis(Axis axis) {
        yAxis_ = std::move(axis);
        return *this;
    }

    ImPlotFlags ChartWidget::flags() const {
        return flags_;
    }

    ChartWidget& ChartWidget::set_flags(ImPlotFlags flags) {
        flags_ = flags;
        return *this;
    }

    bool ChartWidget::show() {
        // title_id() carries the "##id", so charts with the same title don't share state.
        if (!ImPlot::BeginPlot(title_id().c_str(), size(), flags_)) {
            return false;
        }
        setup_axis(ImAxis_X1, xAxis_);
        setup_axis(ImAxis_Y1, yAxis_);
        for (const auto& series : series_) {
            series->plot();
        }
        ImPlot::EndPlot();
        return false;
    }

    ImVec2 ChartWidget::measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) {
        // Needs the window's ImPlot context, like ImGui-based measures need its ImGui context.
        const ImVec2 intrinsic = ImPlot::GetStyle().PlotDefaultSize;
        return ImVec2{
            fit_measure(intrinsic.x, width, widthMode),
            fit_measure(intrinsic.y, height, heightMode)
        };
    }
}
