
#pragma once

#include <string>
#include <memory>

#include <SDL3/SDL.h>
#include <globjects/Texture.h>

#include "thorin/gpubackend.hpp"

namespace thorin {
    // OpenGL 3.3 core backend: an SDL GL context and ImGui / ImPlot / ImPlot3D contexts per window.
    class GLBackend: public GPUBackend {
        SDL_GLContext gl_context;
        const std::string glsl_version = "#version 330 core";
        // This window's copy of background_, in its GL context.
        std::unique_ptr<globjects::Texture> backgroundTexture_;

    protected:
        ImTextureID background_texture() override;

    public:
        GLBackend(const WindowConfig &config = {});
        void init() override;
        void destroy() override;
        void update(Widget *rootWidget) override;
        void make_current() override;
    };
}
