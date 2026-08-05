
#include <vector>
#include <moodycamel/concurrentqueue.h>
#include <functional>

namespace thorin {
    class ActionManager {
        moodycamel::ConcurrentQueue<std::pair<std::move_only_function<void()>, uint64_t>> immediate_;
        std::vector<std::vector<std::move_only_function<void()>>> ring_;
        uint32_t frameIndex_ = 0;

    public:
        ActionManager(uint64_t maxDelayFrames = 256);
        void add_action(std::move_only_function<void()> fn, uint64_t delay = 1);
        void dispatch();
    };

}
