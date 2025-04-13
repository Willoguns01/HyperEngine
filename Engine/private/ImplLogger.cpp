#include "Engine/Logger.hpp"
#include "ImplLogger.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <spdlog/sinks/callback_sink.h>

#include <mutex>
#include <list>

#include <iostream>

namespace spdlog::sinks
{
    class custom_buffered_sink final : public base_sink<std::mutex>
    {
    public:
        explicit custom_buffered_sink() 
        : _q(512) {}

        std::vector<HyperEngine::Logger::LoggerEntry> GetLatest()
        {
            std::lock_guard<std::mutex> lock(base_sink<std::mutex>::mutex_);
            size_t numItems = _q.size();
            std::vector<HyperEngine::Logger::LoggerEntry> ret;
            ret.reserve(numItems);
            for (size_t i = 0; i < numItems; i++) {
                memory_buf_t formatted;
                base_sink<std::mutex>::formatter_->format(_q.at(i), formatted);
                ret.push_back(HyperEngine::Logger::LoggerEntry{
                    .str = SPDLOG_BUF_TO_STRING(formatted),
                    .level = _q.at(i).level
                });
            }
            for (size_t i = 0; i < numItems; i++) {
                _q.pop_front();
            }
            return std::move(ret);
        }
    
    protected:
        void sink_it_(const details::log_msg& msg) override {
            _q.push_back(details::log_msg_buffer{msg});
        }

        void flush_() override {}

    private:
        details::circular_q<details::log_msg_buffer> _q;
    };
}

namespace
{
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> stdoutSink = nullptr;
    std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> fileSink = nullptr;
    std::shared_ptr<spdlog::sinks::custom_buffered_sink> bufferSink = nullptr;
    std::list<spdlog::sink_ptr> sinkList;
    const char* sinkPattern = "[%R][%L][%n] %^%v%$";
    static bool sinksInitialised = false;
    uint32_t numLoggersAlive = 0;
    std::mutex loggerInitMutex;
}

namespace HyperEngine
{
    ImplLogger::ImplLogger(const std::string& name)
    {
        std::unique_lock<std::mutex> lock(loggerInitMutex);

        _name = name;

        bool firstInit = !sinksInitialised;

        if (!sinksInitialised)
        {
            stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            stdoutSink->set_level(spdlog::level::trace);
            stdoutSink->set_pattern(sinkPattern);

            fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("Logs/log.txt", 1024 * 1024 * 5, 5, true);
            fileSink->set_level(spdlog::level::trace);
            fileSink->set_pattern(sinkPattern);

            bufferSink = std::make_shared<spdlog::sinks::custom_buffered_sink>();
            fileSink->set_level(spdlog::level::trace);
            fileSink->set_pattern(sinkPattern);

            sinksInitialised = true;
            
            sinkList = {stdoutSink, fileSink, bufferSink};
        }

        _logger = std::make_shared<spdlog::logger>(name.data(), sinkList.begin(), sinkList.end());
        numLoggersAlive += 1;

        if (firstInit)
            Info("Initialised logging sinks");

        Info("Initialised logger '{0}'", _name.c_str());

        _isValid = true;
    }

    ImplLogger::~ImplLogger()
    {
        Info("Cleaning-up logger '{0}'", _name.c_str());
        std::unique_lock<std::mutex> lock(loggerInitMutex);

        numLoggersAlive -= 1;
        
        if (numLoggersAlive == 0) {
            Info("End of logs");

            sinksInitialised = false;
            
            for (auto sink : sinkList) {
                sink->flush();
                sink.reset();
            }
            sinkList.clear();
        }

        _logger.reset();
    }

    void ImplLogger::Info(const std::string& log)
    {
        _logger->info(log.data());
    }

    void ImplLogger::Warn(const std::string& log)
    {
        _logger->warn(log.data());
    }

    void ImplLogger::Error(const std::string& log)
    {
        _logger->error(log.data());
    }

    std::vector<Logger::LoggerEntry> ImplLogger::GetLogs()
    {
        std::vector<Logger::LoggerEntry> entries = bufferSink->GetLatest();
        return std::move(entries);
    }
}
