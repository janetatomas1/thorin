
#pragma once

#include <string>
#include <imgui.h>
#include <SDL3/SDL.h>

namespace thorin {
    enum class Backend {
        OpenGL = 0
    };

    enum class WindowState {
        MAXIMIZED = 0,
        MINIMIZED = 1,
        NORMAL = 2,
    };

    struct WindowConfig {
        int width = 1920,
        height = 80;
        std::string title = "Thorin";
        int windowFlags = ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize;
        SDL_InitFlags sdlFlags = SDL_WINDOW_OPENGL
        | SDL_WINDOW_RESIZABLE
        | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        WindowState windowState = WindowState::MAXIMIZED;
    };
}
