#include <array>
#include <cstdint>

#include <glbinding/gl/gl.h>

#include <thorin/thorin.hpp>
#include <thorin/text.hpp>
#include <thorin/gl/imagebutton.hpp>

using namespace thorin;
using namespace literals;
using namespace gl;

// A 2x2 checkerboard in the given colour, scaled up with nearest filtering.
std::unique_ptr<globjects::Texture> checkerboard(uint32_t rgba) {
    const std::array<uint32_t, 4> pixels = {rgba, 0xffffffff, 0xffffffff, rgba};

    auto texture = globjects::Texture::createDefault(GL_TEXTURE_2D);
    texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    texture->image2D(0, GL_RGBA8, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    return texture;
}

struct ImageButtonExample: Widget {
    Text title = Text("Image button example", this);
    ImageButton red = ImageButton("red", [this] { status.set_title("Red clicked"); }, this);
    ImageButton blue = ImageButton("blue", [this] { status.set_title("Blue clicked"); }, this);
    Text status = Text("Click a button", this);

    ImageButtonExample(const std::string& title): Widget(title) {
        column(10);
        padding(10);
        align_items(YGAlignFlexStart);

        red.width(64).height(64);
        blue.width(128).height(64);
    }

    void init() override {
        // Textures need this window's GL context.
        window()->backend()->make_current();
        red.set_texture(checkerboard(0xff0000ff));   // little endian: 0xAABBGGRR
        blue.set_texture(checkerboard(0xffff0000));
        Widget::init();
    }
};

int main(const int argc, char** argv) {
    auto app = Thorin(argc, argv);
    app.add_window<ImageButtonExample>(std::string("ImageButton Example"));
    return app.exec();
}
