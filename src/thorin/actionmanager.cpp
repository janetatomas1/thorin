#include <libassert/assert.hpp>

#include "thorin/actionmanager.hpp"

namespace thorin {
    ActionManager::ActionManager(uint64_t maxDelayFrames) {
        DEBUG_ASSERT(maxDelayFrames > 0, "ActionManager requires at least 1 ring slot", maxDelayFrames);

        ring_.resize(maxDelayFrames);
        for (auto& bucket : ring_) {
            bucket.reserve(16);
        }
    }

    void ActionManager::add_action(action &&fn, uint64_t delay) {
        DEBUG_ASSERT(fn != nullptr, "add_action called with an empty/null action");
        immediate_.enqueue({std::move(fn), delay});
    }

    void ActionManager::dispatch() {
        DEBUG_ASSERT(!ring_.empty(), "dispatch called on ActionManager with empty ring");
        DEBUG_ASSERT(frameIndex_ < ring_.size(), "frameIndex_ out of range", frameIndex_, ring_.size());

        std::pair<action, uint64_t> action;
        while (immediate_.try_dequeue(action)) {
            DEBUG_ASSERT(action.first != nullptr, "dequeued a null/empty action");

            if(action.second <= 1) {
                action.first();
            } else {
                const size_t offset = action.second < ring_.size() ? action.second : ring_.size() - 1;
                const size_t index = (frameIndex_ + offset) % ring_.size();
                DEBUG_ASSERT(index < ring_.size(), "computed ring index out of range", index, ring_.size());
                ring_[index].push_back(std::move(action.first));
            }
        }

        // run delayed actions
        auto& bucket = ring_[frameIndex_];
        for (auto& act : bucket) {
            DEBUG_ASSERT(act != nullptr, "delayed bucket contains a null/empty action");
            act();
        }
        bucket.clear();

        frameIndex_ = (frameIndex_ + 1) % ring_.size();
    }
}
