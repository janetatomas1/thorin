
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

        RadioButton& add_option(const std::string& label);
        RadioButton& add_option(const std::string& label, std::function<void()> onSelect);

        [[nodiscard]] int selected() const;
        void set_selected(int index);

        [[nodiscard]] size_t count() const;
        RadioButton& option_at(size_t index);
        bool show() override;
    };
}
