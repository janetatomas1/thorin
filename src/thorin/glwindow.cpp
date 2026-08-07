
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#include <SDL3/SDL.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include "thorin/glwindow.hpp"

using namespace gl;

namespace thorin {
    void GLWindow::init() {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

        window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        handle_ = SDL_CreateWindow(
            "Dear ImGui SDL3+OpenGL3 example",
            config_.width,
            config_.height,
            window_flags
        );
        gl_context = SDL_GL_CreateContext(handle_);
        SDL_SetPointerProperty(SDL_GetWindowProperties(handle_), "WRAPPER", this);

        SDL_GL_MakeCurrent(handle_, gl_context);
        SDL_GL_SetSwapInterval(1);
        SDL_SetWindowPosition(handle_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(handle_);

        glbinding::initialize(
            [](const char* name) {
                return SDL_GL_GetProcAddress(name);
            }
        );

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);
        style.FontScaleDpi = main_scale;

        ImGui_ImplSDL3_InitForOpenGL(handle_, gl_context);
        ImGui_ImplOpenGL3_Init(glsl_version.c_str());
    }

    void GLWindow::destroy() {
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(handle_);
    }

    void GLWindow::update() {
        SDL_ShowWindow(handle_);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Render();
        glViewport(0, 0, config_.width, config_.height);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(handle_);
    }

    void GLWindow::make_current() {
        SDL_GL_MakeCurrent(handle_, gl_context);
    }
}
