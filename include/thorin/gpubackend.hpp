
#pragma once

class SDL_Window;
class Window;

namespace thorin {
    class GPUBackend {
        Window *window_;
        SDL_Window *sdlWindow_;

    public:
        GPUBackend(Window *window = nullptr);
        virtual ~GPUBackend() = default;
        virtual void init() {}
        virtual void update() {}
        virtual void destroy() {}
    };
}
