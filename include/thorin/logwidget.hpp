#pragma once

#include <cstdint>
#include <vector>

#include "thorin/log.hpp"
#include "thorin/widget.hpp"

namespace thorin {
    // Shows the messages kept by log_memory() (LogConfig::memoryCapacity), from the start of the app: a toolbar
    // with a minimum level, a text filter, Clear and Copy, above a scrolling list coloured by level. It follows
    // new messages while scrolled to the bottom. Shows nothing when the buffer is off.
    // This widget has no intrinsic size: give it one (width/height, flex_grow).
    class LogWidget : public Widget {
        std::vector<LogEntry> entries_;
        // Indices into entries_ passing the level and the filter.
        std::vector<int> visible_;
        std::uint64_t version_ = UINT64_MAX;
        spdlog::level::level_enum minLevel_ = spdlog::level::trace;
        ImGuiTextFilter filter_;
        bool refilter_ = true;

        // Copies the buffer when it changed and rebuilds visible_ when anything did.
        void refresh(MemorySink& memory);

    public:
        LogWidget(Widget* parent = nullptr);

        // Messages below it are hidden.
        [[nodiscard]] spdlog::level::level_enum min_level() const;
        LogWidget& set_min_level(spdlog::level::level_enum level);

        bool show() override;
    };
}
