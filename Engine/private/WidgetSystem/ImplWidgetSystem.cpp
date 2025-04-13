#include "ImplWidgetSystem.hpp"

#include "Engine/WidgetSystem/WidgetBase.hpp"

namespace HyperEngine
{  
    ImplWidgetSystem::ImplWidgetSystem(const WidgetSystemInfo& systemInfo)
    {
        _logger = systemInfo.logger;

        _logger.Info("Initialised widget system");
    }

    ImplWidgetSystem::~ImplWidgetSystem()
    {
        for (auto& it : _widgets) {
            it.second.reset();
        }
        _widgets.clear();
        
        _logger.Info("Cleaned-up widget system");
    }

    void ImplWidgetSystem::Update()
    {
        for (auto& it : _widgets)
        {
            it.second->Update();
        }
    }
}
