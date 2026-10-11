
#pragma once

#include <memory>
#include <vector>

#include "thorin/windowmanager.hpp"
#include "thorin/randomgenerator.hpp"
#include "thorin/actionmanager.hpp"
#include "thorin/theme.hpp"
#include "thorin/image.hpp"

namespace thorin {
    class Thorin {
        // The live app, so widgets reach it without a window (e.g. from their constructors).
        static inline Thorin* current_ = nullptr;

        int exitCode_ = 0;
        uint64_t frame_ = 0;
        bool shouldExit_ = false;

        // Before windowManager_, so the fonts outlive the windows whose atlases borrow them. Heap-allocated so
        // the addresses add_font() hands out survive the vector growing.
        std::vector<std::unique_ptr<Font>> fonts_;
        // Like fonts_: before windowManager_, heap-allocated for stable addresses.
        std::vector<std::unique_ptr<Image>> images_;
        const Image* background_ = nullptr;
        float backgroundOpacity_ = 1.0f;
        WindowManager windowManager_ = WindowManager(*this);
        static RandomGenerator randomGenerator_;
        ActionManager actionManager_;
        Theme theme_;

        void init();
        void destroy();
        void update();
    public:
        Thorin(int argc, char** argv);
        ~Thorin();

        // WindowManager and current_ hold this address, so it must not move.
        Thorin(const Thorin&) = delete;
        Thorin& operator=(const Thorin&) = delete;
        Thorin(Thorin&&) = delete;
        Thorin& operator=(Thorin&&) = delete;

        // The live app. Asserts when there is none (e.g. headless tests).
        static Thorin& current();
        [[nodiscard]] static bool has_current();
        int exec();
        void exit();

        static uint64_t random() {
            return randomGenerator_.random();
        }
        [[nodiscard]] uint64_t frame() const;
        [[nodiscard]] const FrameConfig& frame_config() const;
        void set_frame_config(const FrameConfig &config);
        [[nodiscard]] const Theme& theme() const;
        // Applies to every window, open or created later. Open windows switch on the next frame.
        void set_theme(const Theme &theme);
        // The app keeps the font until it exits; the reference stays valid, e.g. for Theme::font.
        const Font& add_font(Font font);
        // Frees a font from add_font() on the next frame. It must not be the current theme's font (set another
        // theme first); references to it dangle afterwards.
        void remove_font(const Font& font);
        // The app keeps the image until it exits or remove_image(); windows draw it with set_background().
        const Image& add_image(Image image);
        // Frees an image from add_image() on the next frame. Neither the app nor an open window may use it as
        // its background.
        void remove_image(const Image& image);
        // Default background for windows created afterwards: Window::init() takes it unless the window already
        // has one. Use an image from add_image(); nullptr = none.
        void set_background(const Image* image, float opacity = 1.0f);
        [[nodiscard]] const Image* background() const;
        [[nodiscard]] float background_opacity() const;
        // Keeps an OnDemand loop drawing for at least this many more frames. Animating widgets call it every
        // frame they are drawn.
        void request_redraw(int frames = 2);
        // Whether any action is queued or delayed.
        [[nodiscard]] bool actions_pending() const;
        // Thread-safe; also wakes an OnDemand loop.
        void add_action(
            action &&fn,
            uint64_t delay = 0
        );
        // The app takes ownership of the window and returns its id. The window is created on the next frame, so
        // get_window(id) is null until then and again once the window has been closed. Use it to check whether
        // a window is still open.
        uint64_t add_window(std::unique_ptr<Window> window);
        uint64_t add_window(const WindowConfig &config = {});
        template<WidgetConcept W, typename... Args>
        uint64_t add_window(WindowConfig config, Args &&... args);
        template<WidgetConcept W, typename... Args>
        uint64_t add_window(Args &&... args);
        Window* get_window_at(size_t index);
        Window* get_window(uint64_t id);
        void remove_window_at(size_t index);
        // Does nothing if the window has already been closed.
        void remove_window(uint64_t id);
    };

    template <WidgetConcept W, typename ... Args>
    uint64_t Thorin::add_window(WindowConfig config, Args&&... args) {
        // The window is registered first so it can reach the app; set_root_widget() then gives the
        // widget its window right away and queues the swap after the window's init, which calls
        // the widget's init(). The widget's constructor still runs without a window.
        auto window = std::make_unique<Window>(config);
        Window* raw = window.get();
        const uint64_t id = add_window(std::move(window));
        raw->set_root_widget(std::make_unique<W>(std::forward<Args>(args)...));
        return id;
    }

    template <WidgetConcept W, typename ... Args>
    uint64_t Thorin::add_window(Args&&... args) {
        return add_window<W>({}, std::forward<Args>(args)...);
    }
}
