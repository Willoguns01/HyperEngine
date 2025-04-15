#include "Engine/WidgetSystem/Widgets/MenuBar.hpp"

#include <imgui.h>

namespace HyperEngine::Widgets
{
    MenuBar::MenuBar(WidgetSystem* widgetSystem, const std::string& name)
        : WidgetBase(widgetSystem, name)
    {
        WidgetBase::GetWidgetSystem()->GetLogger().Info("Initialised menu bar 'widget'");
    }

    MenuBar::~MenuBar()
    {
        WidgetBase::GetWidgetSystem()->GetLogger().Info("Cleaned-up menu bar 'widget'");
    }

    void MenuBar::Update()
    {
        WidgetSystem* widgetSystem = WidgetBase::GetWidgetSystem();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Windows"))
            {
                if (ImGui::MenuItem("Console")) {
                    widgetSystem->GetWidget("Console")->SetOpen("True");
                }

                ImGui::EndMenu();
            }

            static bool vsyncEnable = true;

            if (ImGui::BeginMenu("Settings"))
            {
                if (ImGui::MenuItem("Vsync")) {
                    vsyncEnable = !vsyncEnable;
                    daxa::PresentMode presentMode;
                    if (vsyncEnable) {
                        presentMode = daxa::PresentMode::FIFO_RELAXED;
                    } else {
                        presentMode = daxa::PresentMode::IMMEDIATE;
                    }

                    GraphicsSettings settings = renderer->GetSettings();
                    settings.presentMode = presentMode;
                    renderer->ApplySettings(settings);
                }
            }

            ImGui::EndMainMenuBar();
        }
    }
}
