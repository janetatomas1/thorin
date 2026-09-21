
#include "thorin/text.hpp"

namespace thorin {
    Text::Text(const std::string& text, Widget* parent)
    : Widget(text, parent) {}

    bool Text::show() {
        // Wrap at the widget's own width when layout gave it one, otherwise at the window edge.
        const float wrapPos = width() > 0.0f ? ImGui::GetCursorPosX() + width() : 0.0f;

        ImGui::PushTextWrapPos(wrapPos);
        ImGui::TextUnformatted(title().c_str());
        ImGui::PopTextWrapPos();

        return false;
    }
}
