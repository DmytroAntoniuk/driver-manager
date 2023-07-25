#pragma once

#include <logger/configurator.h>
#include <logger/level.h>
#include <logger/logger.h>

#include "logs-dir-path.h"

namespace Brand
{
    inline void ConfigureLogger()
    {
        constexpr size_t max_log_file_size  = 100 * 1024 * 1024;
        spdlog::level::level_enum log_level = spdlog::level::debug;

        Logger::Configurator::Configure({.logs_dir_path = Brand::GetLogsDirPath(), .log_level = log_level, .max_log_file_size = max_log_file_size, .create_console = true});
    }
}  // namespace Brand
