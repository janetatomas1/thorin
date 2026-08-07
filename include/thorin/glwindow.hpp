
#include <string>

#include "thorin/window.hpp"

#include <SDL3/SDL.h>

namespace thorin {
    class GLWindow: public Window {
        SDL_GLContext gl_context;
        std::string glsl_version = "#version 330 core";

    public:
        void init() override;
        void destroy() override;
        void update() override;
        void make_current();
    };
}
