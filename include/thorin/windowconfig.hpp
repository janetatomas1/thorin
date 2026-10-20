
#pragma once

#include <string>
#include <imgui.h>
#include <SDL3/SDL.h>

namespace thorin {
    // The GPU API a window draws with.
    enum class Backend {
        OpenGL = 0
    };

    // How a window is first shown.
    enum class WindowState {
        MAXIMIZED = 0,
        MINIMIZED = 1,
        NORMAL = 2,
    };

    // Settings a window is created with; passed to Thorin::add_window(). Size and title can change later
    // through Window.
    struct WindowConfig {
        int width = 1920,
        height = 1080;
        std::string title = "Thorin";
        // NoBringToFrontOnFocus: clicking the root must not cover floating windows (dock pages).
        // NoDocking: pages can't be docked into the root itself, only into dock spaces.
        // Flags of the ImGui window that holds the root widget.
        int windowFlags = ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoDocking;
        SDL_WindowFlags sdlFlags = SDL_WINDOW_OPENGL
        | SDL_WINDOW_RESIZABLE
        | SDL_WINDOW_HIGH_PIXEL_DENSITY;
        WindowState windowState = WindowState::MAXIMIZED;
        // ImGui settings file (docking layout, table state) for this window's context; empty disables it.
        // Each window has its own context, so windows must not share a file.
        std::string iniFile;
    };
}
