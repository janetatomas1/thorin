
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "thorin/widget.hpp"
#include "thorin/radiobutton.hpp"

namespace thorin {
    class RadioGroup : public Widget {
        int selected_ = 0;
        std::vector<RadioButton> options_;

    public:
        RadioGroup(Widget* parent = nullptr);

        // Returned reference is invalidated by any subsequent add_option() call
        // (options_ is a vector and may reallocate). Chain off it immediately;
        // don't store it across further add_option() calls.
        RadioButton& add_option(const std::string& label);
        RadioButton& add_option(const std::string& label, std::function<void()> onSelect);

        [[nodiscard]] int selected() const;
        void set_selected(int index);

        [[nodiscard]] size_t count() const;
        // Same invalidation caveat as add_option(): don't hold this reference
        // across a later add_option() call.
        RadioButton& at(size_t index);
        bool show() override;
    };
}
