
#pragma once

#include <string>

#include <SDL3/SDL.h>

struct WindowConfig {
    int width = 1920, height = 1080;
    std::string title = "Thorin";
    bool maximized = false;
};

namespace thorin {
    class Window;

    class GPUBackend {
    protected:
        SDL_WindowFlags window_flags = 0;
        SDL_Window* handle_ = nullptr;
        Window *window_;
        SDL_Window *sdlWindow_;
        WindowConfig config_;

    public:
        GPUBackend(Window *window, const WindowConfig &config);
        virtual ~GPUBackend() = default;
        virtual void init() {}
        virtual void update() {}
        virtual void destroy() {}
        void maximize();
        void minimize();
        void set_size(int width, int height);
        void set_title(const std::string& title);
        const std::string& title() const;
        [[nodiscard]] int height() const;
        [[nodiscard]] int width() const;
    };
}
