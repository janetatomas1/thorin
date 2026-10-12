#include <string>

#include "thorin/logwidget.hpp"

namespace thorin {
    namespace {
        ImVec4 level_color(spdlog::level::level_enum level) {
            switch (level) {
                case spdlog::level::trace:
                case spdlog::level::debug: return ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
                case spdlog::level::warn: return {1.0f, 0.78f, 0.25f, 1.0f};
                case spdlog::level::err:
                case spdlog::level::critical: return {1.0f, 0.35f, 0.35f, 1.0f};
                default: return ImGui::GetStyleColorVec4(ImGuiCol_Text);
            }
        }
    }

    LogWidget::LogWidget(Widget* parent): Widget("", parent) {}

    spdlog::level::level_enum LogWidget::min_level() const {
        return minLevel_;
    }

    LogWidget& LogWidget::set_min_level(spdlog::level::level_enum level) {
        minLevel_ = level;
        refilter_ = true;
        return *this;
    }

    void LogWidget::refresh(MemorySink& memory) {
        if (const auto version = memory.version(); version != version_) {
            memory.copy(entries_);
            version_ = version;
            refilter_ = true;
        }

        if (!refilter_) {
            return;
        }
        visible_.clear();

        for (int i = 0; i < static_cast<int>(entries_.size()); i++) {
            const LogEntry& entry = entries_[i];

            if (entry.level >= minLevel_ && filter_.PassFilter(entry.line.c_str())) {
                visible_.push_back(i);
            }
        }
        refilter_ = false;
    }

    bool LogWidget::show() {
        const auto memory = log_memory();
        // id(), not the title: renaming must not reset the scroll position.
        ImGui::PushID(static_cast<int>(id()));
        ImGui::BeginChild("log", size());

        if (memory == nullptr) {
            ImGui::TextDisabled("Log buffer is off (LogConfig::memoryCapacity = 0)");
            ImGui::EndChild();
            ImGui::PopID();
            return false;
        }

        // Toolbar.
        static constexpr const char* levels[] = {"trace", "debug", "info", "warn", "error", "critical"};
        int level = static_cast<int>(minLevel_);
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 7.0f);

        if (ImGui::Combo("##level", &level, levels, IM_ARRAYSIZE(levels))) {
            set_min_level(static_cast<spdlog::level::level_enum>(level));
        }
        ImGui::SameLine();
        const float buttons = ImGui::CalcTextSize("ClearCopy").x
            + ImGui::GetStyle().FramePadding.x * 4.0f
            + ImGui::GetStyle().ItemSpacing.x * 2.0f;

        if (filter_.Draw("##filter", ImGui::GetContentRegionAvail().x - buttons)) {
            refilter_ = true;
        }
        ImGui::SameLine();

        if (ImGui::Button("Clear")) {
            memory->clear();
        }
        ImGui::SameLine();
        const bool copy = ImGui::Button("Copy");

        refresh(*memory);

        if (copy) {
            std::string text;

            for (const int i: visible_) {
                text += entries_[i].line;
                text += '\n';
            }
            ImGui::SetClipboardText(text.c_str());
        }

        // Rows.
        ImGui::BeginChild("rows", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);
        // Read before adding rows: following only while the user is at the bottom.
        const bool follow = ImGui::GetScrollY() >= ImGui::GetScrollMaxY();
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(visible_.size()));

        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                const LogEntry& entry = entries_[visible_[row]];
                ImGui::PushStyleColor(ImGuiCol_Text, level_color(entry.level));
                ImGui::TextUnformatted(entry.line.c_str(), entry.line.c_str() + entry.line.size());
                ImGui::PopStyleColor();
            }
        }
        clipper.End();

        if (follow) {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();

        ImGui::EndChild();
        ImGui::PopID();
        return false;
    }
}
