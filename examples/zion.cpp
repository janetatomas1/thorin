
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <random>
#include <vector>

#include <imgui.h>
#include <imgui_internal.h>

#include <thorin/thorin.hpp>
#include <thorin/tabwidget.hpp>
#include <thorin/pane.hpp>
#include <thorin/theme.hpp>
#include <thorin/window.hpp>
#include <thorin/font.hpp>
#include <thorin/dockarea.hpp>
#include <thorin/logwidget.hpp>

using namespace thorin;

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
    // Animated: an OnDemand or Adaptive loop must keep drawing.
    if (Thorin::has_current()) {
        Thorin::current().request_redraw();
    }
}

struct CPUTab : public Widget {
    CPUTab(): Widget("CPU") {}
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
        // No dock tab above the pane: the CPU/GPU/Disk bar is its top edge, spanning the full width.
        set_dock_flags(ImGuiDockNodeFlags_NoTabBar)
        .set_window_flags(ImGuiWindowFlags_NoBackground);
        tabs.flex_grow(1);
    }
};

// Everything logged since the app started: thorin's own messages and the app's.
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
        add_pane<LogPane>().set_dock(ImGuiDir_Down).set_ratio(0.3f);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);

    Theme theme = Theme::matrix();
    theme.font = &Font::jetbrains_mono();
    theme.style.FontSizeBase = 15.0f;

    app.set_theme(theme);
    app.set_background_painter([] { return BackgroundPainter(MatrixRain{}); });
    app.add_window<Root>();
    spdlog::info("zion started");

    return app.exec();
}
