
#pragma once

#include <string>
#include <memory>

#include <SDL3/SDL.h>
#include <globjects/Texture.h>

#include "thorin/gpubackend.hpp"

struct ImGuiContext;
struct ImPlotContext;
struct ImPlot3DContext;

namespace thorin {
    // OpenGL 3.3 core backend: an SDL GL context and ImGui / ImPlot / ImPlot3D contexts per window.
    class GLBackend: public GPUBackend {
        SDL_GLContext gl_context;
        ImGuiContext* imguiContext_ = nullptr;
        ImPlotContext* implotContext_ = nullptr;
        ImPlot3DContext* implot3dContext_ = nullptr;
        const std::string glsl_version = "#version 330 core";
        // This window's copy of background_, in its GL context.
        std::unique_ptr<globjects::Texture> backgroundTexture_;

        // Re-uploads backgroundTexture_ if background_ changed, then draws it over the root window's area.
        void draw_background();

    public:
        GLBackend(const WindowConfig &config = {});
        void init() override;
        void destroy() override;
        void update(Widget *rootWidget) override;
        void process_event(const SDL_Event& event) override;
        void make_current() override;
    };
}
