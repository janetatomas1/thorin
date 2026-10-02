
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
        height = 1080;
        std::string title = "Thorin";
        // NoBringToFrontOnFocus: clicking the root must not cover floating windows (dock pages).
        // NoDocking: pages can't be docked into the root itself, only into dock spaces.
        int windowFlags = ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoDocking;
        SDL_WindowFlags sdlFlags = SDL_WINDOW_OPENGL
        | SDL_WINDOW_RESIZABLE
        | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        WindowState windowState = WindowState::MAXIMIZED;
    };
}
