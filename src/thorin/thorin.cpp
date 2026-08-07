
#include "thorin/thorin.hpp"

#include "thorin/glwindow.hpp"

namespace thorin {
    void Thorin::init() {}

    void Thorin::update() {
        actionManager_.dispatch();
        windowManager_.update();
    }

    Thorin::Thorin(int argc, char** argv) {
        windowManager_.init();
    }

    void Thorin::destroy() {
        windowManager_.destroy();
    }

    int Thorin::exec() {
        init();

        while(!shouldExit_) {
            update();
            frame_ += 1;
            shouldExit_ |= windowManager_.count() == 0;
        }

        destroy();
        return exitCode_;
    }

    void Thorin::exit() {}

    uint64_t Thorin::frame() const {
        return frame_;
    }

    void Thorin::add_action(std::move_only_function<void()> fn, uint64_t delay) {
        actionManager_.add_action(std::move(fn), delay);
    }

    Window* Thorin::add_window(std::unique_ptr<Window> window) {
        return windowManager_.add_window(std::move(window));
    }

    Window* Thorin::add_window() {
        return windowManager_.add_window(std::make_unique<GLWindow>());
    }
}
