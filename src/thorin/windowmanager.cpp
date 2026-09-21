#include <imgui_impl_sdl3.h>
#include <SDL3/SDL.h>
#include <libassert/assert.hpp>

#include "thorin/windowmanager.hpp"
#include "thorin/thorin.hpp"


namespace thorin {
    WindowManager::WindowManager(Thorin &app) : app_(app) {}

    void WindowManager::init() {
        [[maybe_unused]] bool ok = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
        DEBUG_ASSERT(ok, "SDL_Init failed", SDL_GetError());
    }

    void WindowManager::destroy() {
        SDL_Quit();
    }

    void WindowManager::update() {
        SDL_Delay(20);
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            // Every window has its own ImGui context, so an event has to reach the context of its window.
            // Events that belong to no window (e.g. gamepad hot-plug) go to all of them.
            SDL_Window* handle = SDL_GetWindowFromEvent(&event);
            Window* target = nullptr;

            if (handle == nullptr) {
                for (auto &window: windows_) {
                    window->backend()->process_event(event);
                }
            } else {
                target = static_cast<Window*>(SDL_GetPointerProperty(SDL_GetWindowProperties(handle), "WRAPPER", nullptr));

                if (target != nullptr) {
                    target->backend()->process_event(event);
                }
            }

            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                DEBUG_ASSERT(target != nullptr, "close-requested event for untracked window");
                target->close();
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

    uint64_t WindowManager::add_window(std::unique_ptr<Window> window) {
        DEBUG_ASSERT(window != nullptr, "add_window called with a null window");

        const uint64_t id = window->id();
        window->set_window_manager(this);

        // The window is initialized and tracked on the next frame; until then the queued action owns it.
        app_.add_action([window = std::move(window), this] () mutable {
            window->init();
            windows_.push_back(std::move(window));
        });

        return id;
    }

    Window* WindowManager::get_window_at(const size_t index) {
        DEBUG_ASSERT(index < windows_.size(), "window index out of range", index, windows_.size());
        return windows_[index].get();
    }

    Window* WindowManager::get_window(uint64_t id) {
        auto it = std::find_if(windows_.begin(), windows_.end(),
        [id](const auto& window){
            return id == window->id();
        });

        if (it != windows_.end()) {
            return it->get();
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

            // The window may already be gone (closed by the user, or removed twice); that is not an error.
            if (it == windows_.end()) {
                return;
            }

            (*it)->destroy();
            windows_.erase(it);
        });
    }
}