#include "thorin/gl/renderwidget.hpp"
#include "thorin/window.hpp"

namespace thorin {
    RenderWidget::RenderWidget(Widget* parent): Widget("", parent) {}

    globjects::Texture* RenderWidget::texture() const {
        return texture_.get();
    }

    RenderWidget& RenderWidget::set_texture(std::unique_ptr<globjects::Texture> texture) {
        texture_ = std::move(texture);
        return *this;
    }

    void RenderWidget::destroy() {
        // The texture is deleted in its own context, before the backend destroys it.
        if (texture_ != nullptr) {
            window()->backend()->make_current();
            texture_.reset();
        }
        Widget::destroy();
    }

    bool RenderWidget::show() {
        if (texture_ != nullptr) {
            ImGui::Image(static_cast<ImTextureID>(texture_->id()), size());
        }
        return false;
    }
}
