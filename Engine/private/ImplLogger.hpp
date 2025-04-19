#pragma once

#include <memory>
#include <string>

#include <spdlog/spdlog.h>

namespace HyperEngine
{
    struct ImplLogger
    {
        ImplLogger() = default;
        ImplLogger(const std::string& name);
        ~ImplLogger();

        void Info(const std::string& log) const;
        void Warn(const std::string& log) const;
        void Error(const std::string& log) const;

        std::shared_ptr<spdlog::logger> _logger = nullptr;
        std::string _name;

        template <typename... Args>
        void Info(spdlog::format_string_t<Args...> fmt, Args &&...args) const {
            _logger->info(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void Warn(spdlog::format_string_t<Args...> fmt, Args &&...args) const {
            _logger->warn(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void Error(spdlog::format_string_t<Args...> fmt, Args &&...args) const {
            _logger->error(fmt, std::forward<Args>(args)...);
        }

        bool _isValid = false;

        // returns log entries and clears internal buffer
        std::vector<Logger::LoggerEntry> GetLogs();
    };
}
