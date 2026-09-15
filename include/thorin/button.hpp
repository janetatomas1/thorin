
#pragma once

#include <function2/function2.hpp>

#include "thorin/widget.hpp"

namespace thorin {
    class Button: public Widget {
        fu2::unique_function<void()> callback_;

    public:
        Button(
            const std::string &title,
            Widget *parent = nullptr
        );
        Button(
            const std::string &title,
            fu2::unique_function<void()> callback,
            Widget *parent = nullptr
        );
        bool show() override;
    };
}
