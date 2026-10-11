
#include <thorin/thorin.hpp>
#include <thorin/tabwidget.hpp>
#include <thorin/pane.hpp>
#include <thorin/theme.hpp>

using namespace thorin;

struct CPUTab : public Widget {
    CPUTab(): Widget("CPU") {}
};

struct Zion: TabWidget {
    Zion(): TabWidget() {
        auto & cpu = add_tab<CPUTab>();
        add_tab("GPU");
        add_tab("Disk");
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.set_theme(Theme::matrix());
    app.add_window<Zion>();
    return app.exec();
}
