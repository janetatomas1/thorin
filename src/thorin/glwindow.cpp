
#include <glbinding/gl/gl.h>
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
        window = SDL_CreateWindow("Dear ImGui SDL3+OpenGL3 example", (int)(1280 * main_scale), (int)(800 * main_scale), window_flags);
        gl_context = SDL_GL_CreateContext(window);

        SDL_GL_MakeCurrent(window, gl_context);
        SDL_GL_SetSwapInterval(1); // Enable vsync
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(window);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup scaling
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

        const char *glsl_version = "#version 330 core";
        ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void GLWindow::destroy() {
    }

    void GLWindow::update() {
        SDL_Event event;
        SDL_ShowWindow(window);
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
        }

        // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }
}
