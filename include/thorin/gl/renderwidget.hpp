#pragma once

#include <memory>

#include <globjects/Texture.h>

#include "thorin/widget.hpp"

namespace thorin {
    // Draws a GL texture at this widget's Yoga box; it has no intrinsic size.
    // The texture must belong to this widget's window's GL context.
    class RenderWidget : public Widget {
        std::unique_ptr<globjects::Texture> texture_;

    public:
        RenderWidget(Widget* parent = nullptr);

        [[nodiscard]] globjects::Texture* texture() const;
        RenderWidget& set_texture(std::unique_ptr<globjects::Texture> texture);

        void destroy() override;
        bool show() final;
    };
}
