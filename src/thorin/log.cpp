#include <mutex>
#include <vector>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "thorin/log.hpp"

namespace thorin {
    namespace {
        constexpr const char* thorinName = "thorin";
        constexpr const char* appName = "app";

        // Guards the shared sinks and config; spdlog's sinks are thread-safe (_mt) themselves.
        std::mutex mutex;
        std::vector<spdlog::sink_ptr> sinks;
        LogConfig current;
        bool configured = false;

        std::vector<spdlog::sink_ptr> make_sinks(const LogConfig& config) {
            std::vector<spdlog::sink_ptr> result;

            if (config.console) {
                result.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
            }

            if (!config.file.empty()) {
                result.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                    config.file.string(), config.maxFileSize, config.maxFiles
                ));
            }
            return result;
        }

        // Points a logger at the shared sinks and the config's level for its kind.
        void apply(spdlog::logger& logger) {
            logger.sinks() = sinks;
            logger.set_level(logger.name() == thorinName ? current.thorinLevel : current.level);
            logger.set_pattern(current.pattern);
            // Files are written in chunks; make sure problems reach the disk right away.
            logger.flush_on(spdlog::level::warn);
        }

        void configure_locked(const LogConfig& config) {
            current = config;
            sinks = make_sinks(config);
            configured = true;

            if (spdlog::get(thorinName) == nullptr) {
                spdlog::register_logger(std::make_shared<spdlog::logger>(thorinName));
            }

            if (spdlog::default_logger()->name() != appName) {
                auto app = std::make_shared<spdlog::logger>(appName);
                spdlog::set_default_logger(app);
            }
            spdlog::apply_all([](const std::shared_ptr<spdlog::logger>& logger) {
                apply(*logger);
            });
        }
    }

    spdlog::logger& logger() {
        std::lock_guard lock(mutex);

        if (!configured) {
            configure_locked({});
        }
        return *spdlog::get(thorinName);
    }

    void configure_logging(const LogConfig& config) {
        std::lock_guard lock(mutex);
        configure_locked(config);
    }

    std::shared_ptr<spdlog::logger> make_logger(const std::string& name) {
        std::lock_guard lock(mutex);

        if (!configured) {
            configure_locked({});
        }

        if (auto existing = spdlog::get(name)) {
            return existing;
        }
        auto result = std::make_shared<spdlog::logger>(name);
        apply(*result);
        spdlog::register_logger(result);
        return result;
    }
}
