
#include "thorin/thorin.hpp"

namespace thorin {
    void Thorin::init() {
        windowManager_.init();
    }

    void Thorin::update() {}

    void Thorin::destroy() {
        windowManager_.destroy();
    }

    int Thorin::run() {
        init();

        while(!shouldExit_ && windowManager_.count() > 0) {
            update();

            frame_ += 1;
        }

        destroy();
        return exitCode_;
    }

    uint64_t Thorin::frame() const {
        return frame_;
    }
}
