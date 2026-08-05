
#include "thorin/windowmanager.hpp"

#include <SDL3/SDL.h>
#include "thorin/glwindow.hpp"

namespace thorin {
    void WindowManager::init() {
        // TODO: take care of failure case
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);

        windows_.push_back(std::make_unique<GLWindow>());
    }

    void WindowManager::destroy() {
        SDL_Quit();
    }

    void WindowManager::update() {
        for (auto &window: windows_) {
            window->update();
        }
    }

    size_t WindowManager::count() const {
        return windows_.size();
    }
}
