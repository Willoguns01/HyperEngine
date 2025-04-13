#pragma once

#include "Engine/Logger.hpp"

#include <string>
#include <memory>

namespace HyperEngine
{
    namespace Widgets {
        class WidgetBase;
    }

    struct WidgetSystemInfo {
        Logger logger;
    };

    struct ImplWidgetSystem;
    class WidgetSystem
    {
    public:
        WidgetSystem() = default;
        WidgetSystem(const WidgetSystemInfo& systemInfo);
        WidgetSystem(const WidgetSystem& other);
        ~WidgetSystem() = default;

        template <typename T>
        void AddWidget(const std::string& name)
        {
            // TODO: do we need to check the type inherits from WidgetBase?
            AddWidget(name, std::make_shared<T>(this, name));
        }

        void AddWidget(const std::string& name, std::shared_ptr<Widgets::WidgetBase> widget);

        template <typename T>
        std::shared_ptr<T> GetWidget(const std::string& name) const
        {
            return static_cast<std::shared_ptr<T>>(GetWidget(name));
        }

        std::shared_ptr<Widgets::WidgetBase> GetWidget(const std::string& name) const;

        void Update();

        Logger GetLogger() const;

    private:
        std::shared_ptr<ImplWidgetSystem> impl = nullptr;
    };
}
