#include <cstdint>
#include <deque>
#include <format>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <sys/utsname.h>

#include <glbinding/gl/gl.h>

#include <thorin/thorin.hpp>
#include <thorin/dockarea.hpp>
#include <thorin/menubar.hpp>
#include <thorin/progressbar.hpp>
#include <thorin/table.hpp>
#include <thorin/text.hpp>

using namespace thorin;

// Zion: Linux system information in a dock area. System on the left, memory at the bottom, CPU and
// GPU as tabs in the middle. Uptime, CPU and memory refresh every second. Closed panes come back
// from the View menu.

namespace {
    // The value of the first "key<sep>value" line in a file, trimmed; empty if there is none.
    std::string read_field(const std::string& path, const std::string& key, char separator) {
        std::ifstream file(path);
        std::string line;

        while (std::getline(file, line)) {
            const size_t at = line.find(separator);
            if (at == std::string::npos) {
                continue;
            }

            std::string name = line.substr(0, at);
            name.erase(name.find_last_not_of(" \t") + 1);
            if (name != key) {
                continue;
            }

            std::string value = line.substr(at + 1);
            value.erase(0, value.find_first_not_of(" \t\""));
            value.erase(value.find_last_not_of(" \t\"") + 1);
            return value;
        }
        return "";
    }

    // A /proc/meminfo value in KiB.
    uint64_t meminfo(const std::string& key) {
        const std::string value = read_field("/proc/meminfo", key, ':');
        return value.empty() ? 0 : std::stoull(value);
    }

    std::string gib(uint64_t kib) {
        return std::format("{:.1f} GiB", static_cast<double>(kib) / (1024.0 * 1024.0));
    }

    std::string uptime() {
        double seconds = 0.0;
        std::ifstream("/proc/uptime") >> seconds;

        const auto total = static_cast<uint64_t>(seconds);
        return std::format("{}d {}h {}m", total / 86400, total / 3600 % 24, total / 60 % 60);
    }

    // The average of every "cpu MHz" line in /proc/cpuinfo.
    std::string frequency() {
        std::ifstream file("/proc/cpuinfo");
        std::string line;
        double sum = 0.0;
        int count = 0;

        while (std::getline(file, line)) {
            if (line.starts_with("cpu MHz")) {
                sum += std::stod(line.substr(line.find(':') + 1));
                count += 1;
            }
        }
        return count > 0 ? std::format("{:.0f} MHz", sum / count) : "unknown";
    }

    struct CpuTimes {
        uint64_t idle = 0;
        uint64_t total = 0;
    };

    // The aggregate "cpu" line of /proc/stat first, then one per logical CPU.
    std::vector<CpuTimes> cpu_times() {
        std::ifstream file("/proc/stat");
        std::string line;
        std::vector<CpuTimes> times;

        while (std::getline(file, line) && line.starts_with("cpu")) {
            std::istringstream fields(line);
            std::string name;
            // user nice system idle iowait irq softirq steal; guest time is already in user.
            uint64_t value[8] = {};
            fields >> name;
            for (auto& v : value) {
                fields >> v;
            }

            CpuTimes t;
            t.idle = value[3] + value[4];
            for (const auto v : value) {
                t.total += v;
            }
            times.push_back(t);
        }
        return times;
    }

    float usage(const CpuTimes& before, const CpuTimes& after) {
        const uint64_t total = after.total - before.total;
        return total == 0 ? 0.0f : 1.0f - static_cast<float>(after.idle - before.idle) / static_cast<float>(total);
    }

    // Two columns of text; add_row returns the value cell so it can be updated.
    struct InfoTable: Table {
        InfoTable(Widget* parent): Table({"Property", "Value"}, parent) {}

        Text& add_row(const std::string& name, const std::string& value) {
            add_cell<Text>(name);
            return add_cell<Text>(value);
        }
    };
}

struct SystemPane: Pane {
    InfoTable table = InfoTable(this);
    Text* uptimeText = nullptr;

    SystemPane(): Pane("System") {
        column();
        padding(10);
        set_dock(ImGuiDir_Left).set_ratio(0.3f);

        utsname names{};
        uname(&names);

        table.add_row("OS", read_field("/etc/os-release", "PRETTY_NAME", '='));
        table.add_row("Hostname", names.nodename);
        table.add_row("Kernel", std::format("{} {}", names.sysname, names.release));
        table.add_row("Build", names.version);
        table.add_row("Architecture", names.machine);
        uptimeText = &table.add_row("Uptime", uptime());
    }

    void refresh() {
        uptimeText->set_title(uptime());
    }
};

struct CpuPane: Pane {
    InfoTable table = InfoTable(this);
    Text usageTitle = Text("Usage", this);
    ProgressBar total = ProgressBar(this);
    Text threadsTitle = Text("Per thread", this);
    // deque: adding bars never moves the widgets already parented to the pane.
    std::deque<ProgressBar> threads;
    Text* frequencyText = nullptr;
    std::vector<CpuTimes> previous = cpu_times();

    CpuPane(): Pane("CPU") {
        column(6);
        padding(10);

        const size_t count = previous.empty() ? 0 : previous.size() - 1;
        table.add_row("Model", read_field("/proc/cpuinfo", "model name", ':'));
        table.add_row("Vendor", read_field("/proc/cpuinfo", "vendor_id", ':'));
        table.add_row("Cores / threads", std::format("{} / {}", read_field("/proc/cpuinfo", "cpu cores", ':'), count));
        table.add_row("Cache", read_field("/proc/cpuinfo", "cache size", ':'));
        frequencyText = &table.add_row("Frequency", frequency());

        for (size_t i = 0; i < count; ++i) {
            threads.emplace_back(this);
        }
    }

    void refresh() {
        const std::vector<CpuTimes> current = cpu_times();
        if (current.size() != previous.size() || current.empty()) {
            return;
        }

        total.set_fraction(usage(previous[0], current[0]));
        for (size_t i = 0; i < threads.size(); ++i) {
            threads[i].set_fraction(usage(previous[i + 1], current[i + 1]));
            threads[i].set_overlay(std::format("CPU {}: {:.0f}%", i, threads[i].fraction() * 100.0f));
        }

        frequencyText->set_title(frequency());
        previous = current;
    }
};

struct MemoryPane: Pane {
    Text ramTitle = Text("RAM", this);
    ProgressBar ram = ProgressBar(this);
    Text swapTitle = Text("Swap", this);
    ProgressBar swap = ProgressBar(this);

    MemoryPane(): Pane("Memory") {
        column(6);
        padding(10);
        set_dock(ImGuiDir_Down).set_ratio(0.25f);
        refresh();
    }

    static void show_usage(ProgressBar& bar, uint64_t total, uint64_t available) {
        const uint64_t used = total - available;
        bar.set_fraction(total == 0 ? 0.0f : static_cast<float>(used) / static_cast<float>(total));
        bar.set_overlay(std::format("{} / {}", gib(used), gib(total)));
    }

    void refresh() {
        show_usage(ram, meminfo("MemTotal"), meminfo("MemAvailable"));
        show_usage(swap, meminfo("SwapTotal"), meminfo("SwapFree"));
    }
};

struct GpuPane: Pane {
    InfoTable table = InfoTable(this);
    Text* vendor = nullptr;
    Text* renderer = nullptr;
    Text* version = nullptr;
    Text* glsl = nullptr;
    bool queried = false;

    GpuPane(): Pane("GPU") {
        column();
        padding(10);

        vendor = &table.add_row("Vendor", "");
        renderer = &table.add_row("Renderer", "");
        version = &table.add_row("OpenGL", "");
        glsl = &table.add_row("GLSL", "");
    }

    static std::string gl_string(gl::GLenum name) {
        const auto* value = reinterpret_cast<const char*>(gl::glGetString(name));
        return value != nullptr ? value : "unknown";
    }

    bool show() override {
        // The window's GL context is current while it draws.
        if (!queried) {
            vendor->set_title(gl_string(gl::GL_VENDOR));
            renderer->set_title(gl_string(gl::GL_RENDERER));
            version->set_title(gl_string(gl::GL_VERSION));
            glsl->set_title(gl_string(gl::GL_SHADING_LANGUAGE_VERSION));
            queried = true;
        }
        return Pane::show();
    }
};

struct Zion: Widget {
    MenuBar menuBar = MenuBar(this);
    DockArea area = DockArea(this);
    // Added in this order: System splits off the left of the whole area, Memory the bottom of the rest,
    // CPU and GPU are tabs in the middle.
    SystemPane& system = area.add_pane<SystemPane>();
    MemoryPane& memory = area.add_pane<MemoryPane>();
    CpuPane& cpu = area.add_pane<CpuPane>();
    GpuPane& gpu = area.add_pane<GpuPane>();
    double lastRefresh = 0.0;

    Zion(const std::string& title): Widget(title) {
        column();
        area.flex_grow(1);

        menuBar.add_menu("File").add_item("Quit", [this] { app().exit(); }, "Ctrl+Q");

        auto& view = menuBar.add_menu("View");
        view.add_item("System", [this] { system.open(); });
        view.add_item("CPU", [this] { cpu.open(); });
        view.add_item("Memory", [this] { memory.open(); });
        view.add_item("GPU", [this] { gpu.open(); });
    }

    bool show() override {
        const double now = ImGui::GetTime();
        if (now - lastRefresh >= 1.0) {
            system.refresh();
            cpu.refresh();
            memory.refresh();
            lastRefresh = now;
        }
        return Widget::show();
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    // Full rate while the user interacts, 10 fps otherwise; still enough for the 1 s refresh.
    app.set_frame_config({.policy = FramePolicy::Adaptive});
    app.add_window<Zion>(std::string("Zion"));
    return app.exec();
}
