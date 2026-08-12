
#pragma once

#include <cstdint>
#include <string>

namespace thorin {
    class Widget {
        uint64_t id_ = 0;
        std::string title_;
        std::string titleID_;

    public:
        Widget(const std::string &title = "");
        virtual ~Widget() = default;
        [[nodiscard]] uint64_t id() const;
        [[nodiscard]] std::string title() const;
        [[nodiscard]] std::string title_id() const;
        bool render();
        bool show();
    };
}
