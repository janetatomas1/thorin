
#pragma once

#include <memory>

#include "thorin/windowmanager.hpp"
#include "thorin/randomgenerator.hpp"
#include "thorin/actionmanager.hpp"

namespace thorin {
    class Thorin {
        // The live app, so widgets reach it without a window (e.g. from their constructors).
        static inline Thorin* current_ = nullptr;

        int exitCode_ = 0;
        uint64_t frame_ = 0;
        bool shouldExit_ = false;

        WindowManager windowManager_ = WindowManager(*this);
        static RandomGenerator randomGenerator_;
        ActionManager actionManager_;

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
