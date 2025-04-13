#include "Engine/Logger.hpp"

#include "ImplLogger.hpp"

namespace HyperEngine
{
    Logger::Logger(const std::string& name)
    {
        impl = std::make_shared<ImplLogger>(name);
    }

    void Logger::Info(const std::string& log)
    {
        impl->Info(log);
    }

    void Logger::Warn(const std::string& log)
    {
        impl->Warn(log);
    }

    void Logger::Error(const std::string& log)
    {
        impl->Error(log);
    }

    bool Logger::IsValid() const {
        return impl->_isValid;
    }

    std::vector<Logger::LoggerEntry> Logger::GetLogs()
    {
        return impl->GetLogs();
    }

    std::shared_ptr<spdlog::logger> Logger::GetLogger() const
    {
        return impl->_logger;
    }
}
