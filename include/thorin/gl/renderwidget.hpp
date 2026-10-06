#pragma once

#include <memory>

#include <globjects/Framebuffer.h>
#include <globjects/Texture.h>

#include "thorin/widget.hpp"

namespace thorin {
    // Draws a GL texture at this widget's Yoga box; it has no intrinsic size.
    // The texture must belong to this widget's window's GL context.
    class RenderWidget : public Widget {
        std::unique_ptr<globjects::Texture> texture_;
        std::unique_ptr<globjects::Framebuffer> framebuffer_;
        bool flipUv_ = false;

    public:
        RenderWidget(Widget* parent = nullptr);

        [[nodiscard]] globjects::Texture* texture() const;
        // Also creates framebuffer() with the texture as its colour attachment; nullptr removes both.
        // Call with the window's GL context current.
        RenderWidget& set_texture(std::unique_ptr<globjects::Texture> texture);
        // Bind it to render into texture(). Has no depth attachment; add one if needed.
        [[nodiscard]] globjects::Framebuffer* framebuffer() const;

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
