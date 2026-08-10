
#pragma once

#include <SDL3/SDL.h>
#include <string>

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
        uint64_t id_ = 0;
        SDL_WindowFlags window_flags = 0;
        SDL_Window* handle_ = nullptr;
        WindowConfig config_;

        WindowManager *manager_ = nullptr;
    public:
        virtual ~Window() = default;
        Window();
        Window(const WindowConfig &config): config_(config) {};
        virtual void init() = 0;
        virtual void destroy() = 0;
        virtual void update() = 0;
        virtual void event(const SDL_Event *event);
        void close();
        void maximize();
        void minimize();
        Thorin &app();
        uint64_t id();
        void set_window_manager(WindowManager *manager);
    };
}
