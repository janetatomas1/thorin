
#pragma once

#include "context.hpp"

namespace thorin {
    class Thorin {
        int exitCode_ = 0;
        bool shouldExit_ = false;

        void init();
        void destroy();
        void update();
    public:
        Thorin() = default;
        int run();
        void exit();
    };
}
