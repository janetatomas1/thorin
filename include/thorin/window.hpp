
#pragma once

#include <cstddef>
#include <filesystem>
#include <memory>
#include <span>

#include <SDL3/SDL.h>
#include <imgui.h>

#include "thorin/widget.hpp"
#include "thorin/gpubackend.hpp"

namespace thorin {
    class Thorin;
    class WindowManager;

    class Window {
    protected:
        uint64_t id_ = 0;
        WindowManager *manager_ = nullptr;
        std::unique_ptr<Widget> rootWidget_;
        std::unique_ptr<GPUBackend> backend_;
    public:
        virtual ~Window() = default;
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window &&window) noexcept = default;
        Window& operator=(Window &&window) noexcept = default;
        Window(const WindowConfig &config ={}, std::unique_ptr<Widget> widget = nullptr);
        void init();
        void destroy();
        void update();
        void close();
        Thorin &app();
        [[nodiscard]] uint64_t id() const;
        void set_window_manager(WindowManager *manager);
        void set_root_widget(std::unique_ptr<Widget> widget);
        Widget* root_widget();
        GPUBackend* backend();
        void maximize();
        void minimize();
        void set_size(int width, int height);
        void set_title(const std::string &title);
        [[nodiscard]] const std::string& title() const;
        [[nodiscard]] int height() const;
        [[nodiscard]] int width() const;
        // Adds a font to this window's atlas, for Style::set_font. nullptr if it can't be loaded.
        // Makes this window's contexts current. Each call adds a new font, so load once and reuse.
        [[nodiscard]] ImFont* load_font(const std::filesystem::path& path);
        // Copies data.
        [[nodiscard]] ImFont* load_font(std::span<const std::byte> data);
        // Primary font for the whole window (ImGuiIO::FontDefault); widget styles still override it.
        // Must come from this window's load_font(). nullptr = ImGui's default font.
        void set_font(ImFont* font);
    };
}
