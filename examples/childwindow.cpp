#include <thorin/thorin.hpp>
#include <thorin/childwindow.hpp>
#include <thorin/text.hpp>
#include <thorin/button.hpp>
#include <deque>
#include <format>
#include <iostream>

using namespace thorin;
using namespace literals;

// A bordered child window with more lines than fit: Yoga padding and gap apply inside it,
// and it scrolls because its height is fixed.
struct LogPanel: ChildWindow {
    // deque: adding lines never moves the widgets already parented to the panel.
    std::deque<Text> lines;

    LogPanel(Widget* parent): ChildWindow("Log", parent) {
        set_child_flags(ImGuiChildFlags_Borders);
        column(4);
        padding(10);
        height(200);

        for (int i = 0; i < 40; ++i) {
            lines.emplace_back(std::format("Log line {}", i), this);
        }
    }
};

// Absolutely positioned: takes no space in the column and is drawn above the window's items.
struct Overlay: ChildWindow {
    Text text = Text("Floating panel", this);
    Button button = Button("Click", [] { std::cout << "Clicked" << std::endl; }, this);

    Overlay(Widget* parent): ChildWindow("Overlay", parent) {
        set_child_flags(ImGuiChildFlags_Borders);
        column(6);
        padding(8);
        width(180);
    }
};

struct ChildWindowExample: Widget {
    Text title = Text("Child window example", this);
    LogPanel log = LogPanel(this);
    Text footer = Text("Below the panel", this);
    Overlay overlay = Overlay(this);

    ChildWindowExample(const std::string& title): Widget(title) {
        column(10);
        padding(10);
        overlay.margin(10_pcts);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<ChildWindowExample>(std::string("ChildWindow Example"));
    return app.exec();
}
