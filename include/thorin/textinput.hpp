
#pragma once

#include <string>
#include <functional>

#include "thorin/widget.hpp"

namespace thorin {
    using TextInputFlags = ImGuiInputTextFlags;

    class TextInput : public Widget {
        std::string value_;
        std::string hint_;
        std::function<void(const std::string&)> onChange_;
        TextInputFlags flags_ = ImGuiInputTextFlags_None;
        bool multiline_ = false;

    public:
        TextInput(const std::string& title = "", Widget* parent = nullptr);

        bool show() final;
        ImVec2 measure(float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) final;

        [[nodiscard]] const std::string& value() const;
        TextInput& set_value(const std::string& value);

        [[nodiscard]] const std::string& hint() const;
        TextInput& set_hint(const std::string& hint);

        [[nodiscard]] TextInputFlags flags() const;
        TextInput& set_flags(TextInputFlags flags);

        // Multiline ignores hint() - InputTextMultiline has no hinted variant in ImGui.
        [[nodiscard]] bool multiline() const;
        TextInput& set_multiline(bool multiline);

        TextInput& set_on_change(std::function<void(const std::string&)> callback);
    };
}
