
#include "thorin/gpubackend.hpp"

#include "thorin/thorin.hpp"
#include "thorin/window.hpp"

namespace thorin {
    GPUBackend::GPUBackend(
        Window *window,
        const WindowConfig &config
    ): window_(window), config_(config) {}

    void GPUBackend::maximize() {
        window_->app().add_action([this]() {
            SDL_MaximizeWindow(handle_);
        });
    }

    void GPUBackend::minimize() {
        window_->app().add_action([this]() {
            SDL_MinimizeWindow(handle_);
        });
    }

    void GPUBackend::set_size(int width, int height) {
        window_->app().add_action([this, width, height]() {
            if(SDL_SetWindowSize(handle_, width, height)) {
                config_.width = width;
                config_.height = height;
            }
        });
    }

    void GPUBackend::set_title(const std::string& title) {
        window_->app().add_action([this, title]() {
            config_.title = title;
        });
    }

    const std::string& GPUBackend::title() const {
        return config_.title;
    }

    int GPUBackend::height() const {
        return config_.height;
    }

    int GPUBackend::width() const {
        return config_.width;
    }
}
