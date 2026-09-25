
#pragma once

#include <string>
#include <functional>

#include "thorin/widget.hpp"
#include "thorin/container.hpp"
#include "thorin/radiobutton.hpp"

namespace thorin {
    // Container is protected: options are added through add_option(), which links them to selected_.
    class RadioGroup : public Widget, protected Container<RadioButton> {
        int selected_ = 0;

    public:
        using Container::count;
        using Container::at;
        using Container::items;

        RadioGroup(Widget* parent = nullptr);

        // Options hold a pointer to selected_, so moves re-point them at the new group.
        RadioGroup(RadioGroup&& other) noexcept;
        RadioGroup& operator=(RadioGroup&& other) noexcept;

        // Returned reference stays valid for the RadioGroup's lifetime.
        RadioButton& add_option(const std::string& label);
        RadioButton& add_option(const std::string& label, std::function<void()> onSelect);

        [[nodiscard]] int selected() const;
        void set_selected(int index);

        bool show() override;
    };
}
