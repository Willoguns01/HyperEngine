#pragma once

#include "Engine/WidgetSystem/WidgetBase.hpp"

#include <functional>

namespace HyperEngine::Widgets
{
    class ConsoleWidget : public WidgetBase
    {
    public:
        typedef std::function<void(void*,const std::string&)> CommandCallbackFunc;

        ConsoleWidget() = default;
        ConsoleWidget(Logger logger);
        ~ConsoleWidget();

        void Update() override;

        void RegisterCommand(const std::string& command, CommandCallbackFunc callback, void* customData);

    private:
        Logger _engineLogger;

        std::vector<Logger::LoggerEntry> _logs;

        bool _scrollBottom = false;
        char _inputBuffer[256];

        std::unordered_map<std::string, std::pair<CommandCallbackFunc, void*>> _callbacks;

        void ExecuteCommand(char* command);

        static void HelpCommandCallback(void* data, const std::string& args);
        static void ClearCommandCallback(void* data, const std::string& args);
    };
}
