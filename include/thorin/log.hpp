#pragma once

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>

#include <spdlog/spdlog.h>

namespace thorin {
    // Where log messages go. All loggers share the same sinks: thorin's ("thorin"), the app's default
    // logger ("app", spdlog's default) and every logger made with make_logger().
    struct LogConfig {
        // Level of the app's loggers (the default logger and make_logger() ones).
        spdlog::level::level_enum level = spdlog::level::info;
        // Level of the framework's "thorin" logger, set apart so it can be quieted on its own.
        spdlog::level::level_enum thorinLevel = spdlog::level::info;
        bool console = true;
        // Rotating log file; empty = none.
        std::filesystem::path file;
        std::size_t maxFileSize = 5 * 1024 * 1024;
        std::size_t maxFiles = 3;
        std::string pattern = "[%H:%M:%S.%e] [%n] [%^%l%$] %v";
    };

    // The framework's logger. Usable before the app exists; logs to the console until configured.
    [[nodiscard]] spdlog::logger& logger();

    // Applies config to every registered logger (sinks, levels, pattern). Thorin calls it with the defaults on
    // construction; call it again (or Thorin::set_log_config) to change it.
    void configure_logging(const LogConfig& config);

    // A named app logger sharing the configured sinks, e.g. make_logger("zion.cpu"). Registered with spdlog,
    // so spdlog::get(name) finds it too.
    std::shared_ptr<spdlog::logger> make_logger(const std::string& name);
}
