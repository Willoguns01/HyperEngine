#pragma once

#include "Engine/WidgetSystem/WidgetSystem.hpp"

#include <unordered_map>
#include <string>

namespace HyperEngine
{
    struct ImplWidgetSystem
    {
        ImplWidgetSystem() = default;
        ImplWidgetSystem(const WidgetSystemInfo& systemInfo);
        ~ImplWidgetSystem();

        std::unordered_map<std::string, std::shared_ptr<Widgets::WidgetBase>> _widgets;

        Logger _logger;

        void Update();
    };
}
