
#pragma once

#include <string>

#include "thorin/widget.hpp"

namespace thorin {
    class Label : public Widget {
    public:
        Label(const std::string& text = "", Widget* parent = nullptr);

        bool show() override;
    };
}
