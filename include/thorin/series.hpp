#pragma once

#include <optional>
#include <span>
#include <string>
#include <variant>
#include <vector>

#include <imgui.h>

namespace thorin {
    // Values a series owns.
    class SeriesBuffer {
        std::vector<double> values_;

    public:
        SeriesBuffer() = default;
        SeriesBuffer(std::vector<double> values);

        [[nodiscard]] std::span<const double> values() const;
        [[nodiscard]] std::size_t size() const;
        [[nodiscard]] bool empty() const;

        SeriesBuffer& add(double value);
        SeriesBuffer& add(std::span<const double> values);
        // Index must be < size().
        SeriesBuffer& remove(std::size_t index);
        SeriesBuffer& clear();
    };

    // Values a series borrows: the caller keeps the buffer alive while the series uses it,
    // and changes it only on the UI thread (post()), never during a frame.
    class SeriesView {
        std::span<const double> values_;

    public:
        SeriesView() = default;
        SeriesView(std::span<const double> values);

        [[nodiscard]] std::span<const double> values() const;
        [[nodiscard]] std::size_t size() const;
        [[nodiscard]] bool empty() const;
    };

    // Values a series plots: owned or borrowed.
    using SeriesData = std::variant<SeriesBuffer, SeriesView>;
    [[nodiscard]] std::span<const double> values(const SeriesData& data);

    // One plot item of a ChartWidget. Subclasses draw themselves in plot(),
    // which runs between ImPlot::BeginPlot and EndPlot.
    class Series {
        std::string label_;
        std::optional<ImVec4> color_;

    public:
        explicit Series(const std::string& label);
        virtual ~Series() = default;

        virtual void plot() const = 0;

        // Legend entry; "##" hides it, as in ImGui.
        [[nodiscard]] const std::string& label() const;
        Series& set_label(const std::string& label);
        // Empty = ImPlot picks the next colormap color.
        [[nodiscard]] std::optional<ImVec4> color() const;
        Series& set_color(std::optional<ImVec4> color);
    };

    // A series of (x, y) points; xs and ys are plotted up to the shorter of the two.
    class XYSeries : public Series {
        SeriesData xs_;
        SeriesData ys_;

    protected:
        [[nodiscard]] std::span<const double> xs() const;
        [[nodiscard]] std::span<const double> ys() const;
        [[nodiscard]] int count() const;

    public:
        XYSeries(const std::string& label, SeriesData xs, SeriesData ys);
        XYSeries& set_data(SeriesData xs, SeriesData ys);
        // E.g. std::get_if<SeriesBuffer>(&series.xs_data()) to add or remove owned values.
        [[nodiscard]] SeriesData& xs_data();
        [[nodiscard]] SeriesData& ys_data();
    };

    class LineSeries : public XYSeries {
        float lineWeight_ = 1.0f;

    public:
        using XYSeries::XYSeries;
        // Line thickness in pixels.
        [[nodiscard]] float line_weight() const;
        LineSeries& set_line_weight(float weight);
        void plot() const override;
    };

    class ScatterSeries : public XYSeries {
        float lineWeight_ = 1.0f;

    public:
        using XYSeries::XYSeries;
        // Marker edge thickness in pixels.
        [[nodiscard]] float line_weight() const;
        ScatterSeries& set_line_weight(float weight);
        void plot() const override;
    };

    // Bars centered on xs, barWidth in plot units.
    class BarSeries : public XYSeries {
        double barWidth_ = 0.67;

    public:
        using XYSeries::XYSeries;
        [[nodiscard]] double bar_width() const;
        BarSeries& set_bar_width(double width);
        void plot() const override;
    };
}
