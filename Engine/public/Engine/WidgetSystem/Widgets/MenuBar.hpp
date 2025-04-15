#pragma  once

#include "Engine/WidgetSystem/WidgetBase.hpp"

#include "Engine/Rendering/Renderer.hpp"

namespace HyperEngine::Widgets
{
    class MenuBar : public WidgetBase
    {
    public:
        MenuBar() = default;
        MenuBar(WidgetSystem* widgetSystem, const std::string& name);
        ~MenuBar();

        void Update() override;

        Renderer* renderer = nullptr;
    };
}
