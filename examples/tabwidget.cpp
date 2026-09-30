#include <thorin/thorin.hpp>
#include <thorin/tabwidget.hpp>
#include <thorin/text.hpp>
#include <thorin/button.hpp>
#include <thorin/checkbox.hpp>
#include <iostream>
#include <string>

using namespace thorin;
using namespace literals;

// A page's title is its tab label.
struct GeneralPage: Widget {
    Text text = Text("General settings", this);
    Checkbox autosave = Checkbox("Autosave", [](bool value) {
        std::cout << "Autosave " << value << std::endl;
    }, this);

    GeneralPage(): Widget("General") {
        column(10);
        padding(10);
    }
};

struct AboutPage: Widget {
    Text text = Text("Thorin tab widget example", this);
    Button button = Button("Hello", [] { std::cout << "Hello" << std::endl; }, this);

    AboutPage(): Widget("About") {
        column(10);
        padding(10);
    }
};

// Pages added at runtime by the "Add tab" button.
struct NewPage: Widget {
    Text text;

    NewPage(const std::string& title): Widget(title), text("This is " + title, this) {
        padding(10);
    }
};

struct TabWidgetExample: Widget {
    int added = 0;
    Button addTab = Button("Add tab", [this] {
        tabs.add_tab<NewPage>("Tab " + std::to_string(++added));
    }, this);
    TabWidget tabs = TabWidget(this);

    TabWidgetExample(const std::string& title): Widget(title) {
        column(10);
        padding(10);

        addTab.align_self(YGAlignFlexStart);

        tabs.add_tab<GeneralPage>();
        tabs.add_tab<AboutPage>();
        tabs.flex_grow(1);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<TabWidgetExample>(std::string("TabWidget Example"));
    return app.exec();
}
