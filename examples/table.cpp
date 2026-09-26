#include <thorin/thorin.hpp>
#include <thorin/table.hpp>
#include <thorin/text.hpp>
#include <thorin/button.hpp>
#include <thorin/checkbox.hpp>
#include <iostream>

using namespace thorin;

struct TableExample: Widget {
    // Declared before the table, so they outlive it.
    Text alpha = Text("alpha");
    Checkbox alphaEnabled = Checkbox("##enabled");
    Button alphaRun = Button("Run", [] { std::cout << "Run alpha" << std::endl; });

    Text beta = Text("beta");
    Checkbox betaEnabled = Checkbox("##enabled");
    Button betaRun = Button("Run", [] { std::cout << "Run beta" << std::endl; });

    Table table = Table({"Name", "Count", "Ratio", "Enabled", "Action"}, this);

    TableExample(const std::string& title): Widget(title) {
        column();
        padding(10);

        table.add_cell(alpha).add_cell(1).add_cell(0.5f).add_cell(alphaEnabled).add_cell(alphaRun);
        table.add_cell(beta).add_cell(2).add_cell(0.25f).add_cell(betaEnabled).add_cell(betaRun);
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<TableExample>(std::string("Table Example"));
    return app.exec();
}
