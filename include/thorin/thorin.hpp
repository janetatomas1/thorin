
#pragma once

#include "thorin/windowmanager.hpp"
#include "thorin/randomgenerator.hpp"

namespace thorin {
    class Thorin {
        int exitCode_ = 0;
        uint64_t frame_ = 0;
        bool shouldExit_ = false;

        WindowManager windowManager_;
        RandomGenerator randomGenerator_;

        void init();
        void destroy();
        void update();
    public:
        Thorin() = default;
        int run();
        void exit();

        uint64_t random();
        uint64_t frame() const;
    };
}
