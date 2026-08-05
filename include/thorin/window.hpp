
#pragma once

#include <SDL3/SDL.h>

namespace thorin {
    class Thorin;
    class WindowManager;

    struct WindowConfig {
        int width = 1920, height = 1080;
        std::string title = "Thorin";
        bool maximized = false;
    };

    class Window {
    protected:
        SDL_WindowFlags window_flags;
        SDL_Window* handle_;
        WindowConfig config_;

        WindowManager *manager_;
    public:
        virtual ~Window() = default;
        Window() = default;
        Window(const WindowConfig &config): config_(config) {};
        virtual void init() = 0;
        virtual void destroy() = 0;
        virtual void update() = 0;
        virtual bool event(const SDL_Event *event);
    };

    inline bool Window::event(const SDL_Event *event) {
        if (event != nullptr && event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
            // SDL_DestroyWindow(handle_);
        }
    }
}
