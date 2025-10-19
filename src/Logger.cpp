#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

void InitializeLogger() {
    auto console = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(console);
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Logger initialized");
}
