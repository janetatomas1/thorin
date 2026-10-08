#pragma once

#include <memory>

#include <function2/function2.hpp>

#include <globjects/Framebuffer.h>
#include <globjects/Renderbuffer.h>
#include <globjects/Texture.h>

#include "thorin/widget.hpp"

namespace thorin {
    // Draws a GL texture at this widget's Yoga box; it has no intrinsic size.
    // The texture must belong to this widget's window's GL context.
    class RenderWidget : public Widget {
    public:
        // Called from show() with framebuffer() bound and the viewport set to texture_width() x texture_height().
        // Set the GL state you need (depth test, clears); the bound framebuffer and the viewport are restored
        // afterwards.
        using RenderCallback = fu2::unique_function<void(RenderWidget &self)>;

    private:
        std::unique_ptr<globjects::Texture> texture_;
        std::unique_ptr<globjects::Framebuffer> framebuffer_;
        std::unique_ptr<globjects::Renderbuffer> depth_;
        RenderCallback onRender_;
        int textureWidth_ = 0;
        int textureHeight_ = 0;
        bool flipUv_ = false;
        bool autoSize_ = false;
        bool hasDepth_ = false;

        // Recreates framebuffer() for texture_, with a depth attachment if hasDepth_.
        void rebuild_framebuffer();

    protected:
        // Auto-sizes the texture and runs the render callback; show() calls it before drawing the image.
        void render();

    public:
        RenderWidget(Widget* parent = nullptr);

        [[nodiscard]] globjects::Texture* texture() const;
        // Also creates framebuffer() with the texture as its colour attachment; nullptr removes both.
        // Call with the window's GL context current.
        RenderWidget& set_texture(std::unique_ptr<globjects::Texture> texture);
        // Reallocates texture() at the new size, keeping its internal format; the contents are lost.
        // The depth buffer is resized with it. Does nothing without a texture. Needs the GL context current.
        RenderWidget& resize(int width, int height);
        // Bind it to render into texture().
        [[nodiscard]] globjects::Framebuffer* framebuffer() const;
        // The texture's size in pixels, 0 without a texture.
        [[nodiscard]] int texture_width() const;
        [[nodiscard]] int texture_height() const;

        // Called every frame the widget is drawn; it also keeps an OnDemand or Adaptive loop drawing.
        RenderWidget& set_on_render(RenderCallback callback);

        // Resizes the texture to the Yoga box in pixels (points times the framebuffer scale) whenever the
        // box changes. Needs a texture; its initial size does not matter.
        [[nodiscard]] bool auto_size() const;
        RenderWidget& set_auto_size(bool enabled);

        // A 24-bit depth renderbuffer attached to framebuffer(), sized with the texture. Off by default.
        // Call with the window's GL context current.
        [[nodiscard]] bool depth() const;
        RenderWidget& set_depth(bool enabled);

        // Draws the texture upside down. Set it for textures rendered through a framebuffer,
        // since GL's origin is bottom-left and ImGui's is top-left.
        [[nodiscard]] bool flip_uv() const;
        RenderWidget& set_flip_uv(bool flip);
        // Texture coordinates of the top-left and bottom-right corners, with flip_uv() applied.
        [[nodiscard]] ImVec2 uv0() const;
        [[nodiscard]] ImVec2 uv1() const;

        void destroy() override;
        bool show() override;
    };
}
