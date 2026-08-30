
#pragma once

#include <cstdint>
#include <string>
#include <concepts>

#include "thorin/layout.hpp"

namespace thorin {
    class Window;
    class Thorin;

    class Widget {
        uint64_t id_ = 0;
        std::string title_;
        std::string titleID_;

        Window* window_ = nullptr;
        Widget* parent_ = nullptr;

        Layout layout_;
    public:
        Widget(const std::string &title = "", Widget *parent = nullptr);
        virtual ~Widget() = default;
        [[nodiscard]] uint64_t id() const;
        [[nodiscard]] std::string title() const;
        [[nodiscard]] std::string title_id() const;
        bool render();
        virtual bool show();
        Window* window();
        void set_window(Window* window);
        Widget* parent();
        void set_parent(Widget* parent);
        Thorin& app();
        Layout& layout();
    };

    template <class W>
    concept WidgetConcept = std::derived_from<W, Widget> || std::is_same_v<W, Widget>;
}
