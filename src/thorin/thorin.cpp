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

    Thorin::Thorin([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
        DEBUG_ASSERT(argc >= 0, "argc is negative", argc);
        DEBUG_ASSERT(argc == 0 || argv != nullptr, "argv is null but argc is nonzero", argc);
        DEBUG_ASSERT(current_ == nullptr, "only one Thorin may exist at a time");
        current_ = this;

        windowManager_.init();
    }

    Thorin::~Thorin() {
        current_ = nullptr;
    }

    Thorin& Thorin::current() {
        DEBUG_ASSERT(current_ != nullptr, "Thorin::current called with no Thorin instance");
        return *current_;
    }

    bool Thorin::has_current() {
        return current_ != nullptr;
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
        add_action([this]() {
            shouldExit_ = true;
        });
    }

    uint64_t Thorin::frame() const {
        return frame_;
    }

    const FrameConfig& Thorin::frame_config() const {
        return windowManager_.frame_config();
    }

    void Thorin::set_frame_config(const FrameConfig &config) {
        windowManager_.set_frame_config(config);
    }

    void Thorin::request_redraw(int frames) {
        windowManager_.request_redraw(frames);
    }

    bool Thorin::actions_pending() const {
        return actionManager_.pending();
    }

    void Thorin::add_action(action &&fn, uint64_t delay) {
        DEBUG_ASSERT(fn != nullptr, "add_action called with an empty/null action");
        actionManager_.add_action(std::move(fn), delay);
        windowManager_.wake();
    }

    uint64_t Thorin::add_window(std::unique_ptr<Window> window) {
        return windowManager_.add_window(std::move(window));
    }

    uint64_t Thorin::add_window(const WindowConfig &config) {
        return windowManager_.add_window(std::make_unique<Window>(config));
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
