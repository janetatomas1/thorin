#include <algorithm>

#include "thorin/chartwidget.hpp"

namespace thorin {
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

    void ChartWidget::clear_series() {
        series_.clear();
    }

    const std::vector<std::unique_ptr<Series>>& ChartWidget::series() const {
        return series_;
    }

    const std::string& ChartWidget::x_label() const {
        return xLabel_;
    }

    const std::string& ChartWidget::y_label() const {
        return yLabel_;
    }

    ChartWidget& ChartWidget::set_axis_labels(const std::string& x, const std::string& y) {
        xLabel_ = x;
        yLabel_ = y;
        return *this;
    }

    std::optional<ImPlotRange> ChartWidget::x_limits() const {
        return xLimits_;
    }

    ChartWidget& ChartWidget::set_x_limits(std::optional<ImPlotRange> limits) {
        xLimits_ = limits;
        return *this;
    }

    std::optional<ImPlotRange> ChartWidget::y_limits() const {
        return yLimits_;
    }

    ChartWidget& ChartWidget::set_y_limits(std::optional<ImPlotRange> limits) {
        yLimits_ = limits;
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
        ImPlot::SetupAxes(
            xLabel_.empty() ? nullptr : xLabel_.c_str(),
            yLabel_.empty() ? nullptr : yLabel_.c_str()
        );
        if (xLimits_) {
            ImPlot::SetupAxisLimits(ImAxis_X1, xLimits_->Min, xLimits_->Max, ImPlotCond_Always);
        }
        if (yLimits_) {
            ImPlot::SetupAxisLimits(ImAxis_Y1, yLimits_->Min, yLimits_->Max, ImPlotCond_Always);
        }
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
