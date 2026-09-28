
#pragma once

#include <memory>
#include <ranges>
#include <vector>

#include <libassert/assert.hpp>

#include "thorin/widget.hpp"

namespace thorin {
    // Owns widgets through unique_ptr, so references returned by add() and at() stay valid
    // for the Container's lifetime. Holds no layout logic: the owner decides whether the
    // widgets join its Yoga tree (by passing itself as parent) or are drawn outside it.
    template <WidgetConcept T>
    class Container {
        std::vector<std::unique_ptr<T>> items_;

    public:
        template <WidgetConcept U = T, class... Args>
        requires std::derived_from<U, T>
        U& add(Args&&... args) {
            auto item = std::make_unique<U>(std::forward<Args>(args)...);
            auto& ref = *item;
            items_.push_back(std::move(item));
            return ref;
        }

        [[nodiscard]] size_t count() const {
            return items_.size();
        }

        T& at(size_t index) {
            DEBUG_ASSERT(index < items_.size(), "Container::at index out of range", index, items_.size());
            return *items_[index];
        }

    protected:
        // The widgets as T&, in insertion order. Ownership stays out of reach: the
        // unique_ptrs can't be reset or moved out through this view.
        auto items() {
            return items_ | std::views::transform([](std::unique_ptr<T>& item) -> T& { return *item; });
        }
    };
}
