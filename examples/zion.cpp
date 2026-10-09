#include <thorin/thorin.hpp>

using namespace thorin;

struct Zion: Widget {
    Zion(const std::string& title): Widget(title) {}
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<Zion>(std::string("Zion"));
    return app.exec();
}
