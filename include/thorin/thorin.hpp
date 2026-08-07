
#pragma once

#include "thorin/windowmanager.hpp"
#include "thorin/randomgenerator.hpp"
#include "thorin/actionmanager.hpp"

namespace thorin {
    class Thorin {
        int exitCode_ = 0;
        uint64_t frame_ = 0;
        bool shouldExit_ = false;

        WindowManager windowManager_ = WindowManager(*this);
        static RandomGenerator randomGenerator_;
        ActionManager actionManager_;

        void init();
        void destroy();
        void update();
    public:
        Thorin(int argc, char** argv);
        int exec();
        void exit();

        static uint64_t random() {
            return randomGenerator_.random();
        }
        uint64_t frame() const;
        void add_action(
            std::move_only_function<void()> fn,
            uint64_t delay = 1
        );
        Window *add_window(std::unique_ptr<Window> window);
        Window *add_window();
    };
}
