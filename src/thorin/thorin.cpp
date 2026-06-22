
#include "thorin/thorin.hpp"

namespace thorin {
    void Thorin::init() {}

    void Thorin::update() {}

    void Thorin::destroy() {}

    int Thorin::run() {
        init();

        while(!shouldExit_) {
            update();
        }

        destroy();
        return exitCode_;
    }
}
