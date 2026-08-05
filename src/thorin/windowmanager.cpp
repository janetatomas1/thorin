
#include "thorin/windowmanager.hpp"

#include <SDL3/SDL.h>
#include "thorin/glwindow.hpp"

namespace thorin {
    void WindowManager::init() {
        // TODO: take care of failure case
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);

        windows_.push_back(std::make_unique<GLWindow>());
        windows_[0]->init();
    }

    void WindowManager::destroy() {
        SDL_Quit();
    }

    void WindowManager::update() {
        SDL_Delay(100);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            Window* win = static_cast<Window*>(
                SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(&event)), "WRAPPER", nullptr)
            );

            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&  win != nullptr) {
                win->event(&event);
            }
        }
        for (auto &window: windows_) {
            window->update();
        }
    }

    size_t WindowManager::count() const {
        return windows_.size();
    }
}
