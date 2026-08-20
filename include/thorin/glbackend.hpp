
#pragma once

#include <string>

#include "thorin/gpubackend.hpp"

#include <SDL3/SDL.h>

namespace thorin {
    class GLBackend: public GPUBackend {
        SDL_GLContext gl_context;
        const std::string glsl_version = "#version 330 core";

    public:
        GLBackend(Window *window);
        void init() override;
        void destroy() override;
        void update() override;
        void make_current();
    };
}
