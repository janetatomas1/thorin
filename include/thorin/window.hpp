
#pragma once

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
        SDL_Window *handle_;

        WindowManager *manager_ = nullptr;
        std::unique_ptr<Widget> mainWidget_ = std::make_unique<Widget>();
        std::unique_ptr<GPUBackend> backend_;
    public:
        virtual ~Window() = default;
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window &&window) noexcept = default;
        Window& operator=(Window &&window) noexcept = default;
        Window(const WindowConfig &config ={});
        void init();
        void destroy();
        void update();
        void close();
        Thorin &app();
        uint64_t id() const;
        void set_window_manager(WindowManager *manager);
        void set_main_widget(std::unique_ptr<Widget> widget);
        Widget* main_widget();
        GPUBackend* backend();
        void maximize();
        void minimize();
        void set_size(int width, int height);
        void set_title(const std::string &title);
        const std::string& title() const;
        [[nodiscard]] int height() const;
        [[nodiscard]] int width() const;
    };
}
