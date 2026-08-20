
#pragma once

#include <string>
#include <memory>

#include <SDL3/SDL.h>

#include "thorin/widget.hpp"
#include "thorin/gpubackend.hpp"

namespace thorin {
    class Thorin;
    class WindowManager;

    class Window {
    protected:
        uint64_t id_ = 0;
        SDL_WindowFlags window_flags = 0;
        SDL_Window* handle_ = nullptr;
        WindowConfig config_;

        WindowManager *manager_ = nullptr;
        std::unique_ptr<Widget> mainWidget_ = std::make_unique<Widget>();
        std::unique_ptr<GPUBackend> backend_;
    public:
        virtual ~Window() = default;
        Window();
        Window(Window &&window) noexcept = default;
        Window& operator=(Window &&window) noexcept = default;
        Window(const WindowConfig &config): config_(config) {};
        virtual void init();
        virtual void destroy();
        virtual void update();
        void close();
        void maximize();
        void minimize();
        Thorin &app();
        uint64_t id() const;
        void set_window_manager(WindowManager *manager);
        void set_main_widget(std::unique_ptr<Widget> widget);
        Widget* main_widget();
    };
}
