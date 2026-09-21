#pragma once

#include <string>

#include "thorin/widget.hpp"

namespace thorin {
    class Text : public Widget {
    public:
        Text(const std::string& text = "", Widget* parent = nullptr);

        bool show() override;
    };
}
