
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
        Window* add_window(Window &&window);
        Window* get_window_at(size_t index);
        Window* get_window(uint64_t id);
        void remove_window_at(size_t index);
        void remove_window(uint64_t id);
    };
}
