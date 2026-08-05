
#pragma once

#include <vector>
#include <memory>

#include "thorin/window.hpp"


namespace thorin {
    class Thorin;

    class WindowManager {
        std::vector<std::unique_ptr<Window>> windows_;
        Thorin &app_;

    public:
        WindowManager(Thorin &app);
        void init();
        void update();
        void destroy();
        [[nodiscard]] size_t count() const;
        Thorin &app();
        Window *add_window(std::unique_ptr<Window> window);
    };
}
