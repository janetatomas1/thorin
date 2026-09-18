#include <imgui_impl_sdl3.h>
#include <SDL3/SDL.h>
#include <libassert/assert.hpp>

#include "thorin/windowmanager.hpp"
#include "thorin/thorin.hpp"


namespace thorin {
    WindowManager::WindowManager(Thorin &app) : app_(app) {}

    void WindowManager::init() {
        bool ok = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
        DEBUG_ASSERT(ok, "SDL_Init failed", SDL_GetError());
    }

    void WindowManager::destroy() {
        SDL_Quit();
    }

    void WindowManager::update() {
        SDL_Delay(20);
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                auto id = static_cast<uint64_t>(
                    SDL_GetNumberProperty(
                        SDL_GetWindowProperties(SDL_GetWindowFromEvent(&event)),
                        "WRAPPER",
                        0
                    )
                );
                auto window = get_window(id);
                DEBUG_ASSERT(window != nullptr, "close-requested event for untracked window id", id);
                window->close();
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

    Window* WindowManager::add_window(Window &&window) {
        window.set_window_manager(this);
        app_.add_action(std::move([window = std::move(window), this] () mutable {
            auto owned = std::make_unique<Window>(std::move(window));
            owned->init();
            windows_.push_back(std::move(owned));
        }));

        return nullptr;
    }

    Window* WindowManager::get_window_at(const size_t index) {
        DEBUG_ASSERT(index < windows_.size(), "window index out of range", index, windows_.size());
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
        DEBUG_ASSERT(index < windows_.size(), "window index out of range", index, windows_.size());
        remove_window(windows_[index]->id());
    }

    void WindowManager::remove_window(uint64_t id) {
        app().add_action([this, id](){
            auto it = std::find_if(
                windows_.begin(),
                windows_.end(),
                [id](const auto& win) {
                    return win->id() == id;
                }
            );

            DEBUG_ASSERT(it != windows_.end(), "remove_window: no window with this id", id);

            (*it)->destroy();
            windows_.erase(it);
        });
    }
}