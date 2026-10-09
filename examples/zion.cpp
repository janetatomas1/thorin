
#include <thorin/thorin.hpp>
#include <thorin/dockarea.hpp>
#include <thorin/pane.hpp>

using namespace thorin;

struct Overview : public Pane {
    Overview(): Pane("Overview") {}
};

struct Zion: DockArea {
    Zion(): DockArea() {
        add_pane<Overview>()
        .set_dock(ImGuiDir_Left)
        .set_closable(false);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<Zion>();
    return app.exec();
}
