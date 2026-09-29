#include <thorin/thorin.hpp>
#include <thorin/popup.hpp>
#include <thorin/text.hpp>
#include <thorin/button.hpp>
#include <iostream>

using namespace thorin;
using namespace literals;

// Children are laid out by Yoga inside the popup, relative to its top-left corner.
// Clicking outside the popup closes it.
struct ActionsPopup: Popup {
    Button copy = Button("Copy", [this] {
        std::cout << "Copied" << std::endl;
        close();
    }, this);
    Button paste = Button("Paste", [this] {
        std::cout << "Pasted" << std::endl;
        close();
    }, this);

    ActionsPopup(Widget* parent): Popup("Actions", parent) {
        column(6);
        padding(8);
        width(120);
        height(80);
        position(10, YGEdgeLeft);
        position(50, YGEdgeTop);
    }
};

struct PopupExample: Widget {
    Text title = Text("Popup example", this);
    Button button = Button("Actions", [this] { popup.open(); }, this);
    ActionsPopup popup = ActionsPopup(this);

    PopupExample(const std::string& title): Widget(title) {
        column(10);
        padding(10);
        align_items(YGAlignFlexStart);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<PopupExample>(std::string("Popup Example"));
    return app.exec();
}
