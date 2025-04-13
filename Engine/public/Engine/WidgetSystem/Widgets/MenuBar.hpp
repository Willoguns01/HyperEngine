#pragma  once

#include "Engine/WidgetSystem/WidgetBase.hpp"

namespace HyperEngine::Widgets
{
    class MenuBar : public WidgetBase
    {
    public:
        MenuBar() = default;
        MenuBar(WidgetSystem* widgetSystem, const std::string& name);
        ~MenuBar();

        void Update() override;
    };
}
