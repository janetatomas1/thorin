#include <cmath>

#include <glbinding/gl/gl.h>
#include <glm/vec4.hpp>

#include <thorin/thorin.hpp>
#include <thorin/text.hpp>
#include <thorin/gl/renderwidget.hpp>

using namespace thorin;
using namespace literals;
using namespace gl;

struct RenderWidgetExample: Widget {
    Text title = Text("Render widget example", this);
    RenderWidget view = RenderWidget(this);

    RenderWidgetExample(const std::string& title): Widget(title) {
        column(10);
        padding(10);

        // Fills the rest of the window; auto-size keeps the texture at the box's pixel size.
        view.set_flip_uv(true).set_auto_size(true).flex_grow(1);
        view.set_on_render([](RenderWidget &self) {
            // Just a colour that changes with time.
            const float t = static_cast<float>(ImGui::GetTime());
            self.framebuffer()->clearBuffer(GL_COLOR, 0, glm::vec4(
                0.5f + 0.5f * std::sin(t),
                0.5f + 0.5f * std::cos(t + 2.0f),
                0.5f + 0.5f * std::sin(t + 4.0f),
                1.0f
            ));
        });
    }

    void init() override {
        window()->backend()->make_current();

        // The size is replaced by auto-size on the first frame.
        auto texture = globjects::Texture::createDefault(GL_TEXTURE_2D);
        texture->image2D(0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        view.set_texture(std::move(texture));

        Widget::init();
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<RenderWidgetExample>(std::string("RenderWidget Example"));
    return app.exec();
}
