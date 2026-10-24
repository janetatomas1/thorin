
#pragma once

#include <string>
#include <memory>

#include <SDL3/SDL.h>
#include <globjects/Texture.h>

#include "thorin/gpubackend.hpp"

namespace thorin {
    // OpenGL core-profile backend (3.3 by default, see GLBackendConfig): an SDL GL context and
    // ImGui context per window.
    class GLBackend: public GPUBackend {
        SDL_GLContext glContext_;
        GLBackendConfig glConfig_;
        // This window's copy of background_, in its GL context.
        std::unique_ptr<globjects::Texture> backgroundImage_;

    protected:
        ImTextureID upload_background() override;

    public:
        GLBackend(const WindowConfig &config = {}, const GLBackendConfig &glConfig = {});
        void init() override;
        void destroy() override;
        void update(Widget *rootWidget) override;
        void make_current() override;
    };
}
