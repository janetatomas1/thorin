#include <libassert/assert.hpp>

#include "thorin/window.hpp"
#include "thorin/thorin.hpp"
#include "thorin/glbackend.hpp"

namespace thorin {
    Window::Window(const WindowConfig &config, std::unique_ptr<Widget> widget)
    : id_(Thorin::random()), rootWidget_(widget == nullptr ? std::make_unique<Widget>() : std::move(widget)) {
        backend_ = std::make_unique<GLBackend>(config);
    }

    void Window::init() {
        DEBUG_ASSERT(backend_ != nullptr, "Window::init called with no backend");
        backend_->set_window(this);
        backend_->init();
    }

    void Window::destroy() {
        DEBUG_ASSERT(backend_ != nullptr, "Window::destroy called with no backend");
        backend_->destroy();
    }

    void Window::update() {
        DEBUG_ASSERT(backend_ != nullptr, "Window::update called with no backend");
        DEBUG_ASSERT(rootWidget_ != nullptr, "Window::update called with no root widget");
        backend_->update(rootWidget_.get());
    }

    void Window::close() {
        DEBUG_ASSERT(manager_ != nullptr, "Window::close called before set_window_manager");
        manager_->remove_window(id_);
    }

    Thorin& Window::app() {
        DEBUG_ASSERT(manager_ != nullptr, "Window::app called before set_window_manager");
        return manager_->app();
    }

    uint64_t Window::id() const {
        return id_;
    }

    void Window::set_window_manager(WindowManager* manager) {
        DEBUG_ASSERT(manager != nullptr, "set_window_manager called with null manager");
        manager_ = manager;
    }

    void Window::set_root_widget(std::unique_ptr<Widget> widget) {
        DEBUG_ASSERT(widget != nullptr, "set_root_widget called with null widget");

        app().add_action([this, widget = std::move(widget)]() mutable  {
            if (rootWidget_ != nullptr) {
                rootWidget_->destroy();
            }

            rootWidget_ = std::move(widget);
            rootWidget_->init();
        });
    }

    Widget* Window::root_widget() {
        return rootWidget_.get();
    }

    GPUBackend* Window::backend() {
        return backend_.get();
    }

    void Window::maximize() {
        DEBUG_ASSERT(backend_ != nullptr, "Window::maximize called with no backend");
        backend_->maximize();
    }

    void Window::minimize() {
        DEBUG_ASSERT(backend_ != nullptr, "Window::minimize called with no backend");
        backend_->minimize();
    }

    void Window::set_size(int width, int height) {
        DEBUG_ASSERT(backend_ != nullptr, "Window::set_size called with no backend");
        backend_->set_size(width, height);
    }

    void Window::set_title(const std::string &title) {
        DEBUG_ASSERT(backend_ != nullptr, "Window::set_title called with no backend");
        backend_->set_title(title);
    }

    const std::string& Window::title() const {
        DEBUG_ASSERT(backend_ != nullptr, "Window::title called with no backend");
        return backend_->title();
    }

    int Window::height() const {
        DEBUG_ASSERT(backend_ != nullptr, "Window::height called with no backend");
        return backend_->height();
    }

    int Window::width() const {
        DEBUG_ASSERT(backend_ != nullptr, "Window::width called with no backend");
        return backend_->width();
    }
}
