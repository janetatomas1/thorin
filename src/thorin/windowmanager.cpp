
#include "thorin/windowmanager.hpp"
#include <iostream>

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
        window->set_window_manager(this);
        app_.add_action(std::move([window = std::move(window), this] () mutable {
            window->init();
            windows_.push_back(std::move(window));
        }));

        return ptr;
    }

    Window* WindowManager::get_window_at(const size_t index) {
        return windows_[index].get();
    }

    Window* WindowManager::get_window(uint64_t id) {
        auto window = std::find_if(windows_.begin(), windows_.end(),
        [id](const auto& window){
            return id == window->id();
        });

        if (window != windows_.end()) {
            return window->get();
        }

        return nullptr;
    }

    void WindowManager::remove_window_at(size_t index) {
        app().add_action([this, index](){
            windows_[index]->destroy();
            windows_.erase(windows_.begin() + index);
        });
    }


    void WindowManager::remove_window(uint64_t id) {
        auto it = std::find_if(
            windows_.begin(),
            windows_.end(),
            [id](const auto& win) {
                return win->id() == id;
            }
        );
        auto dist = std::distance(windows_.begin(), it);
        remove_window_at(dist);
    }
}
