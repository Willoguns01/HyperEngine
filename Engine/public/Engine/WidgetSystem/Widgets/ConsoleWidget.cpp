#include "Engine/WidgetSystem/Widgets/ConsoleWidget.hpp"

#include "Engine/Logger.hpp"

#include <imgui.h>

namespace HyperEngine::Widgets
{
    constexpr float partialNum = 0.5f;
    constexpr ImVec4 infoColour = ImVec4{partialNum, 1.0f, partialNum, 1.0f};
    constexpr ImVec4 warnColour = ImVec4{1.0f, 1.0f, partialNum, 1.0f};
    constexpr ImVec4 errorColour = ImVec4{1.0f, partialNum, partialNum, 1.0f};
    
    ConsoleWidget::ConsoleWidget(Logger logger)
    {
        RegisterCommand("help", HelpCommandCallback, this);
        RegisterCommand("clear", ClearCommandCallback, this);
    
        memset(_inputBuffer, 0, sizeof(_inputBuffer));
    
        _engineLogger = logger;
        _engineLogger.Info("Initialised logger widget");
    }

    ConsoleWidget::~ConsoleWidget()
    {
        _engineLogger.Info("Cleaned-up logger widget");
    }

    void ConsoleWidget::Update()
    {
        std::vector<Logger::LoggerEntry> entries = _engineLogger.GetLogs();
        for (auto& entry : entries) {
            _logs.push_back(entry);
        }
    
        if (_isOpen)
        {
            ImGui::Begin("Console", &_isOpen);

            const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
            ImGui::BeginChild("LoggerChild", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar);

            ImVec4 textColour = infoColour;
            for (const auto& entry : _logs)
            {
                switch(entry.level)
                {
                    case spdlog::level::info:
                    textColour = infoColour;
                    break;
                    case spdlog::level::warn:
                    textColour = warnColour;
                    break;
                    case spdlog::level::err:
                    textColour = errorColour;
                    break;
                }
                
                ImGui::TextColored(textColour, entry.str.c_str());
            }

            if (_scrollBottom || ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
            
            ImGui::EndChild();

            // Command-line
#ifdef NDEBUG
            bool reclaim_focus = false;
            ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
            if (ImGui::InputText("Input", _inputBuffer, IM_ARRAYSIZE(_inputBuffer), input_text_flags, nullptr, (void*)this))
            {
                char* s = _inputBuffer;
                if (s[0])
                    ExecuteCommand(s);
                strcpy_s(s, 1, "");
                reclaim_focus = true;
            }

            // Auto-focus on window apparition
            ImGui::SetItemDefaultFocus();
            if (reclaim_focus)
                ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
#else
            // TODO: what actually causes this?
            // memory access error when clicking on the input box
            ImGui::TextColored(errorColour, "Application was built in debug configuration, a current bug will cause a crash if using console commands");
#endif

            ImGui::End();
        }
    }

    void ConsoleWidget::RegisterCommand(const std::string& command, CommandCallbackFunc callback, void* customData)
    {
        _callbacks.insert(std::pair(command, std::pair(callback, customData)));
    }

    void ConsoleWidget::ExecuteCommand(char* command)
    {
        std::string command_s = std::string(command);

        // TODO: command arguments for e.g. setting rendering config from commandline

        // TODO: commands should try automatically resolve capitalisation

        auto it = _callbacks.find(command_s);
        if (it != _callbacks.end())
        {
            _engineLogger.Warn("Execute command '{0}'", command);
            it->second.first(it->second.second, "");
        }
        else
        {
            _engineLogger.Error("Command '{0}' does not exist", command);
        }
    }

    void ConsoleWidget::HelpCommandCallback(void* data, const std::string& args)
    {
        ConsoleWidget* widget = static_cast<ConsoleWidget*>(data);

        widget->_engineLogger.Info("Available commands:");

        // TODO: split entries into pages, print separate pages depending on supplied arg
        for (auto& entry : widget->_callbacks) {
            widget->_engineLogger.Info(" - {0}", entry.first.c_str());
        }
    }

    void ConsoleWidget::ClearCommandCallback(void* data, const std::string& args)
    {
        ConsoleWidget* widget = static_cast<ConsoleWidget*>(data);
        widget->_logs.clear();
    }
}
