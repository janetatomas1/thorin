
#include <vector>
#include <moodycamel/concurrentqueue.h>

#include "thorin/action.hpp"

namespace thorin {
    // Runs functions on the UI thread between frames. Actions can be added from any thread; delayed ones
    // wait in a ring of maxDelayFrames slots. Thorin owns one; use Thorin::add_action() or Widget::post().
    class ActionManager {
        moodycamel::ConcurrentQueue<std::pair<action, uint64_t>> immediate_;
        std::vector<std::vector<action>> ring_;
        uint32_t frameIndex_ = 0;
        size_t delayed_ = 0;

    public:
        ActionManager(uint64_t maxDelayFrames = 256);
        // delay is the number of dispatches to wait: 0 runs on the next dispatch, 1 on the one after, and so on.
        // The largest delay is maxDelayFrames - 1; a longer one runs at that limit.
        void add_action(action &&fn, uint64_t delay = 0);
        // Runs the actions that are due, in the order they were added. Called once per frame by Thorin.
        void dispatch();
        // Whether any action is queued or waiting in the delay ring.
        [[nodiscard]] bool pending() const;
    };
}
