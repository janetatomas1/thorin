
#include "thorin/windowmanager.hpp"

#include <SDL3/SDL.h>
#include "thorin/glwindow.hpp"
#include "thorin/thorin.hpp"

namespace thorin {
    WindowManager::WindowManager(Thorin &app) : app_(app) {}

    void WindowManager::init() {
        // TODO: take care of failure case
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
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

    Thorin& WindowManager::app() {
        return app_;
    }

    Window* WindowManager::add_window(std::unique_ptr<Window> window) {
        auto ptr = window.get();
        app_.add_action(std::move([window = std::move(window), this] () mutable {
            window->init();
            windows_.push_back(std::move(window));
        }));

        return ptr;
    }
}
