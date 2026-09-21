#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <string>

#include "thorin/widget.hpp"

namespace thorin {
    using ColorEditFlags = ImGuiColorEditFlags;

    // An RGB (N == 3) or RGBA (N == 4) color, each component in 0..1. Clicking the swatch opens a picker.
    template <std::size_t N>
    class ColorEdit : public Widget {
        static_assert(N == 3 || N == 4, "ColorEdit supports 3 (RGB) or 4 (RGBA) components");

    public:
        using Value = std::array<float, N>;

    private:
        Value value_{};
        ColorEditFlags flags_ = ImGuiColorEditFlags_None;
        std::function<void(const Value&)> onChange_;

    public:
        ColorEdit(const std::string& title = "", Widget* parent = nullptr);

        bool show() override;

        [[nodiscard]] const Value& value() const;
        ColorEdit& set_value(const Value& value);

        [[nodiscard]] ColorEditFlags flags() const;
        ColorEdit& set_flags(ColorEditFlags flags);

        ColorEdit& set_on_change(std::function<void(const Value&)> callback);
    };

    template <std::size_t N>
    ColorEdit<N>::ColorEdit(const std::string& title, Widget* parent)
    : Widget(title, parent) {}

    template <std::size_t N>
    bool ColorEdit<N>::show() {
        if (width() > 0.0f) {
            ImGui::SetNextItemWidth(width());
        }

        const std::string id = title_id();
        bool changed;

        if constexpr (N == 3) {
            changed = ImGui::ColorEdit3(id.c_str(), value_.data(), flags_);
        } else {
            changed = ImGui::ColorEdit4(id.c_str(), value_.data(), flags_);
        }

        if (changed && onChange_) {
            onChange_(value_);
        }

        return changed;
    }

    template <std::size_t N>
    const typename ColorEdit<N>::Value& ColorEdit<N>::value() const {
        return value_;
    }

    template <std::size_t N>
    ColorEdit<N>& ColorEdit<N>::set_value(const Value& value) {
        value_ = value;
        return *this;
    }

    template <std::size_t N>
    ColorEditFlags ColorEdit<N>::flags() const {
        return flags_;
    }

    template <std::size_t N>
    ColorEdit<N>& ColorEdit<N>::set_flags(ColorEditFlags flags) {
        flags_ = flags;
        return *this;
    }

    template <std::size_t N>
    ColorEdit<N>& ColorEdit<N>::set_on_change(std::function<void(const Value&)> callback) {
        onChange_ = std::move(callback);
        return *this;
    }
}
