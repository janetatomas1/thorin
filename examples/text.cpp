#include <thorin/thorin.hpp>
#include <thorin/text.hpp>

using namespace thorin;
using namespace literals;

struct TextExample: Widget {
    Text text = Text(
        "This text is wrapped to the width of its widget. Resize the window "
        "and the lines break again at the widget's edge instead of running off the side.",
        this
    );

    TextExample(const std::string& title): Widget(title) {
        center();
        text.width(40_pcts);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<TextExample>(std::string("Text Example"));
    return app.exec();
}
