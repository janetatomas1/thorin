#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#include <SDL3/SDL.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <libassert/assert.hpp>

#include "thorin/glbackend.hpp"

#include <iostream>

#include "thorin/window.hpp"


using namespace gl;

namespace thorin {
    GLBackend::GLBackend(
        const WindowConfig &config
    ) : GPUBackend(config), gl_context(nullptr){}

    void GLBackend::init() {
        DEBUG_ASSERT(window_ != nullptr, "GLBackend::init called before set_window");

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
        if (main_scale <= 0.0f) {
            // SDL reports 0 when it cannot query the display; a zero scale would collapse the whole UI.
            main_scale = 1.0f;
        }

        handle_ = SDL_CreateWindow(
            config_.title.c_str(),
            config_.width,
            config_.height,
            config_.sdlFlags
        );
        DEBUG_ASSERT(handle_ != nullptr, "SDL_CreateWindow failed", SDL_GetError());

        gl_context = SDL_GL_CreateContext(handle_);
        DEBUG_ASSERT(gl_context != nullptr, "SDL_GL_CreateContext failed", SDL_GetError());

        if (config_.windowState == WindowState::MAXIMIZED) {
            maximize();
        } else if (config_.windowState == WindowState::MINIMIZED) {
            minimize();
        }

        SDL_SetPointerProperty(SDL_GetWindowProperties(handle_), "WRAPPER", window_);

        [[maybe_unused]] bool currentOk = SDL_GL_MakeCurrent(handle_, gl_context);
        DEBUG_ASSERT(currentOk, "SDL_GL_MakeCurrent failed", SDL_GetError());

        SDL_GL_SetSwapInterval(1);
        SDL_SetWindowPosition(handle_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(handle_);

        glbinding::initialize(
            [](const char* name) {
                return SDL_GL_GetProcAddress(name);
            }
        );

        IMGUI_CHECKVERSION();
        imguiContext_ = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);
        style.FontScaleDpi = main_scale;

        [[maybe_unused]] bool sdlInitOk = ImGui_ImplSDL3_InitForOpenGL(handle_, gl_context);
        DEBUG_ASSERT(sdlInitOk, "ImGui_ImplSDL3_InitForOpenGL failed");

        [[maybe_unused]] bool gl3InitOk = ImGui_ImplOpenGL3_Init(glsl_version.c_str());
        DEBUG_ASSERT(gl3InitOk, "ImGui_ImplOpenGL3_Init failed", glsl_version);
    }

    void GLBackend::destroy() {
        DEBUG_ASSERT(handle_ != nullptr, "GLBackend::destroy called with no window handle");
        DEBUG_ASSERT(gl_context != nullptr, "GLBackend::destroy called with no gl context");
        DEBUG_ASSERT(imguiContext_ != nullptr, "GLBackend::destroy called with no imgui context");

        // The backends and the context are per window; shut them down while this window's contexts are current.
        make_current();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext(imguiContext_);
        imguiContext_ = nullptr;

        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(handle_);
    }

    void GLBackend::update(Widget *rootWidget) {
        DEBUG_ASSERT(handle_ != nullptr, "GLBackend::update called with no window handle");
        DEBUG_ASSERT(rootWidget != nullptr, "GLBackend::update called with null root widget");

        make_current();
        SDL_ShowWindow(handle_);
        SDL_GetWindowSize(handle_, &config_.width, &config_.height);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(config_.width, config_.height));
        ImGui::Begin(
            config_.title.c_str(),
            nullptr,
            config_.windowFlags
        );

        rootWidget->layout().calculate_layout(config_.width, config_.height);
        rootWidget->render();

        ImGui::End();
        ImGui::Render();
        glViewport(0, 0, config_.width, config_.height);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(handle_);
    }

    void GLBackend::process_event(const SDL_Event& event) {
        DEBUG_ASSERT(imguiContext_ != nullptr, "process_event called with no imgui context");

        ImGui::SetCurrentContext(imguiContext_);
        ImGui_ImplSDL3_ProcessEvent(&event);
    }

    void GLBackend::make_current() {
        DEBUG_ASSERT(handle_ != nullptr, "make_current called with no window handle");
        DEBUG_ASSERT(gl_context != nullptr, "make_current called with no gl context");
        DEBUG_ASSERT(imguiContext_ != nullptr, "make_current called with no imgui context");

        SDL_GL_MakeCurrent(handle_, gl_context);
        ImGui::SetCurrentContext(imguiContext_);
    }
}