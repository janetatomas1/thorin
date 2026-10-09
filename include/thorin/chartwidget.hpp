#pragma once

#include <concepts>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <implot.h>

#include "thorin/series.hpp"
#include "thorin/widget.hpp"

namespace thorin {
    // An ImPlot plot drawn at this widget's Yoga box. It owns its Series; whether a series
    // owns its values is up to its SeriesData (SeriesBuffer or SeriesView). Intrinsic size is ImPlot's PlotDefaultSize.
    // The title is the plot title; "##" hides it.
    class ChartWidget : public Widget {
        std::vector<std::unique_ptr<Series>> series_;
        std::string xLabel_;
        std::string yLabel_;
        std::optional<ImPlotRange> xLimits_;
        std::optional<ImPlotRange> yLimits_;
        ImPlotFlags flags_ = ImPlotFlags_None;

    public:
        ChartWidget(const std::string& title = "", Widget* parent = nullptr);

        // The reference stays valid until the series is removed or the chart is destroyed.
        template <std::derived_from<Series> S, class... Args>
        S& add_series(Args&&... args) {
            series_.push_back(std::make_unique<S>(std::forward<Args>(args)...));
            return static_cast<S&>(*series_.back());
        }
        Series& add_series(std::unique_ptr<Series> series);
        // Destroys the series; does nothing when it isn't in this chart.
        void remove_series(const Series& series);
        void clear_series();
        [[nodiscard]] const std::vector<std::unique_ptr<Series>>& series() const;

        [[nodiscard]] const std::string& x_label() const;
        [[nodiscard]] const std::string& y_label() const;
        ChartWidget& set_axis_labels(const std::string& x, const std::string& y);

        // Fixed axis limits; empty = auto-fit to the data.
        [[nodiscard]] std::optional<ImPlotRange> x_limits() const;
        ChartWidget& set_x_limits(std::optional<ImPlotRange> limits);
        [[nodiscard]] std::optional<ImPlotRange> y_limits() const;
        ChartWidget& set_y_limits(std::optional<ImPlotRange> limits);

        [[nodiscard]] ImPlotFlags flags() const;
        ChartWidget& set_flags(ImPlotFlags flags);

        bool show() final;
        ImVec2 measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) final;
    };
}
