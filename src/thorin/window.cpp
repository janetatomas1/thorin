
#include "thorin/window.hpp"
#include "thorin/thorin.hpp"
#include "thorin/glbackend.hpp"

#include <iostream>

namespace thorin {
    Window::Window(const WindowConfig &config): id_(Thorin::random()) {
        backend_ = std::make_unique<GLBackend>(config);
    }

    void Window::init() {
        backend_->set_window(this);
        backend_->init();
    }

    void Window::destroy() {
        backend_->destroy();
    }

    void Window::update() {
        backend_->update(rootWidget_.get());
    }

    void Window::close() {
        manager_->remove_window(id_);
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

    void Window::set_root_widget(std::unique_ptr<Widget> widget) {
        rootWidget_ = std::move(widget);
    }

    Widget* Window::root_widget() {
        return rootWidget_.get();
    }

    GPUBackend* Window::backend() {
        return backend_.get();
    }

    void Window::maximize() {
        backend_->maximize();
    }

    void Window::minimize() {
        backend_->minimize();
    }

    void Window::set_size(int width, int height) {
        backend_->set_size(width, height);
    }

    void Window::set_title(const std::string &title) {
        backend_->set_title(title);
    }

    const std::string& Window::title() const {
        return backend_->title();
    }

    int Window::height() const {
        return backend_->height();
    }

    int Window::width() const {
        return backend_->width();
    }
}
