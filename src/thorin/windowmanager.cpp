
#include "thorin/windowmanager.hpp"

#include <GLFW/glfw3.h>

namespace thorin {
    void WindowManager::init() {
        if(glfwInit() == GLFW_FALSE) {

        }
    }

    void WindowManager::destroy() {
        glfwTerminate();
    }

    void WindowManager::update() {
        for (auto &window: windows_) {
            window->update();
        }
    }

    size_t WindowManager::count() const {
        return windows_.size();
    }
}
