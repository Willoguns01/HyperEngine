#pragma once

#include "Engine/Logger.hpp"

#include <string>
#include <memory>

#include "Engine/WidgetSystem/WidgetBase.hpp"

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
        ~WidgetSystem() = default;

        template <typename T, typename... Args>
        void AddWidget(const std::string& name, Args... args)
        {
            AddWidget(name, std::make_shared<T>(args...));
        }

        void AddWidget(const std::string& name, std::shared_ptr<Widgets::WidgetBase> widget);

        template <typename T>
        std::shared_ptr<T> GetWidget(const std::string& name) const
        {
            return std::dynamic_pointer_cast<T>(GetWidget(name));
        }

        std::shared_ptr<Widgets::WidgetBase> GetWidget(const std::string& name) const;

        void Update();

        Logger GetLogger() const;

    private:
        std::shared_ptr<ImplWidgetSystem> impl = nullptr;
    };
}
