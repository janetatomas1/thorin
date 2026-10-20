
#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <function2/function2.hpp>
#include <imgui.h>

#include "thorin/windowconfig.hpp"
#include "thorin/theme.hpp"
#include "thorin/image.hpp"


namespace thorin {
    class Window;
    class Widget;

    // Draws a window's background each frame into its root draw list: above WindowBg and the background image,
    // below every widget. Called with the window's rectangle.
    using BackgroundPainter = fu2::unique_function<void(ImDrawList& list, ImVec2 min, ImVec2 max)>;

    // The per-window platform and GPU part of a Window: SDL window, GPU and ImGui contexts, theme, fonts,
    // background and icon. Subclasses implement one graphics API (GLBackend).
    class GPUBackend {
    protected:
        SDL_Window* handle_ = nullptr;
        Window *window_;
        WindowConfig config_;
        // Display content scale the theme's sizes are multiplied by.
        float scale_ = 1.0f;
        // The theme font this window's atlas borrows, to tell when the theme's font changes.
        const Font* font_ = nullptr;
        ImFont* imFont_ = nullptr;
        // Drawn stretched behind the root widget; the backend uploads its own texture of it.
        const Image* background_ = nullptr;
        // 0 = invisible, 1 = opaque; WindowBg shows through the rest.
        float backgroundOpacity_ = 1.0f;
        // The texture no longer matches background_; re-uploaded on the next update().
        bool backgroundDirty_ = false;
        BackgroundPainter backgroundPainter_;
        const Image* icon_ = nullptr;

    public:
        GPUBackend(const WindowConfig &config);
        virtual ~GPUBackend() = default;
        virtual void init() = 0;
        // Draws one frame: the root widget in an ImGui window filling the SDL window.
        virtual void update(Widget *rootWidget) = 0;
        virtual void destroy() = 0;
        virtual void process_event(const SDL_Event& event) = 0;
        // Makes this window's GPU and ImGui contexts the current ones.
        virtual void make_current() = 0;
        void set_window(Window *window);
        // Copies the theme into the current ImGui / ImPlot / ImPlot3D contexts, scaled; call make_current() first.
        void apply_theme(const Theme &theme);
        [[nodiscard]] const Image* background() const;
        // nullptr removes it. The image must stay alive while it is set.
        void set_background(const Image* image, float opacity = 1.0f);
        // nullptr / empty removes it.
        void set_background_painter(BackgroundPainter painter);
        [[nodiscard]] bool has_background_painter() const;
        [[nodiscard]] const Image* icon() const;
        // Applied now if the window exists, else remembered for Window::init(). SDL copies the pixels.
        void set_icon(const Image* image);
        Window* window();
        [[nodiscard]] SDL_Window* handle() const;
        void maximize();
        void minimize();
        void set_size(int width, int height);
        void set_title(const std::string& title);
        [[nodiscard]] const std::string& title() const;
        [[nodiscard]] int height() const;
        [[nodiscard]] int width() const;
    };
}
