
#pragma once

class GLFWwindow;

namespace thorin {
    class Window {
        GLFWwindow *handle_ = nullptr;

    public:
        Window() = default;
        virtual ~Window() = default;
        virtual void init() = 0;
        virtual void destroy() = 0;
        virtual void update() = 0;
    };
}
