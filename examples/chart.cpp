#include <cmath>
#include <string>
#include <variant>

#include <thorin/thorin.hpp>
#include <thorin/chartwidget.hpp>

using namespace thorin;

// Chart: sine and cosine sampled every frame, scrolling through the last 10 seconds
// in a ChartWidget that fills the window.

struct Chart: Widget {
    static constexpr double window = 10.0;

    ChartWidget chart = ChartWidget("Sine and cosine", this);
    LineSeries& sine = chart.add_series<LineSeries>("sin(t)", SeriesBuffer(), SeriesBuffer());
    LineSeries& cosine = chart.add_series<LineSeries>("cos(t)", SeriesBuffer(), SeriesBuffer());

    Chart(const std::string& title): Widget(title) {
        column();
        padding(10);
        chart.flex_grow(1);
        chart.set_axis_labels("t [s]", "y");
        chart.set_y_limits(ImPlotRange(-1.2, 1.2));
    }

    // Appends (t, y) and drops the points that scrolled out of the window.
    static void sample(LineSeries& series, double t, double y) {
        auto& xs = std::get<SeriesBuffer>(series.xs_data());
        auto& ys = std::get<SeriesBuffer>(series.ys_data());
        xs.add(t);
        ys.add(y);
        while (xs.values().front() < t - window) {
            xs.remove(0);
            ys.remove(0);
        }
    }

    bool show() override {
        const double t = ImGui::GetTime();
        sample(sine, t, std::sin(t));
        sample(cosine, t, std::cos(t));
        chart.set_x_limits(ImPlotRange(t - window, t));
        return Widget::show();
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<Chart>(std::string("Chart"));
    return app.exec();
}
