#include <thorin/thorin.hpp>
#include <thorin/dockarea.hpp>
#include <thorin/selectable.hpp>
#include <thorin/text.hpp>
#include <thorin/textinput.hpp>
#include <array>
#include <deque>
#include <format>
#include <memory>
#include <string>

using namespace thorin;
using namespace literals;

// A small IDE in a dock area: the explorer on the left, output at the bottom, editor and notes as
// tabs in the middle. Drag the tabs to rearrange, split or float the panes; picking a file opens
// it in the editor and logs it.

struct File {
    const char* name;
    const char* content;
};

constexpr std::array files{
    File{"main.cpp", "#include \"app.hpp\"\n\nint main() {\n    return App().run();\n}\n"},
    File{"app.hpp", "#pragma once\n\nstruct App {\n    int run();\n};\n"},
    File{"app.cpp", "#include \"app.hpp\"\n\nint App::run() {\n    return 0;\n}\n"},
    File{"CMakeLists.txt", "cmake_minimum_required(VERSION 3.28)\nproject(app)\nadd_executable(app main.cpp app.cpp)\n"},
};

struct Output: Pane {
    // deque: adding lines never moves the widgets already parented to the pane.
    std::deque<Text> lines;

    Output(): Pane("Output") {
        column(2);
        padding(10);
        set_dock(ImGuiDir_Down).set_ratio(0.3f);
        log("Ready.");
    }

    void log(const std::string& line) {
        lines.emplace_back(std::format("[{:02}] {}", lines.size() + 1, line), this);
    }
};

struct Editor: Pane {
    Text file = Text("No file open", this);
    TextInput text = TextInput("##text", this);

    Editor(): Pane("Editor") {
        column(8);
        padding(10);
        text.set_multiline(true);
        text.flex_grow(1);
    }

    void open(const File& f) {
        file.set_title(f.name);
        text.set_value(f.content);
    }
};

struct Notes: Pane {
    TextInput text = TextInput("##notes", this);

    Notes(): Pane("Notes") {
        padding(10);
        text.set_multiline(true);
        text.set_value("Drag a tab onto another pane's edge to split it,\nor out of the area to float it.");
        text.flex_grow(1);
    }
};

struct Explorer: Pane {
    Selectable list;

    Explorer(Editor& editor, Output& output): Pane("Explorer"),
    list([&editor, &output](std::vector<size_t> indices) {
        if (indices.empty()) {
            return;
        }

        const File& f = files[indices.front()];
        editor.open(f);
        output.log(std::format("Opened {}", f.name));
    }, this) {
        column();
        padding(10);
        set_dock(ImGuiDir_Left).set_ratio(0.2f);

        list.set_flags(ImGuiMultiSelectFlags_SingleSelect | ImGuiMultiSelectFlags_ScopeRect);
        for (const File& f : files) {
            list.add_item(f.name);
        }
    }
};

struct DockAreaExample: Widget {
    DockArea area = DockArea(this);

    DockAreaExample(const std::string& title): Widget(title) {
        column();
        padding(10);
        area.flex_grow(1);

        // Split off in add order: the explorer takes the left of the whole area, the output the
        // bottom of what is left; editor and notes are tabs in the middle. The explorer needs the
        // editor and output, so those are built first and added after it.
        area.add_pane(std::make_unique<Editor>()).set_dock(ImGuiDir_Left).set_ratio(0.2f);
        area.add_pane(std::make_unique<Editor>()).set_dock(ImGuiDir_Down).set_ratio(0.2f);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<DockAreaExample>(std::string("DockArea Example"));
    return app.exec();
}
