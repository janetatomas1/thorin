#include <cmath>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>

#include "thorin/gl/renderwidget.hpp"
#include "thorin/thorin.hpp"
#include "thorin/window.hpp"

namespace thorin {
    RenderWidget::RenderWidget(Widget* parent): Widget("", parent) {}

    globjects::Texture* RenderWidget::texture() const {
        return texture_.get();
    }

    void RenderWidget::rebuild_framebuffer() {
        framebuffer_.reset();
        depth_.reset();

        if (texture_ == nullptr) {
            return;
        }

        framebuffer_ = std::make_unique<globjects::Framebuffer>();
        framebuffer_->attachTexture(gl::GL_COLOR_ATTACHMENT0, texture_.get());

        if (hasDepth_) {
            depth_ = std::make_unique<globjects::Renderbuffer>();
            depth_->storage(gl::GL_DEPTH_COMPONENT24, textureWidth_, textureHeight_);
            framebuffer_->attachRenderBuffer(gl::GL_DEPTH_ATTACHMENT, depth_.get());
        }
    }

    RenderWidget& RenderWidget::set_texture(std::unique_ptr<globjects::Texture> texture) {
        texture_ = std::move(texture);
        textureWidth_ = 0;
        textureHeight_ = 0;

        if (texture_ != nullptr) {
            textureWidth_ = texture_->getLevelParameter(0, gl::GL_TEXTURE_WIDTH);
            textureHeight_ = texture_->getLevelParameter(0, gl::GL_TEXTURE_HEIGHT);
        }
        rebuild_framebuffer();
        return *this;
    }

    RenderWidget& RenderWidget::resize(int width, int height) {
        if (texture_ != nullptr) {
            const auto format = static_cast<gl::GLenum>(texture_->getLevelParameter(0, gl::GL_TEXTURE_INTERNAL_FORMAT));
            // No data is uploaded, but format and type must still be valid for a colour format.
            texture_->image2D(0, format, width, height, 0, gl::GL_RGBA, gl::GL_UNSIGNED_BYTE, nullptr);
            textureWidth_ = width;
            textureHeight_ = height;

            if (depth_ != nullptr) {
                depth_->storage(gl::GL_DEPTH_COMPONENT24, width, height);
            }
        }
        return *this;
    }

    globjects::Framebuffer* RenderWidget::framebuffer() const {
        return framebuffer_.get();
    }

    int RenderWidget::texture_width() const {
        return textureWidth_;
    }

    int RenderWidget::texture_height() const {
        return textureHeight_;
    }

    RenderWidget& RenderWidget::set_on_render(RenderCallback callback) {
        onRender_ = std::move(callback);
        return *this;
    }

    bool RenderWidget::auto_size() const {
        return autoSize_;
    }

    RenderWidget& RenderWidget::set_auto_size(bool enabled) {
        autoSize_ = enabled;
        return *this;
    }

    bool RenderWidget::depth() const {
        return hasDepth_;
    }

    RenderWidget& RenderWidget::set_depth(bool enabled) {
        if (hasDepth_ != enabled) {
            hasDepth_ = enabled;
            rebuild_framebuffer();
        }
        return *this;
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
            depth_.reset();
            texture_.reset();
        }
        Widget::destroy();
    }

    void RenderWidget::render() {
        if (texture_ == nullptr) {
            return;
        }

        if (autoSize_) {
            // Windows are high pixel density, so the box in points is not the pixel count.
            const ImVec2 scale = ImGui::GetIO().DisplayFramebufferScale;
            const int width = static_cast<int>(std::lround(size().x * scale.x));
            const int height = static_cast<int>(std::lround(size().y * scale.y));

            // The box is empty until the first layout; a zero-sized texture is invalid.
            if (width > 0 && height > 0 && (width != textureWidth_ || height != textureHeight_)) {
                resize(width, height);
            }
        }

        if (onRender_ && textureWidth_ > 0 && textureHeight_ > 0) {
            gl::GLint viewport[4];
            gl::glGetIntegerv(gl::GL_VIEWPORT, viewport);
            gl::GLint previous = 0;
            gl::glGetIntegerv(gl::GL_FRAMEBUFFER_BINDING, &previous);

            framebuffer_->bind(gl::GL_FRAMEBUFFER);
            gl::glViewport(0, 0, textureWidth_, textureHeight_);
            onRender_(*this);

            gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, static_cast<gl::GLuint>(previous));
            gl::glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

            Thorin::current().request_redraw();
        }
    }

    bool RenderWidget::show() {
        if (texture_ == nullptr) {
            return false;
        }

        render();
        ImGui::Image(static_cast<ImTextureID>(texture_->id()), size(), uv0(), uv1());
        return false;
    }
}
