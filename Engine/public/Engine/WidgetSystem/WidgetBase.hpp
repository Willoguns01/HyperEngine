#pragma once

#include <memory>

#include "Engine/WidgetSystem/WidgetSystem.hpp"

namespace HyperEngine::Widgets
{
    class WidgetBase
    {
    public:
        WidgetBase() = default;
        ~WidgetBase() = default;

        virtual void Update() = 0;

        bool _isOpen = true;
    };
}
