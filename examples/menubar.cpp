
#include <thorin/thorin.hpp>
#include <thorin/menubar.hpp>
#include <iostream>

using namespace thorin;
using namespace literals;

struct MenuBarExample: Widget {
    MenuBar menuBar = MenuBar(this);

    MenuBarExample(const std::string& title): Widget(title) {
        column();

        auto& file = menuBar.add_menu("File");
        file.add_item("New", [] { std::cout << "New" << std::endl; }, "Ctrl+N");
        file.add_item("Open", [] { std::cout << "Open" << std::endl; }, "Ctrl+O");

        auto& recent = file.add_menu("Open Recent");
        recent.add_item("notes.txt", [] { std::cout << "Open notes.txt" << std::endl; });
        recent.add_item("todo.md", [] { std::cout << "Open todo.md" << std::endl; });

        file.add_item("Save", [] { std::cout << "Save" << std::endl; }, "Ctrl+S").set_enabled(false);
        file.add_separator();
        file.add_item("Quit", [this] { app().exit(); }, "Ctrl+Q");

        auto& view = menuBar.add_menu("View");
        auto& statusBar = view.add_item("Status Bar").set_checkable(true).set_checked(true);
        statusBar.set_callback([&statusBar] {
            std::cout << "Status bar: " << (statusBar.checked() ? "on" : "off") << std::endl;
        });

        menuBar.add_menu("Help").add_item("About", [] { std::cout << "About" << std::endl; });
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<MenuBarExample>(std::string("MenuBar Example"));
    return app.exec();
}
