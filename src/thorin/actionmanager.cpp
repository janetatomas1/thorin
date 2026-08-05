
#include "thorin/actionmanager.hpp"

namespace thorin {
    ActionManager::ActionManager(uint64_t maxDelayFrames) {
        ring_.resize(maxDelayFrames);
        for (auto& bucket : ring_) {
            bucket.reserve(16);
        }
    }

    void ActionManager::add_action(std::move_only_function<void()> fn, uint64_t delay) {
        immediate_.enqueue({std::move(fn), delay});
    }

    void ActionManager::dispatch() {
        std::pair<std::move_only_function<void()>, uint64_t> action;
        while (immediate_.try_dequeue(action)) {
            if(action.second <= 1) {
                action.first();
            } else {
                const size_t index = action.second < ring_.size() ? action.second - 1 : ring_.size() - 1;
                ring_[index].push_back(std::move(action.first));
            }
        }

        // run delayed actions
        auto& bucket = ring_[frameIndex_];
        for (auto& action : bucket) {
            action();
        }
        bucket.clear();

        frameIndex_ = (frameIndex_ + 1) % ring_.size();
    }
}
