
#include <implot.h>
#include <implot3d.h>

#include "thorin/gpubackend.hpp"

#include "thorin/thorin.hpp"
#include "thorin/window.hpp"

namespace thorin {
    GPUBackend::GPUBackend(
        const WindowConfig &config
    ): window_(nullptr), config_(config) {}

    void GPUBackend::set_window(Window* window) {
        window_ = window;
    }

    void GPUBackend::apply_theme(const Theme &theme) {
        ImPlot::GetStyle() = theme.plot;
        ImPlot3D::GetStyle() = theme.plot3d;
        // Always from the unscaled theme: scaling an already scaled style would compound.
        ImGuiStyle& style = ImGui::GetStyle();
        style = theme.style;
        style.ScaleAllSizes(scale_);
        style.FontScaleDpi = scale_;

        if (theme.font != font_) {
            ImFontAtlas* atlas = ImGui::GetIO().Fonts;

            if (imFont_ != nullptr) {
                atlas->RemoveFont(imFont_);
                imFont_ = nullptr;
            }

            if (theme.font != nullptr) {
                ImFontConfig config;
                // Borrowed: the Font outlives this window, the atlas must not free its bytes.
                config.FontDataOwnedByAtlas = false;
                // ImGui never writes to the data; the parameter just isn't const.
                imFont_ = atlas->AddFontFromMemoryTTF(
                    const_cast<std::byte*>(theme.font->data().data()),
                    static_cast<int>(theme.font->data().size()),
                    0.0f,
                    &config
                );
            }
            font_ = theme.font;
            ImGui::GetIO().FontDefault = imFont_;
        }
    }

    const Image* GPUBackend::background() const {
        return background_;
    }

    void GPUBackend::set_background(const Image* image, float opacity) {
        // Only a new image needs a new texture; opacity is applied when drawing.
        backgroundDirty_ |= image != background_;
        background_ = image;
        backgroundOpacity_ = opacity;
    }

    void GPUBackend::set_background_painter(BackgroundPainter painter) {
        backgroundPainter_ = std::move(painter);
    }

    bool GPUBackend::has_background_painter() const {
        return static_cast<bool>(backgroundPainter_);
    }

    SDL_Window* GPUBackend::handle() const {
        return handle_;
    }

    Window* GPUBackend::window() {
        return window_;
    }

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
            SDL_SetWindowSize(handle_, width, height);
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
