
#pragma once

class GLFWwindow;

namespace thorin {
    struct WindowConfig {
        int width = 1920, height = 1080;
        std::string title;
        bool maximized = false;
    };

    class Window {
    protected:
        GLFWwindow *handle_ = nullptr;
        WindowConfig config_;

    public:
        Window(const WindowConfig &config): config_(config) {};
        virtual void init() = 0;
        virtual void destroy() = 0;
        virtual void update() = 0;
    };
}
