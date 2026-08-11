
#include <vector>
#include <moodycamel/concurrentqueue.h>

#include "thorin/action.hpp"

namespace thorin {
    class ActionManager {
        moodycamel::ConcurrentQueue<std::pair<action, uint64_t>> immediate_;
        std::vector<std::vector<action>> ring_;
        uint32_t frameIndex_ = 0;

    public:
        ActionManager(uint64_t maxDelayFrames = 256);
        void add_action(action &&fn, uint64_t delay = 1);
        void dispatch();
    };
}
