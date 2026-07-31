
#pragma once

#include "thorin/windowmanager.hpp"

namespace thorin {
    class Thorin {
        int exitCode_ = 0;
        bool shouldExit_ = false;
        WindowManager windowManager_;

        void init();
        void destroy();
        void update();
    public:
        Thorin() = default;
        int run();
        void exit();
    };
}
