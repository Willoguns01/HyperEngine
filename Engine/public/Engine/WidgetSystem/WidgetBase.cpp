#pragma once

#include "Engine/WidgetSystem/WidgetBase.hpp"

namespace HyperEngine::Widgets
{
    WidgetBase::WidgetBase(WidgetSystem* widgetSystem, const std::string& name)
    {
        _widgetSystem = widgetSystem;
        _name = name;
    }

    WidgetSystem* WidgetBase::GetWidgetSystem() const {
        return _widgetSystem;
    }

    const std::string& WidgetBase::GetName() const {
        return _name;
    }

    bool WidgetBase::GetOpen() const {
        return _open;
    }

    void WidgetBase::SetOpen(bool state) {
        _open = state;
    }
}
