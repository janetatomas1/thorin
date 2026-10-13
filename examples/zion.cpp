
#include <cfloat>
#include <cmath>
#include <format>
#include <fstream>
#include <limits>
#include <memory>
#include <random>
#include <vector>

#include <sys/utsname.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <thorin/thorin.hpp>
#include <thorin/tabwidget.hpp>
#include <thorin/pane.hpp>
#include <thorin/theme.hpp>
#include <thorin/window.hpp>
#include <thorin/font.hpp>
#include <thorin/image.hpp>
#include <thorin/dockarea.hpp>
#include <thorin/table.hpp>
#include <thorin/chartwidget.hpp>
#include <thorin/text.hpp>
#include <thorin/logwidget.hpp>

using namespace thorin;
using namespace literals;

constexpr char glyphs[] = "0123456789ABCDEFXYZ:.=*+-<>|$#@";

// Animated "digital rain" background painter for Window::set_background_painter(). Falling columns of
// glyphs in the current font, coloured with the theme's text colour. Keeps the loop drawing while set.
class MatrixRain {
    struct Column {
        float head;   // row of the leading glyph, may be negative while above the window
        float speed;  // rows per second
    };

    std::vector<Column> columns_;
    std::minstd_rand rng_{std::random_device{}()};
    int rows_ = 0;

    Column spawn(bool initial);
    [[nodiscard]] char glyph_at(int column, int row, double time) const;

public:
    float opacity = 0.3f;
    // Average fall speed in rows per second; each column varies around it.
    float speed = 10.0f;
    // Glyphs per column, fading from the head upwards.
    int trail = 22;
    // How often a glyph changes, per second.
    float glyphChangeRate = 6.0f;

    void operator()(ImDrawList& list, ImVec2 min, ImVec2 max);
};

MatrixRain::Column MatrixRain::spawn(bool initial) {
    std::uniform_real_distribution<float> start(initial ? -static_cast<float>(rows_) : -0.5f * rows_,
                                                initial ? static_cast<float>(rows_) : 0.0f);
    std::uniform_real_distribution<float> variation(0.5f, 1.5f);
    return {start(rng_), speed * variation(rng_)};
}

char MatrixRain::glyph_at(int column, int row, double time) const {
    // Stateless flicker: each cell changes at its own pace, so nothing per cell has to be stored.
    const auto tick = static_cast<std::uint32_t>(time * glyphChangeRate * (0.5 + (column % 7) / 7.0));
    const std::uint32_t hash = static_cast<std::uint32_t>(column) * 73856093u
        ^ static_cast<std::uint32_t>(row) * 19349663u
        ^ tick * 83492791u;
    return glyphs[hash % (sizeof(glyphs) - 1)];
}

void MatrixRain::operator()(ImDrawList& list, ImVec2 min, ImVec2 max) {
    ImFont* font = ImGui::GetFont();
    const float size = ImGui::GetFontSize();
    const float cellWidth = font->CalcTextSizeA(size, FLT_MAX, 0.0f, "0").x;

    if (cellWidth <= 0.0f || size <= 0.0f || trail <= 0) {
        return;
    }
    const int columns = static_cast<int>(std::ceil((max.x - min.x) / cellWidth));
    rows_ = static_cast<int>(std::ceil((max.y - min.y) / size));

    // New window width: start over, scattered so the screen isn't empty on the first frames.
    if (static_cast<int>(columns_.size()) != columns) {
        columns_.clear();

        for (int i = 0; i < columns; i++) {
            columns_.push_back(spawn(true));
        }
    }
    const float dt = ImGui::GetIO().DeltaTime;
    const double time = ImGui::GetTime();
    const ImVec4 text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
    // The leading glyph is the text colour pushed most of the way to white.
    const ImVec4 head{
        text.x + (1.0f - text.x) * 0.7f,
        text.y + (1.0f - text.y) * 0.7f,
        text.z + (1.0f - text.z) * 0.7f,
        text.w
    };

    for (int c = 0; c < columns; c++) {
        Column& column = columns_[c];
        column.head += column.speed * dt;

        if (column.head - static_cast<float>(trail) > static_cast<float>(rows_)) {
            column = spawn(false);
        }
        const int headRow = static_cast<int>(std::floor(column.head));

        for (int k = 0; k < trail; k++) {
            const int row = headRow - k;

            if (row < 0) {
                break;
            }
            if (row >= rows_) {
                continue;
            }
            const float fade = 1.0f - static_cast<float>(k) / static_cast<float>(trail);
            ImVec4 color = k == 0 ? head : text;
            color.w *= opacity * fade * fade;
            const char glyph[2] = {glyph_at(c, row, time), '\0'};
            list.AddText(
                font,
                size,
                ImVec2(min.x + static_cast<float>(c) * cellWidth, min.y + static_cast<float>(row) * size),
                ImGui::ColorConvertFloat4ToU32(color),
                glyph,
                glyph + 1
            );
        }
    }

    if (Thorin::has_current()) {
        Thorin::current().request_redraw();
    }
}

// One logical CPU: name, a usage sparkline over the last minute, current usage and frequency.
struct CoreRow : public Widget {
    static constexpr std::size_t capacity = 60;

    Text name;
    ChartWidget chart;
    LineSeries& usage = chart.add_series<LineSeries>("##usage", SeriesRing(capacity), SeriesRing(capacity));
    Text value = Text("", this);

    CoreRow(int index, Widget* parent): Widget("", parent),
        name(std::format("CPU {:<2}", index), this),
        chart(std::format("##core{}", index), this) {
        flex_direction(YGFlexDirectionRow);
        align_items(YGAlignCenter);
        // gap(2);
        chart.height(55).max_width(40_pcts);
        chart.set_flags(ImPlotFlags_CanvasOnly).set_legend({});
        chart.set_x_axis({.flags = ImPlotAxisFlags_NoDecorations});
        chart.set_y_axis({.flags = ImPlotAxisFlags_NoDecorations, .limits = ImPlotRange(0, 100)});
    }

    void sample(double time, float percent, float frequencyMHz) {
        usage.xs<SeriesRing>().add(time);
        usage.ys<SeriesRing>().add(percent);
        chart.x_axis().limits = ImPlotRange(time - static_cast<double>(capacity), time);
        value.set_title(std::format("{:3.0f}%  {:4.0f} MHz", percent, frequencyMHz));
    }
};

struct CPUTab : public Widget {
    struct CPUInfo {
        std::string model;
        std::string vendor;
        std::string architecture;
        int cores = 0;
        int threads = 0;
        float minFrequencyMHz = 0;
        float maxFrequencyMHz = 0;
        std::size_t cacheL1d = 0;
        std::size_t cacheL2 = 0;
        std::size_t cacheL3 = 0;
    };

    struct CPUSample {
        float usage = 0;                  // %, total, from /proc/stat deltas
        std::vector<float> coreUsage;     // %, per logical CPU
        std::vector<float> frequencyMHz;  // per CPU, cpufreq/scaling_cur_freq
        float temperatureC = NAN;         // hwmon "k10temp" Tctl; NAN if not found
        float load1 = 0, load5 = 0, load15 = 0;  // /proc/loadavg
        double uptimeSeconds = 0;         // /proc/uptime
    };

    // First line of a sysfs file; empty if it can't be read.
    static std::string read_line(const std::string& path) {
        std::ifstream stream(path);
        std::string line;
        std::getline(stream, line);
        return line;
    }

    static CPUInfo read_cpu_info() {
        CPUInfo info;

        // Only the first processor block: the others repeat the same model fields.
        std::ifstream stream("/proc/cpuinfo");
        std::string line;
        while (std::getline(stream, line) && !line.empty()) {
            const auto colon = line.find(':');
            if (colon == std::string::npos) {
                continue;
            }
            const auto key = line.substr(0, line.find_last_not_of(" \t", colon - 1) + 1);
            const auto value = colon + 2 <= line.size() ? line.substr(colon + 2) : std::string{};

            if (key == "model name") {
                info.model = value;
            } else if (key == "vendor_id") {
                info.vendor = value;
            } else if (key == "cpu cores") {
                info.cores = std::stoi(value);
            } else if (key == "siblings") {
                info.threads = std::stoi(value);
            }
        }

        utsname name{};
        if (uname(&name) == 0) {
            info.architecture = name.machine;
        }

        const std::string cpu0 = "/sys/devices/system/cpu/cpu0/";

        // cpufreq reports kHz.
        if (auto value = read_line(cpu0 + "cpufreq/cpuinfo_min_freq"); !value.empty()) {
            info.minFrequencyMHz = std::stof(value) / 1000.0f;
        }
        if (auto value = read_line(cpu0 + "cpufreq/cpuinfo_max_freq"); !value.empty()) {
            info.maxFrequencyMHz = std::stof(value) / 1000.0f;
        }

        // Sizes as "32K"; Instruction caches are skipped.
        for (int index = 0;; ++index) {
            const auto dir = cpu0 + "cache/index" + std::to_string(index) + "/";
            const auto level = read_line(dir + "level");
            if (level.empty()) {
                break;
            }
            const auto type = read_line(dir + "type");
            const std::size_t size = std::stoul(read_line(dir + "size")) * 1024;

            if (level == "1" && type == "Data") {
                info.cacheL1d = size;
            } else if (level == "2") {
                info.cacheL2 = size;
            } else if (level == "3") {
                info.cacheL3 = size;
            }
        }

        spdlog::info("cpu info fetched");
        return info;
    }

    // Jiffies from one "cpuN" line of /proc/stat.
    struct CoreTimes {
        unsigned long long idle = 0;
        unsigned long long total = 0;
    };

    // Per logical CPU, in /proc/stat order; the aggregate "cpu" line is skipped.
    static std::vector<CoreTimes> read_core_times() {
        std::vector<CoreTimes> times;
        std::ifstream stream("/proc/stat");
        std::string label;

        while (stream >> label && label.starts_with("cpu")) {
            // user nice system idle iowait irq softirq steal
            unsigned long long fields[8] = {};
            for (auto& field : fields) {
                stream >> field;
            }
            stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (label == "cpu") {
                continue;
            }
            CoreTimes core{.idle = fields[3] + fields[4]};
            for (auto field : fields) {
                core.total += field;
            }
            times.push_back(core);
        }
        return times;
    }

    Table cpu {2, this};
    Widget coreList {"", this};
    std::vector<std::unique_ptr<CoreRow>> cores;
    std::vector<CoreTimes> lastTimes = read_core_times();
    double lastSample = 0;

    // Once a second: usage from the /proc/stat delta since the last sample.
    bool show() override {
        const double time = ImGui::GetTime();
        if (time - lastSample >= 1.0) {
            lastSample = time;
            auto times = read_core_times();

            for (std::size_t i = 0; i < cores.size() && i < times.size(); ++i) {
                const auto total = times[i].total - lastTimes[i].total;
                const auto idle = times[i].idle - lastTimes[i].idle;
                const float percent = total > 0 ? 100.0f * static_cast<float>(total - idle) / static_cast<float>(total) : 0.0f;

                const auto freq = read_line(std::format("/sys/devices/system/cpu/cpu{}/cpufreq/scaling_cur_freq", i));
                cores[i]->sample(time, percent, freq.empty() ? 0.0f : std::stof(freq) / 1000.0f);
            }
            lastTimes = std::move(times);
        }
        return Widget::show();
    }

    CPUTab(): Widget("CPU") {
        flex_direction(YGFlexDirectionRow);
        cpu.margin(2_pcts).max_width(20_pcts);
        // Two columns of cores: rows wrap at half the width.
        coreList.margin(1_pcts).flex_grow(1).flex_direction(YGFlexDirectionRow);
        coreList.layout().flex_wrap(YGWrapWrap);
        coreList.layout().align_content(YGAlignFlexStart);

        for (std::size_t i = 0; i < lastTimes.size(); ++i) {
            auto& core = *cores.emplace_back(std::make_unique<CoreRow>(static_cast<int>(i), &coreList));
            core.width(47_pcts).margin(1_pcts);
        }

        const auto info = read_cpu_info();

        auto row = [this](const std::string& name, const std::string& value) {
            cpu.add_cell<Text>(name);
            cpu.add_cell<Text>(value);
        };

        row("Model", info.model);
        row("Vendor", info.vendor);
        row("Architecture", info.architecture);
        row("Cores", std::to_string(info.cores));
        row("Threads", std::to_string(info.threads));
        row("Min frequency", std::format("{:.0f} MHz", info.minFrequencyMHz));
        row("Max frequency", std::format("{:.0f} MHz", info.maxFrequencyMHz));
        row("L1d cache", std::format("{} KB", info.cacheL1d / 1024));
        row("L2 cache", std::format("{} KB", info.cacheL2 / 1024));
        row("L3 cache", std::format("{} KB", info.cacheL3 / 1024));
    }
};

struct Zion: TabWidget {
    Zion(Widget* parent = nullptr): TabWidget(parent) {
        auto & cpu = add_tab<CPUTab>();
        add_tab("GPU");
        add_tab("Disk");
    }
};

struct TabsPane : public Pane {
    Zion tabs = Zion(this);

    TabsPane(): Pane("Monitor") {
        set_dock_flags(ImGuiDockNodeFlags_NoTabBar)
        .set_window_flags(ImGuiWindowFlags_NoBackground);
        tabs.flex_grow(1);
    }
};

struct LogPane : public Pane {
    LogWidget log = LogWidget(this);

    LogPane(): Pane("Log") {
        log.flex_grow(1);
        set_closable(false);
        set_dock_flags(ImGuiDockNodeFlags_NoTabBar)
        .set_window_flags(ImGuiWindowFlags_NoBackground);
    }
};

struct Root : public DockArea {
    Root() {
        add_pane<TabsPane>();
        add_pane<LogPane>().set_dock(ImGuiDir_Down).set_ratio(0.2f);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);

    Theme theme = Theme::matrix();
    theme.font = &Font::jetbrains_mono();

    app.set_theme(theme);
    app.set_background_painter([] { return BackgroundPainter(MatrixRain{}); });

    if (auto icon = Image::from_file(THORIN_EXAMPLES_ASSETS "/zion-icon.png")) {
        app.set_icon(&app.add_image(std::move(*icon)));
    }

    app.add_window<Root>();
    spdlog::info("zion started");

    return app.exec();
}
