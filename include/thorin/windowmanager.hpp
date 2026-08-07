
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
        Window* add_window(std::unique_ptr<Window> window);
        Window* get_window(size_t index);
        Window* get_window_by_id(uint64_t id);
        void remove_window(uint64_t id);
    };
}
