#include <thorin/thorin.hpp>
#include <thorin/separator.hpp>
#include <thorin/text.hpp>

using namespace thorin;
using namespace literals;

// Vertical separators between items in a row: they stretch to the row's height.
struct Toolbar: Widget {
    Text file = Text("File", this);
    Separator first = Separator("", this);
    Text edit = Text("Edit", this);
    Separator second = Separator("", this);
    Text view = Text("View", this);

    Toolbar(Widget* parent): Widget("", parent) {
        row(8);
        first.set_vertical(true);
        second.set_vertical(true);
    }
};

struct SeparatorExample: Widget {
    Text top = Text("Plain separator, full width:", this);
    Separator plain = Separator("", this);

    Text half = Text("50% wide:", this);
    Separator narrow = Separator("", this);

    Separator titled = Separator("Section", this);
    Separator centred = Separator("Centred", this);
    Separator right = Separator("Right", this);
    Separator shortLeft = Separator("40 px to the left", this);
    Separator noLeft = Separator("No line to the left", this);
    Separator narrowTitled = Separator("Narrow", this);

    Toolbar toolbar = Toolbar(this);

    SeparatorExample(const std::string& title): Widget(title) {
        column(10);
        padding(10);

        narrow.width(50_pcts);
        centred.set_text_align(0.5f);
        right.set_text_align(1.0f);
        shortLeft.set_left_width(40);
        noLeft.set_left_width(0);
        narrowTitled.width(200);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<SeparatorExample>(std::string("Separator Example"));
    return app.exec();
}
