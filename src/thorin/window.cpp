
#include "thorin/window.hpp"
#include "thorin/thorin.hpp"
#include "thorin/glbackend.hpp"

namespace thorin {
    Window::Window(): id_(Thorin::random()) {}

    void Window::init() {
        backend_ = std::make_unique<GLBackend>(this);
        backend_->init();
    }

    void Window::destroy() {
        backend_->destroy();
    }

    void Window::update() {
        backend_->update();
    }

    void Window::close() {
        manager_->remove_window_at(0);
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

    uint64_t Window::id() const {
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
