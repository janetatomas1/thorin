
#pragma once

#include <memory>

#include "thorin/windowmanager.hpp"
#include "thorin/randomgenerator.hpp"
#include "thorin/actionmanager.hpp"

namespace thorin {
    class Thorin {
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
        int exec();
        void exit();

        static uint64_t random() {
            return randomGenerator_.random();
        }
        [[nodiscard]] uint64_t frame() const;
        void add_action(
            action &&fn,
            uint64_t delay = 1
        );
        // The returned window is owned by the app and valid right away, but it is only initialized on the next
        // frame. The pointer dangles once the window is closed.
        Window* add_window(std::unique_ptr<Window> window);
        Window* add_window(const WindowConfig &config = {});
        template<WidgetConcept W, typename... Args>
        Window* add_window(WindowConfig config, Args &&... args);
        template<WidgetConcept W, typename... Args>
        Window* add_window(Args &&... args);
        Window* get_window_at(size_t index);
        Window* get_window(uint64_t id);
        void remove_window_at(size_t index);
        void remove_window(uint64_t id);
    };

    template <WidgetConcept W, typename ... Args>
    Window* Thorin::add_window(WindowConfig config, Args&&... args) {
        auto widget = std::make_unique<W>(std::forward<Args>(args)...);
        return add_window(std::make_unique<Window>(config, std::move(widget)));
    }

    template <WidgetConcept W, typename ... Args>
    Window* Thorin::add_window(Args&&... args) {
        return add_window<W>({}, std::forward<Args>(args)...);
    }
}
