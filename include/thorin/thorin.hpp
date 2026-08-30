
#pragma once

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
        Window* add_window(Window &&window);
        Window* add_window(const WindowConfig &config = {});
        template<WidgetConcept W, typename... Args>
        Window* add_window(const WindowConfig &config = {}, Args &&... args);
        Window* get_window_at(size_t index);
        Window* get_window(uint64_t id);
        void remove_window_at(size_t index);
        void remove_window(uint64_t id);
    };

    template <WidgetConcept W, typename ... Args>
    Window* Thorin::add_window(const WindowConfig& config, Args&&... args) {
        auto widget = std::make_unique<W>(std::forward<Args>(args)...);
        auto window = Window(config, std::move(widget));
        return add_window(std::move(window));
    }
}
