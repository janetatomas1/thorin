#include <libassert/assert.hpp>

#include "thorin/thorin.hpp"

namespace thorin {
    RandomGenerator Thorin::randomGenerator_{};

    void Thorin::init() {}

    void Thorin::update() {
        actionManager_.dispatch();
        windowManager_.update();

        frame_ += 1;
        shouldExit_ |= windowManager_.count() == 0;
    }

    Thorin::Thorin(int argc, char** argv) {
        DEBUG_ASSERT(argc >= 0, "argc is negative", argc);
        DEBUG_ASSERT(argc == 0 || argv != nullptr, "argv is null but argc is nonzero", argc);

        windowManager_.init();
    }

    void Thorin::destroy() {
        windowManager_.destroy();
    }

    int Thorin::exec() {
        init();

        while(!shouldExit_) {
            update();
        }

        destroy();
        return exitCode_;
    }

    void Thorin::exit() {
        actionManager_.add_action([this]() {
            shouldExit_ = true;
        });
    }

    uint64_t Thorin::frame() const {
        return frame_;
    }

    void Thorin::add_action(action &&fn, uint64_t delay) {
        DEBUG_ASSERT(fn != nullptr, "add_action called with an empty/null action");
        actionManager_.add_action(std::move(fn), delay);
    }

    Window* Thorin::add_window(Window &&window) {
        return windowManager_.add_window(std::move(window));
    }

    Window* Thorin::add_window(const WindowConfig &config) {
        return windowManager_.add_window(Window(config));
    }

    Window* Thorin::get_window_at(size_t index) {
        return windowManager_.get_window_at(index);
    }

    Window* Thorin::get_window(uint64_t id) {
        return windowManager_.get_window(id);
    }

    void Thorin::remove_window_at(size_t index) {
        windowManager_.remove_window_at(index);
    }

    void Thorin::remove_window(uint64_t id) {
        windowManager_.remove_window(id);
    }
}
