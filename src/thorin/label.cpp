
#include "thorin/label.hpp"

namespace thorin {
    Label::Label(const std::string& text, Widget* parent)
    : Widget(text, parent) {}

    bool Label::show() {
        ImGui::TextUnformatted(title().c_str());
        return false;
    }
}
