
#pragma once

#include <string>
#include <SDL3/SDL.h>

#include "thorin/gpubackend.hpp"

struct ImGuiContext;
struct ImPlotContext;
struct ImPlot3DContext;

namespace thorin {
    class GLBackend: public GPUBackend {
        SDL_GLContext gl_context;
        ImGuiContext* imguiContext_ = nullptr;
        ImPlotContext* implotContext_ = nullptr;
        ImPlot3DContext* implot3dContext_ = nullptr;
        const std::string glsl_version = "#version 330 core";

    public:
        GLBackend(const WindowConfig &config = {});
        void init() override;
        void destroy() override;
        void update(Widget *rootWidget) override;
        void process_event(const SDL_Event& event) override;
        void make_current() override;
    };
}
