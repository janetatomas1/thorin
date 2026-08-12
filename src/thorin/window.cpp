
#include "thorin/window.hpp"
#include "thorin/thorin.hpp"

namespace thorin {
    Window::Window(): id_(Thorin::random()) {}

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

    void Window::set_main_widget(std::unique_ptr<Widget> widget) {
        mainWidget_ = std::move(widget);
    }

    Widget* Window::main_widget() {
        return mainWidget_.get();
    }
}
