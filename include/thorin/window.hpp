
#pragma once

#include <SDL3/SDL.h>

namespace thorin {
    struct WindowConfig {
        int width = 1920, height = 1080;
        std::string title;
        bool maximized = false;
    };

    class Window {
    protected:
        SDL_WindowFlags window_flags;
        SDL_Window* window;
        WindowConfig config_;

    public:
        Window() = default;
        Window(const WindowConfig &config): config_(config) {};
        virtual void init() = 0;
        virtual void destroy() = 0;
        virtual void update() = 0;
    };
}
