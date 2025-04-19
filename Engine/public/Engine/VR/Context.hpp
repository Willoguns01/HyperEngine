#pragma once

#include <openxr/openxr.h>

namespace HyperEngine::VR
{
    struct ContextInfo
    {
        Logger logger;
        bool debug = false;
        std::string applicationName;
        std::string engineName;
    };

    struct ImplContext;
    class Context
    {
    public:
        Context() = default;
        Context(const ContextInfo& info);

        XrViewConfigurationType GetXRViewType() const;
        XrInstance GetXRInstance() const;
        XrSystemId GetXRSystemId() const;

        std::vector<const char*> GetDeviceExtensions() const;

    private:
        std::shared_ptr<ImplContext> _impl = nullptr;
    };
}
