
#pragma once

#include "thorin/windowmanager.hpp"
#include "thorin/randomgenerator.hpp"
#include "thorin/actionmanager.hpp"

namespace thorin {
    class Thorin {
        int exitCode_ = 0;
        uint64_t frame_ = 0;
        bool shouldExit_ = false;

        WindowManager windowManager_;
        static RandomGenerator randomGenerator_;
        ActionManager actionManager_;

        void init();
        void destroy();
        void update();
    public:
        Thorin() = default;
        int run();
        void exit();

        static uint64_t random() {
            return randomGenerator_.random();
        }
        uint64_t frame() const;
    };
}
