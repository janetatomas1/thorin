#include <glbinding/gl/gl.h>
#include <globjects/globjects.h>

#include <SDL3/SDL.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <implot3d.h>
#include <libassert/assert.hpp>

#include "thorin/gl/glbackend.hpp"

#include <iostream>

#include "thorin/window.hpp"
#include "thorin/thorin.hpp"
#include "thorin/log.hpp"


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

        scale_ = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
        if (scale_ <= 0.0f) {
            // SDL reports 0 when it cannot query the display; a zero scale would collapse the whole UI.
            scale_ = 1.0f;
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

        // No vsync: the frame policy paces the loop (DisplayMatched for the display's rate). Vsync would
        // stack one refresh wait per window.
        SDL_GL_SetSwapInterval(0);
        SDL_SetWindowPosition(handle_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(handle_);

        // Also initializes glbinding for this context.
        globjects::init(
            [](const char* name) {
                return SDL_GL_GetProcAddress(name);
            }
        );

        IMGUI_CHECKVERSION();
        imguiContext_ = ImGui::CreateContext();
        // Per window like the ImGui context; make_current() switches all three.
        implotContext_ = ImPlot::CreateContext();
        implot3dContext_ = ImPlot3D::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        // ImGui keeps the pointer, not a copy; config_ outlives the context.
        io.IniFilename = config_.iniFile.empty() ? nullptr : config_.iniFile.c_str();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        apply_theme(window_->app().theme());

        [[maybe_unused]] bool sdlInitOk = ImGui_ImplSDL3_InitForOpenGL(handle_, gl_context);
        DEBUG_ASSERT(sdlInitOk, "ImGui_ImplSDL3_InitForOpenGL failed");

        [[maybe_unused]] bool gl3InitOk = ImGui_ImplOpenGL3_Init(glsl_version.c_str());
        DEBUG_ASSERT(gl3InitOk, "ImGui_ImplOpenGL3_Init failed", glsl_version);

        logger().info(
            "window {} '{}' created {}x{}, OpenGL {} on {}, scale {}",
            window_->id(),
            config_.title,
            config_.width,
            config_.height,
            reinterpret_cast<const char*>(glGetString(GL_VERSION)),
            reinterpret_cast<const char*>(glGetString(GL_RENDERER)),
            scale_
        );
    }

    void GLBackend::destroy() {
        DEBUG_ASSERT(handle_ != nullptr, "GLBackend::destroy called with no window handle");
        DEBUG_ASSERT(gl_context != nullptr, "GLBackend::destroy called with no gl context");
        DEBUG_ASSERT(imguiContext_ != nullptr, "GLBackend::destroy called with no imgui context");

        logger().info("window {} '{}' closed", window_->id(), config_.title);

        // The backends and the context are per window; shut them down while this window's contexts are current.
        make_current();
        // GL objects have to go while their context is current.
        backgroundTexture_.reset();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImPlot3D::DestroyContext(implot3dContext_);
        implot3dContext_ = nullptr;
        ImPlot::DestroyContext(implotContext_);
        implotContext_ = nullptr;
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
        // Yoga padding is the only inset: the root box covers the whole window from (0, 0), so ImGui's padding
        // would only push the content past the edge and bring up a scrollbar.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
        ImGui::Begin(
            config_.title.c_str(),
            nullptr,
            config_.windowFlags
        );
        ImGui::PopStyleVar();
        draw_background();

        rootWidget->layout().calculate_layout(config_.width, config_.height);
        rootWidget->render();

        ImGui::End();
        ImGui::Render();
        glViewport(0, 0, config_.width, config_.height);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(handle_);
    }

    void GLBackend::draw_background() {
        if (backgroundDirty_) {
            backgroundTexture_.reset();

            if (background_ != nullptr) {
                backgroundTexture_ = globjects::Texture::createDefault(GL_TEXTURE_2D);
                backgroundTexture_->image2D(
                    0,
                    GL_RGBA8,
                    background_->width(),
                    background_->height(),
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    background_->pixels().data()
                );
            }
            backgroundDirty_ = false;
        }

        // The root window's own draw list: above its WindowBg, below every widget.
        const ImVec2 pos = ImGui::GetWindowPos();
        const ImVec2 size = ImGui::GetWindowSize();
        const ImVec2 max(pos.x + size.x, pos.y + size.y);

        if (backgroundTexture_ != nullptr) {
            ImGui::GetWindowDrawList()->AddImage(
                static_cast<ImTextureID>(backgroundTexture_->id()),
                pos,
                max,
                ImVec2(0, 0),
                ImVec2(1, 1),
                ImGui::GetColorU32(ImVec4(1, 1, 1, backgroundOpacity_))
            );
        }

        if (backgroundPainter_) {
            backgroundPainter_(*ImGui::GetWindowDrawList(), pos, max);
        }
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
        // Each window registers its own context in globjects::init.
        globjects::setCurrentContext();
        ImGui::SetCurrentContext(imguiContext_);
        ImPlot::SetCurrentContext(implotContext_);
        ImPlot3D::SetCurrentContext(implot3dContext_);
    }
}