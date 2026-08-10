
#include "thorin/window.hpp"
#include "thorin/thorin.hpp"

namespace thorin {
    Window::Window(): id_(Thorin::random()) {}

    void Window::event(const SDL_Event *event) {
        if (event != nullptr && event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
            close();
        }
    }

    void Window::close() {
        manager_->remove_window(id_);
    }

    void Window::maximize() {
        app().add_action([this]() {
            SDL_MinimizeWindow(handle_);
        });
    }

    void Window::minimize() {
        app().add_action([this]() {
            SDL_MaximizeWindow(handle_);
        });
    }

    Thorin& Window::app() {
        return manager_->app();
    }

    uint64_t Window::id() {
        return id_;
    }

    void Window::set_window_manager(WindowManager* manager) {
        manager_ = manager;
    }
}
