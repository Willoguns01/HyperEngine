#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "Engine/WidgetSystem/WidgetBase.hpp"

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
