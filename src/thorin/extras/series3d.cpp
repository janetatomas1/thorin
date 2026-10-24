#include <algorithm>
#include <format>

#include <implot3d.h>
#include <libassert/assert.hpp>

#include "thorin/extras/series3d.hpp"
#include "thorin/thorin.hpp"

namespace thorin {
    Series3D::Series3D(const std::string& label)
    : id_(Thorin::random()), label_(label), labelID_(std::format("{}##{}", label, id_)) {}

    uint64_t Series3D::id() const {
        return id_;
    }

    const std::string& Series3D::label() const {
        return label_;
    }

    const std::string& Series3D::label_id() const {
        return labelID_;
    }

    Series3D& Series3D::set_label(const std::string& label) {
        label_ = label;
        labelID_ = std::format("{}##{}", label, id_);
        return *this;
    }

    std::optional<ImVec4> Series3D::color() const {
        return color_;
    }

    Series3D& Series3D::set_color(std::optional<ImVec4> color) {
        color_ = color;
        return *this;
    }

    XYZSeries::XYZSeries(const std::string& label, SeriesData xs, SeriesData ys, SeriesData zs)
    : Series3D(label), xs_(std::move(xs)), ys_(std::move(ys)), zs_(std::move(zs)) {}

    XYZSeries& XYZSeries::set_data(SeriesData xs, SeriesData ys, SeriesData zs) {
        xs_ = std::move(xs);
        ys_ = std::move(ys);
        zs_ = std::move(zs);
        return *this;
    }

    SeriesData& XYZSeries::xs_data() {
        return xs_;
    }

    SeriesData& XYZSeries::ys_data() {
        return ys_;
    }

    SeriesData& XYZSeries::zs_data() {
        return zs_;
    }

    std::span<const double> XYZSeries::xs() const {
        return values(xs_);
    }

    std::span<const double> XYZSeries::ys() const {
        return values(ys_);
    }

    std::span<const double> XYZSeries::zs() const {
        return values(zs_);
    }

    int XYZSeries::count() const {
        return static_cast<int>(std::min({values(xs_).size(), values(ys_).size(), values(zs_).size()}));
    }

    int XYZSeries::offset() const {
        DEBUG_ASSERT(
            thorin::offset(xs_) == thorin::offset(ys_) && thorin::offset(xs_) == thorin::offset(zs_),
            "XYZSeries: xs, ys and zs rings out of step",
            thorin::offset(xs_), thorin::offset(ys_), thorin::offset(zs_)
        );
        return static_cast<int>(thorin::offset(xs_));
    }

    float Line3DSeries::line_weight() const {
        return lineWeight_;
    }

    Line3DSeries& Line3DSeries::set_line_weight(float weight) {
        lineWeight_ = weight;
        return *this;
    }

    void Line3DSeries::plot() const {
        // ImPlot3D defaults to auto markers; lines have none, as in ChartWidget.
        ImPlot3DSpec spec;
        spec.LineColor = color().value_or(IMPLOT3D_AUTO_COL);
        spec.LineWeight = lineWeight_;
        spec.Marker = ImPlot3DMarker_None;
        spec.Offset = offset();
        ImPlot3D::PlotLine(label_id().c_str(), xs().data(), ys().data(), zs().data(), count(), spec);
    }

    float Scatter3DSeries::line_weight() const {
        return lineWeight_;
    }

    Scatter3DSeries& Scatter3DSeries::set_line_weight(float weight) {
        lineWeight_ = weight;
        return *this;
    }

    void Scatter3DSeries::plot() const {
        // Marker fill and edge default to LineColor.
        ImPlot3DSpec spec;
        spec.LineColor = color().value_or(IMPLOT3D_AUTO_COL);
        spec.LineWeight = lineWeight_;
        spec.Offset = offset();
        ImPlot3D::PlotScatter(label_id().c_str(), xs().data(), ys().data(), zs().data(), count(), spec);
    }
}
