
#include <thorin/thorin.hpp>

int main(const int argc, char **argv) {
    auto app = thorin::Thorin(argc, argv);
    app.add_window();
    return app.exec();
}
