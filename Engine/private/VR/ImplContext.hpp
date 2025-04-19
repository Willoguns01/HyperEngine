#pragma once

#include "Engine/VR/Context.hpp"

namespace HyperEngine::VR
{
    constexpr XrViewConfigurationType xrViewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    constexpr XrEnvironmentBlendMode environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;

    struct ImplContext
    {
        ContextInfo info = {};

        XrInstance _xrInstance = nullptr;
        XrSystemId _xrSystemId = 0u;

        PFN_xrCreateDebugUtilsMessengerEXT xrCreateDebugUtilsMessengerEXT = nullptr;
        PFN_xrDestroyDebugUtilsMessengerEXT xrDestroyDebugUtilsMessengerEXT = nullptr;
        XrDebugUtilsMessengerEXT _xrDebugUtilsMessenger = nullptr;

        PFN_xrGetVulkanGraphicsRequirementsKHR xrGetVulkanGraphicsRequirementsKHR = nullptr;
        PFN_xrGetVulkanInstanceExtensionsKHR xrGetVulkanInstanceExtensionsKHR = nullptr;
        PFN_xrGetVulkanDeviceExtensionsKHR xrGetVulkanDeviceExtensionsKHR = nullptr;

        std::vector<const char*> _deviceExtensions = {};

        ImplContext() = default;
        ImplContext(const ContextInfo& info);
        ~ImplContext();

        bool LoadExtensionFunc(const char* name, PFN_xrVoidFunction* function);

        std::vector<const char*> UnpackExtensionString(const std::string& string);
    };
}
