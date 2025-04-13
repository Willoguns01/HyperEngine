#pragma once

#include <string>
#include <spdlog/spdlog.h>

namespace HyperEngine
{
    struct ImplLogger;
    class Logger
    {
    public:
        Logger() = default;
        Logger(const std::string& name);
        ~Logger() = default;

        void Info(const std::string& log);
        void Warn(const std::string& log);
        void Error(const std::string& log);

        template <typename... Args>
        void Info(spdlog::format_string_t<Args...> fmt, Args &&...args) {
            GetLogger()->info(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void Warn(spdlog::format_string_t<Args...> fmt, Args &&...args) {
            GetLogger()->warn(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void Error(spdlog::format_string_t<Args...> fmt, Args &&...args) {
            GetLogger()->error(fmt, std::forward<Args>(args)...);
        }

        bool IsValid() const;

        struct LoggerEntry {
            std::string str;
            spdlog::level::level_enum level;
        };

        std::vector<LoggerEntry> GetLogs();

    private:
        std::shared_ptr<spdlog::logger> GetLogger() const;

        std::shared_ptr<ImplLogger> impl = nullptr;
    };
}
