#include <imgui_internal.h>

#include "thorin/colorpicker.hpp"

namespace thorin::detail {
    ImGuiColorEditFlags color_edit_options() {
        return GImGui->ColorEditOptions;
    }
}
