#include "Engine/WidgetSystem/WidgetSystem.hpp"

#include  "WidgetSystem/ImplWidgetSystem.hpp"

namespace HyperEngine
{
    WidgetSystem::WidgetSystem(const WidgetSystemInfo& systemInfo)
    {
        impl = std::make_shared<ImplWidgetSystem>(systemInfo);
    }

    WidgetSystem::WidgetSystem(const WidgetSystem& other)
    {
        impl = other.impl;
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
}
