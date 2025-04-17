#include "Engine/WidgetSystem/Widgets/MenuBar.hpp"

#include "Engine/WidgetSystem/Widgets/ConsoleWidget.hpp"

#include <imgui.h>

namespace HyperEngine::Widgets
{
    MenuBar::MenuBar(WidgetSystem* widgetSystem, Renderer renderer)
    {
        _widgetSystem = widgetSystem;
        _logger = _widgetSystem->GetLogger();
        _renderer = renderer;
    }

    void MenuBar::Update()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Windows"))
            {
                if (ImGui::MenuItem("Console")) {
                    _widgetSystem->GetWidget<Widgets::ConsoleWidget>("Console")->_isOpen = true;
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Settings"))
            {
                if (ImGui::MenuItem("Vsync")) {
                    daxa::PresentMode presentMode = _renderer.GetPresentMode();
                    if (presentMode == daxa::PresentMode::IMMEDIATE) {
                        presentMode = daxa::PresentMode::FIFO_RELAXED;
                    } else {
                        presentMode = daxa::PresentMode::IMMEDIATE;
                    }
                    _renderer.SetPresentMode(presentMode);
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }
}
