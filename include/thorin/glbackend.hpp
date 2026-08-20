
#pragma once

#include <string>
#include <SDL3/SDL.h>

#include "thorin/gpubackend.hpp"


namespace thorin {
    class GLBackend: public GPUBackend {
        SDL_GLContext gl_context;
        const std::string glsl_version = "#version 330 core";

    public:
        GLBackend(const WindowConfig &config = {});
        void init() override;
        void destroy() override;
        void update(Widget *rootWidget) override;
        void make_current();
    };
}
