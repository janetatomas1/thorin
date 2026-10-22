#include <glbinding/gl/gl.h>
#include <globjects/globjects.h>

#include <SDL3/SDL.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
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

        create_window(SDL_WINDOW_OPENGL);

        gl_context = SDL_GL_CreateContext(handle_);
        DEBUG_ASSERT(gl_context != nullptr, "SDL_GL_CreateContext failed", SDL_GetError());

        [[maybe_unused]] bool currentOk = SDL_GL_MakeCurrent(handle_, gl_context);
        DEBUG_ASSERT(currentOk, "SDL_GL_MakeCurrent failed", SDL_GetError());

        // No vsync: the frame policy paces the loop (DisplayMatched for the display's rate). Vsync would
        // stack one refresh wait per window.
        SDL_GL_SetSwapInterval(0);

        // Also initializes glbinding for this context.
        globjects::init(
            [](const char* name) {
                return SDL_GL_GetProcAddress(name);
            }
        );

        create_contexts();

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

        logger().info("window {} '{}' closed", window_->id(), config_.title);

        // The backends and the context are per window; shut them down while this window's contexts are current.
        make_current();
        // GL objects have to go while their context is current.
        backgroundImage_.reset();
        ImGui_ImplOpenGL3_Shutdown();
        destroy_contexts();

        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(handle_);
    }

    void GLBackend::update(Widget *rootWidget) {
        make_current();

        ImGui_ImplOpenGL3_NewFrame();
        build_frame(rootWidget);

        glViewport(0, 0, config_.width, config_.height);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(handle_);
    }

    ImTextureID GLBackend::upload_background() {
        backgroundImage_.reset();

        if (background_ == nullptr) {
            return ImTextureID_Invalid;
        }
        backgroundImage_ = globjects::Texture::createDefault(GL_TEXTURE_2D);
        backgroundImage_->image2D(
            0,
            GL_RGBA8,
            background_->width(),
            background_->height(),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            background_->pixels().data()
        );
        return static_cast<ImTextureID>(backgroundImage_->id());
    }

    void GLBackend::make_current() {
        DEBUG_ASSERT(handle_ != nullptr, "make_current called with no window handle");
        DEBUG_ASSERT(gl_context != nullptr, "make_current called with no gl context");

        SDL_GL_MakeCurrent(handle_, gl_context);
        // Each window registers its own context in globjects::init.
        globjects::setCurrentContext();
        make_contexts_current();
    }
}
