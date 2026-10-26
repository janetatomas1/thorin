#pragma once

#include <string>

#include <function2/function2.hpp>

#include "thorin/sdlgpu/sdlgpurenderwidget.hpp"

namespace thorin {
    // A button showing texture(), drawn at this widget's Yoga box (frame padding included).
    // It has no intrinsic size. title is only its ImGui id; it is not shown.
    class SDLGPUImageButton : public SDLGPURenderWidget {
        fu2::unique_function<void()> callback_;

    public:
        SDLGPUImageButton(const std::string& title, Widget* parent = nullptr);
        SDLGPUImageButton(const std::string& title, fu2::unique_function<void()> callback, Widget* parent = nullptr);

        void set_callback(fu2::unique_function<void()> callback);
        bool show() final;
    };
}
