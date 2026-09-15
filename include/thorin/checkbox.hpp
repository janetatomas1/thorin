
#pragma once

#include <functional>
#include <string>

#include "thorin/widget.hpp"

namespace thorin {
    class Checkbox : public Widget {
        std::string label_;

    public:
        bool value_ = false;
        std::function<void(bool)> onChange_;

        Checkbox(
            std::string label,
            std::function<void(bool)> on_change = nullptr,
            Widget* parent = nullptr
        );

        bool show() override;
        [[nodiscard]] bool value() const;
    };
}
