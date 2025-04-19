#include "ImplWidgetSystem.hpp"

namespace HyperEngine
{  
    WidgetSystem::WidgetSystem(const WidgetSystemInfo& systemInfo)
    {
        impl = std::make_shared<ImplWidgetSystem>(systemInfo);
    }

    void WidgetSystem::AddWidget(const std::string& name, std::shared_ptr<Widgets::WidgetBase> widget)
    {
        impl->_widgets.insert(std::pair(name, widget));
    }

    std::shared_ptr<Widgets::WidgetBase> WidgetSystem::GetWidget(const std::string& name) const
    {
        return impl->_widgets.at(name);
    }

    void WidgetSystem::Update()
    {
        impl->Update();
    }

    Logger WidgetSystem::GetLogger() const
    {
        return impl->_logger;
    }

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
