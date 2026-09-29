#include <algorithm>

#include <thorin/thorin.hpp>
#include <thorin/progressbar.hpp>
#include <thorin/button.hpp>
#include <thorin/text.hpp>

using namespace thorin;
using namespace literals;

struct ProgressBarExample: Widget {
    Text title = Text("Progress bar example", this);
    ProgressBar progress = ProgressBar(this);
    Button step = Button("+10%", [this] {
        progress.set_fraction(std::min(progress.fraction() + 0.1f, 1.0f));
    }, this);
    Button reset = Button("Reset", [this] { progress.set_fraction(0.0f); }, this);

    Text loadingLabel = Text("Unknown length:", this);
    ProgressBar loading = ProgressBar(this);
    ProgressBar custom = ProgressBar(this);

    ProgressBarExample(const std::string& title): Widget(title) {
        column(10);
        padding(10);
        align_items(YGAlignFlexStart);

        progress.width(50_pcts);
        loading.set_indeterminate(true).set_overlay("Loading...").width(50_pcts);
        custom.set_fraction(0.3f).set_overlay("3 / 10 files").width(300).height(40);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<ProgressBarExample>(std::string("ProgressBar Example"));
    return app.exec();
}
