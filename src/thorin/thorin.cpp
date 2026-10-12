#include <algorithm>

#include <libassert/assert.hpp>

#include "thorin/thorin.hpp"
#include "thorin/version.hpp"

namespace thorin {
    RandomGenerator Thorin::randomGenerator_{};

    void Thorin::init() {}

    void Thorin::update() {
        actionManager_.dispatch();
        windowManager_.update();

        frame_ += 1;
        shouldExit_ |= windowManager_.count() == 0;
    }

    Thorin::Thorin([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
        DEBUG_ASSERT(argc >= 0, "argc is negative", argc);
        DEBUG_ASSERT(argc == 0 || argv != nullptr, "argv is null but argc is nonzero", argc);
        DEBUG_ASSERT(current_ == nullptr, "only one Thorin may exist at a time");
        current_ = this;
        configure_logging({});

        // SDL's own messages (errors, warnings) go to the thorin logger.
        SDL_SetLogOutputFunction(
            [](void*, int category, SDL_LogPriority priority, const char* message) {
                spdlog::level::level_enum level = spdlog::level::info;

                switch (priority) {
                    case SDL_LOG_PRIORITY_TRACE:
                    case SDL_LOG_PRIORITY_VERBOSE: level = spdlog::level::trace; break;
                    case SDL_LOG_PRIORITY_DEBUG: level = spdlog::level::debug; break;
                    case SDL_LOG_PRIORITY_WARN: level = spdlog::level::warn; break;
                    case SDL_LOG_PRIORITY_ERROR: level = spdlog::level::err; break;
                    case SDL_LOG_PRIORITY_CRITICAL: level = spdlog::level::critical; break;
                    default: break;
                }
                logger().log(level, "SDL ({}): {}", category, message);
            },
            nullptr
        );

        windowManager_.init();
        wake_on_log(true);

        const int sdl = SDL_GetVersion();
        logger().info(
            "thorin {} starting, SDL {}.{}.{}",
            THORIN_VERSION,
            SDL_VERSIONNUM_MAJOR(sdl),
            SDL_VERSIONNUM_MINOR(sdl),
            SDL_VERSIONNUM_MICRO(sdl)
        );
    }

    Thorin::~Thorin() {
        wake_on_log(false);
        SDL_SetLogOutputFunction(SDL_GetDefaultLogOutputFunction(), nullptr);
        spdlog::apply_all([](const std::shared_ptr<spdlog::logger>& l) { l->flush(); });
        current_ = nullptr;
    }

    Thorin& Thorin::current() {
        DEBUG_ASSERT(current_ != nullptr, "Thorin::current called with no Thorin instance");
        return *current_;
    }

    bool Thorin::has_current() {
        return current_ != nullptr;
    }

    void Thorin::destroy() {
        // SDL shuts down with the windows; no more wake events after that.
        wake_on_log(false);
        windowManager_.destroy();
    }

    int Thorin::exec() {
        init();

        while(!shouldExit_) {
            update();
        }

        destroy();
        logger().info("thorin exiting with code {} after {} frames", exitCode_, frame_);
        return exitCode_;
    }

    void Thorin::exit() {
        add_action([this]() {
            shouldExit_ = true;
        });
    }

    uint64_t Thorin::frame() const {
        return frame_;
    }

    const FrameConfig& Thorin::frame_config() const {
        return windowManager_.frame_config();
    }

    void Thorin::set_frame_config(const FrameConfig &config) {
        windowManager_.set_frame_config(config);
    }

    const Theme& Thorin::theme() const {
        return theme_;
    }

    void Thorin::set_theme(const Theme &theme) {
        theme_ = theme;
        // Deferred: replacing the style mid-frame would be undone by the pops of styles pushed before it.
        add_action([this]() {
            windowManager_.apply_theme(theme_);
        });
    }

    void Thorin::set_log_config(const LogConfig &config) {
        configure_logging(config);
        // The buffer may be new.
        wake_on_log(true);
    }

    void Thorin::wake_on_log(bool enabled) {
        if (auto memory = log_memory()) {
            if (enabled) {
                memory->set_on_message([this] { windowManager_.wake(); });
            } else {
                memory->set_on_message({});
            }
        }
    }

    const Font& Thorin::add_font(Font font) {
        return *fonts_.emplace_back(std::make_unique<Font>(std::move(font)));
    }

    void Thorin::remove_font(const Font& font) {
        DEBUG_ASSERT(theme_.font != &font, "remove_font called with the current theme's font");
        DEBUG_ASSERT(
            std::ranges::any_of(fonts_, [&font](const auto& owned) { return owned.get() == &font; }),
            "remove_font called with a font not from add_font"
        );
        // Queued after any pending set_theme(), whose re-theme drops the font from every window's atlas.
        add_action([this, &font]() {
            std::erase_if(fonts_, [&font](const auto& owned) { return owned.get() == &font; });
        });
    }

    const Image& Thorin::add_image(Image image) {
        return *images_.emplace_back(std::make_unique<Image>(std::move(image)));
    }

    void Thorin::remove_image(const Image& image) {
        DEBUG_ASSERT(
            std::ranges::any_of(images_, [&image](const auto& owned) { return owned.get() == &image; }),
            "remove_image called with an image not from add_image"
        );
        add_action([this, &image]() {
            DEBUG_ASSERT(background_ != &image, "remove_image called with the app's background");
            DEBUG_ASSERT(icon_ != &image, "remove_image called with the app's icon");
            // Checked when it runs, after any set_background() queued before it.
            for (size_t i = 0; i < windowManager_.count(); i++) {
                DEBUG_ASSERT(
                    windowManager_.get_window_at(i)->background() != &image,
                    "remove_image called with an image a window still uses as its background"
                );
            }
            std::erase_if(images_, [&image](const auto& owned) { return owned.get() == &image; });
        });
    }

    void Thorin::set_background(const Image* image, float opacity) {
        DEBUG_ASSERT(opacity >= 0.0f && opacity <= 1.0f, "background opacity out of [0, 1]", opacity);
        background_ = image;
        backgroundOpacity_ = opacity;
    }

    const Image* Thorin::background() const {
        return background_;
    }

    float Thorin::background_opacity() const {
        return backgroundOpacity_;
    }

    void Thorin::set_background_painter(std::function<BackgroundPainter()> factory) {
        backgroundPainterFactory_ = std::move(factory);
    }

    const std::function<BackgroundPainter()>& Thorin::background_painter_factory() const {
        return backgroundPainterFactory_;
    }

    void Thorin::set_icon(const Image* image) {
        icon_ = image;
    }

    const Image* Thorin::icon() const {
        return icon_;
    }

    void Thorin::request_redraw(int frames) {
        windowManager_.request_redraw(frames);
    }

    bool Thorin::actions_pending() const {
        return actionManager_.pending();
    }

    void Thorin::add_action(action &&fn, uint64_t delay) {
        DEBUG_ASSERT(fn != nullptr, "add_action called with an empty/null action");
        actionManager_.add_action(std::move(fn), delay);
        windowManager_.wake();
    }

    uint64_t Thorin::add_window(std::unique_ptr<Window> window) {
        return windowManager_.add_window(std::move(window));
    }

    uint64_t Thorin::add_window(const WindowConfig &config) {
        return windowManager_.add_window(std::make_unique<Window>(config));
    }

    Window* Thorin::get_window_at(size_t index) {
        return windowManager_.get_window_at(index);
    }

    Window* Thorin::get_window(uint64_t id) {
        return windowManager_.get_window(id);
    }

    void Thorin::remove_window_at(size_t index) {
        windowManager_.remove_window_at(index);
    }

    void Thorin::remove_window(uint64_t id) {
        windowManager_.remove_window(id);
    }
}
