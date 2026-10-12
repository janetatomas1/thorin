#include <SDL3/SDL.h>

#include "thorin/clipboard.hpp"
#include "thorin/log.hpp"

namespace thorin::clipboard {
    void set_text(std::string_view text) {
        // SDL wants a null-terminated string.
        if (!SDL_SetClipboardText(std::string(text).c_str())) {
            logger().warn("can't set clipboard text: {}", SDL_GetError());
        }
    }

    std::string text() {
        char* raw = SDL_GetClipboardText();

        if (raw == nullptr) {
            return {};
        }
        std::string result(raw);
        SDL_free(raw);
        return result;
    }

    bool has_text() {
        return SDL_HasClipboardText();
    }
}
