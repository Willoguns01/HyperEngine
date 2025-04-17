#pragma  once

#include "Engine/WidgetSystem/WidgetBase.hpp"

#include "Engine/Rendering/Renderer.hpp"

namespace HyperEngine::Widgets
{
    struct ImplMenuBar;
    class MenuBar : public WidgetBase
    {
    public:
        MenuBar() = default;
        MenuBar(WidgetSystem* widgetSystem, Renderer renderer);
        ~MenuBar() = default;

        void Update() override;

        WidgetSystem* _widgetSystem;
        Logger _logger;

        Renderer _renderer;
    };
}
