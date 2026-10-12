#pragma once

#include <string>
#include <string_view>

namespace thorin::clipboard {
    // The system clipboard, shared with ImGui's text inputs. Needs no window or ImGui context, only a live
    // Thorin (SDL video is initialized with it). UI thread only.

    void set_text(std::string_view text);
    // Empty when the clipboard holds no text.
    [[nodiscard]] std::string text();
    [[nodiscard]] bool has_text();
}
