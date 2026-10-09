#include <algorithm>
#include <format>

#include <implot.h>
#include <libassert/assert.hpp>

#include "thorin/series.hpp"
#include "thorin/thorin.hpp"

namespace thorin {
    SeriesBuffer::SeriesBuffer(std::vector<double> values): values_(std::move(values)) {}

    std::span<const double> SeriesBuffer::values() const {
        return values_;
    }

    std::size_t SeriesBuffer::size() const {
        return values_.size();
    }

    bool SeriesBuffer::empty() const {
        return values_.empty();
    }

    SeriesBuffer& SeriesBuffer::add(double value) {
        values_.push_back(value);
        return *this;
    }

    SeriesBuffer& SeriesBuffer::add(std::span<const double> values) {
        values_.insert(values_.end(), values.begin(), values.end());
        return *this;
    }

    SeriesBuffer& SeriesBuffer::remove(std::size_t index) {
        DEBUG_ASSERT(index < values_.size(), "SeriesBuffer::remove index out of range", index, values_.size());
        values_.erase(values_.begin() + static_cast<std::ptrdiff_t>(index));
        return *this;
    }

    SeriesBuffer& SeriesBuffer::clear() {
        values_.clear();
        return *this;
    }

    SeriesView::SeriesView(std::span<const double> values): values_(values) {}

    std::span<const double> SeriesView::values() const {
        return values_;
    }

    std::size_t SeriesView::size() const {
        return values_.size();
    }

    bool SeriesView::empty() const {
        return values_.empty();
    }

    std::span<const double> values(const SeriesData& data) {
        return std::visit([](const auto& d) { return d.values(); }, data);
    }

    Series::Series(const std::string& label)
    : id_(Thorin::random()), label_(label), labelID_(std::format("{}##{}", label, id_)) {}

    uint64_t Series::id() const {
        return id_;
    }

    const std::string& Series::label() const {
        return label_;
    }

    const std::string& Series::label_id() const {
        return labelID_;
    }

    Series& Series::set_label(const std::string& label) {
        label_ = label;
        labelID_ = std::format("{}##{}", label, id_);
        return *this;
    }

    std::optional<ImVec4> Series::color() const {
        return color_;
    }

    Series& Series::set_color(std::optional<ImVec4> color) {
        color_ = color;
        return *this;
    }

    XYSeries::XYSeries(const std::string& label, SeriesData xs, SeriesData ys)
    : Series(label), xs_(std::move(xs)), ys_(std::move(ys)) {}

    XYSeries& XYSeries::set_data(SeriesData xs, SeriesData ys) {
        xs_ = std::move(xs);
        ys_ = std::move(ys);
        return *this;
    }

    SeriesData& XYSeries::xs_data() {
        return xs_;
    }

    SeriesData& XYSeries::ys_data() {
        return ys_;
    }

    std::span<const double> XYSeries::xs() const {
        return values(xs_);
    }

    std::span<const double> XYSeries::ys() const {
        return values(ys_);
    }

    int XYSeries::count() const {
        return static_cast<int>(std::min(values(xs_).size(), values(ys_).size()));
    }

    float LineSeries::line_weight() const {
        return lineWeight_;
    }

    LineSeries& LineSeries::set_line_weight(float weight) {
        lineWeight_ = weight;
        return *this;
    }

    void LineSeries::plot() const {
        ImPlotSpec spec;
        spec.LineColor = color().value_or(IMPLOT_AUTO_COL);
        spec.LineWeight = lineWeight_;
        ImPlot::PlotLine(label_id().c_str(), xs().data(), ys().data(), count(), spec);
    }

    float ScatterSeries::line_weight() const {
        return lineWeight_;
    }

    ScatterSeries& ScatterSeries::set_line_weight(float weight) {
        lineWeight_ = weight;
        return *this;
    }

    void ScatterSeries::plot() const {
        // Marker fill and edge default to LineColor.
        ImPlotSpec spec;
        spec.LineColor = color().value_or(IMPLOT_AUTO_COL);
        spec.LineWeight = lineWeight_;
        ImPlot::PlotScatter(label_id().c_str(), xs().data(), ys().data(), count(), spec);
    }

    double BarSeries::bar_width() const {
        return barWidth_;
    }

    BarSeries& BarSeries::set_bar_width(double width) {
        barWidth_ = width;
        return *this;
    }

    void BarSeries::plot() const {
        ImPlotSpec spec;
        spec.FillColor = color().value_or(IMPLOT_AUTO_COL);
        ImPlot::PlotBars(label_id().c_str(), xs().data(), ys().data(), count(), barWidth_, spec);
    }
}
