#include "thorin/colorpicker.hpp"

namespace thorin::detail {
    ImGuiColorEditFlags color_edit_options() {
        return ImGui::GetIO().ConfigColorEditFlags;
    }
}
