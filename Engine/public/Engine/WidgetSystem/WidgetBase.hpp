#pragma once

#include <string>
#include <memory>

#include "Engine/WidgetSystem/WidgetSystem.hpp"

namespace HyperEngine::Widgets
{
    class WidgetBase
    {
    public:
        WidgetBase() = default;
        WidgetBase(WidgetSystem* widgetSystem, const std::string& name);
        ~WidgetBase() = default;

        WidgetSystem* GetWidgetSystem() const;
        const std::string& GetName() const;

        virtual void Update() = 0;

        bool GetOpen() const;
        void SetOpen(bool state);

    protected:
        bool _open = true;

    private:
        WidgetSystem* _widgetSystem;
        std::string _name;
    };
}
