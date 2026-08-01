
#include <glbinding/gl/gl.h>

#include <GLFW/glfw3.h>

#include "thorin/glwindow.hpp"

using namespace gl;

namespace thorin {
    void GLWindow::init() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        handle_ = glfwCreateWindow(
            config_.width,
            config_.height,
            config_.title.c_str(),
            nullptr,
            nullptr
        );
    }

    void GLWindow::destroy() {
        glfwSetWindowShouldClose(handle_, GLFW_TRUE);
    }

    void GLWindow::update() {

    }
}
