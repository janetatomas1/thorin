#include <glbinding/gl/enum.h>

#include "thorin/gl/renderwidget.hpp"
#include "thorin/window.hpp"

namespace thorin {
    RenderWidget::RenderWidget(Widget* parent): Widget("", parent) {}

    globjects::Texture* RenderWidget::texture() const {
        return texture_.get();
    }

    RenderWidget& RenderWidget::set_texture(std::unique_ptr<globjects::Texture> texture) {
        framebuffer_.reset();
        texture_ = std::move(texture);

        if (texture_ != nullptr) {
            framebuffer_ = std::make_unique<globjects::Framebuffer>();
            framebuffer_->attachTexture(gl::GL_COLOR_ATTACHMENT0, texture_.get());
        }
        return *this;
    }

    RenderWidget& RenderWidget::resize(int width, int height) {
        if (texture_ != nullptr) {
            const auto format = static_cast<gl::GLenum>(texture_->getLevelParameter(0, gl::GL_TEXTURE_INTERNAL_FORMAT));
            // No data is uploaded, but format and type must still be valid for a colour format.
            texture_->image2D(0, format, width, height, 0, gl::GL_RGBA, gl::GL_UNSIGNED_BYTE, nullptr);
        }
        return *this;
    }

    globjects::Framebuffer* RenderWidget::framebuffer() const {
        return framebuffer_.get();
    }

    bool RenderWidget::flip_uv() const {
        return flipUv_;
    }

    RenderWidget& RenderWidget::set_flip_uv(bool flip) {
        flipUv_ = flip;
        return *this;
    }

    ImVec2 RenderWidget::uv0() const {
        return flipUv_ ? ImVec2{0.0f, 1.0f} : ImVec2{0.0f, 0.0f};
    }

    ImVec2 RenderWidget::uv1() const {
        return flipUv_ ? ImVec2{1.0f, 0.0f} : ImVec2{1.0f, 1.0f};
    }

    void RenderWidget::destroy() {
        // GL objects are deleted in their own context, before the backend destroys it.
        if (texture_ != nullptr) {
            window()->backend()->make_current();
            framebuffer_.reset();
            texture_.reset();
        }
        Widget::destroy();
    }

    bool RenderWidget::show() {
        if (texture_ != nullptr) {
            ImGui::Image(static_cast<ImTextureID>(texture_->id()), size(), uv0(), uv1());
        }
        return false;
    }
}
