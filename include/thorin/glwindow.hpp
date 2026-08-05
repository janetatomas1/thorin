
#include "thorin/window.hpp"

#include <SDL3/SDL.h>

namespace thorin {
    class GLWindow: public Window {
        SDL_GLContext gl_context;

    public:
        void init() override;
        void destroy() override;
        void update() override;
    };
}
