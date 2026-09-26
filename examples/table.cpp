#include <thorin/thorin.hpp>
#include <thorin/table.hpp>
#include <thorin/text.hpp>
#include <thorin/button.hpp>
#include <thorin/checkbox.hpp>
#include <iostream>

using namespace thorin;
using namespace literals;

struct TableExample: Widget {
    Table table = Table({"Name", "Count", "Ratio", "Enabled", "Action"}, this);

    TableExample(const std::string& title): Widget(title) {
        column();
        padding(10);

        int n = 0;
        for (const char* name : {"alpha", "beta", "gamma"}) {
            table.add_cell<Text>(name);
            table.add_cell(++n);
            table.add_cell(n / 3.0f);
            table.add_cell<Checkbox>(std::string("##enabled"));
            table.add_cell<Button>("Run", [name] { std::cout << "Run " << name << std::endl; });
        }

        table.width(40_pcts).margin(20_pcts);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<TableExample>(std::string("Table Example"));
    return app.exec();
}
