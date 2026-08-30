
#include <thorin/thorin.hpp>

struct HelloWidget: public thorin::Widget {
    HelloWidget(const std::string& title): Widget(title) {};

    bool show() override {
        ImGui::Button(title().c_str());
        return false;
    }
};

int main(const int argc, char **argv) {
    auto app = thorin::Thorin(argc, argv);
    app.add_window<HelloWidget>(std::string("Hello, Thorin"));
    return app.exec();
}
