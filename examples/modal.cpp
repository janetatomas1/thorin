#include <thorin/thorin.hpp>
#include <thorin/modal.hpp>
#include <thorin/text.hpp>
#include <thorin/button.hpp>
#include <iostream>

using namespace thorin;
using namespace literals;

// Children are laid out by Yoga inside the modal, relative to its top-left corner.
struct ConfirmDialog: Modal {
    Text text = Text("Delete the file?", this);
    Button ok = Button("OK", [this] {
        std::cout << "Deleted" << std::endl;
        close();
    }, this);
    Button cancel = Button("Cancel", [this] { close(); }, this);

    ConfirmDialog(Widget* parent): Modal("Confirm", parent) {
        column(8);
        padding(10);
        width(240);
        height(140);
        position(30_pcts, YGEdgeLeft);
        position(30_pcts, YGEdgeTop);
    }
};

struct ModalExample: Widget {
    Text title = Text("Modal example", this);
    Button button = Button("Delete", [this] { dialog.open(); }, this);
    ConfirmDialog dialog = ConfirmDialog(this);

    ModalExample(const std::string& title): Widget(title) {
        column(10);
        padding(10);
        center();
        button.max_width(20_pcts).min_width(10_pcts);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<ModalExample>(std::string("Modal Example"));
    return app.exec();
}
