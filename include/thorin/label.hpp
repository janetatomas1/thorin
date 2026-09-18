
#pragma once

#include <string>

#include "thorin/widget.hpp"

namespace thorin {
    class Label : public Widget {
        bool wrapped_ = false;
        bool separator_ = false;

    public:
        Label(const std::string& text = "", Widget* parent = nullptr);

        bool show() override;

        [[nodiscard]] bool wrapped() const;
        Label& set_wrapped(bool wrapped);

        [[nodiscard]] bool separator() const;
        Label& set_separator(bool separator);
    };
}
