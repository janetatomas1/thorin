
#pragma once

#include <vector>
#include <memory>

#include "thorin/window.hpp"

namespace thorin {
    class WindowManager {
        std::vector<std::unique_ptr<Window>> windows_;

    public:
        void init();
        void update();
        void destroy();
        [[nodiscard]] size_t count() const;
    };
}
