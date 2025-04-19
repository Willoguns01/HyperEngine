#include "ImplContext.hpp"

namespace HyperEngine::VR
{
    Context::Context(const ContextInfo& info)
    {
        _impl = std::make_shared<ImplContext>(info);
    }

    XrViewConfigurationType Context::GetXRViewType() const
    {
        return xrViewType;
    }

    XrInstance Context::GetXRInstance() const
    {
        return _impl->_xrInstance;
    }

    XrSystemId Context::GetXRSystemId() const
    {
        return _impl->_xrSystemId;
    }

    std::vector<const char*> Context::GetDeviceExtensions() const
    {
        return _impl->_deviceExtensions;
    }

    ImplContext::ImplContext(const ContextInfo& info)
    {
        this->info = info;

        // get all OpenXR instance extensions
        std::vector<XrExtensionProperties> supportedOpenXRInstanceExtensions;
        {
            uint32_t instanceExtensionCount;
            if (XR_FAILED(xrEnumerateInstanceExtensionProperties(nullptr, 0u, &instanceExtensionCount, nullptr))) {
                info.logger.Error("Failed enumerating instance extensions");
                return;
            }

            supportedOpenXRInstanceExtensions.resize(instanceExtensionCount);
            for (XrExtensionProperties& extensionProperty : supportedOpenXRInstanceExtensions)
            {
                extensionProperty.type = XR_TYPE_EXTENSION_PROPERTIES;
                extensionProperty.next = nullptr;
            }

            if (XR_FAILED(xrEnumerateInstanceExtensionProperties(
                nullptr,
                instanceExtensionCount,
                &instanceExtensionCount,
                supportedOpenXRInstanceExtensions.data())))
            {
                info.logger.Error("Failed enumerating instance extensions (2)");
                return;
            }
        }

        // create openxr instance
        {
            XrApplicationInfo applicationInfo {
                .applicationVersion = static_cast<uint32_t>(XR_MAKE_VERSION(0, 1, 0)),
                .engineVersion = static_cast<uint32_t>(XR_MAKE_VERSION(0, 1, 0)),
                .apiVersion = XR_CURRENT_API_VERSION
            };

            memcpy(applicationInfo.applicationName, info.applicationName.data(), info.applicationName.length() + 1u);
            memcpy(applicationInfo.engineName, info.engineName.data(), info.engineName.length() + 1u);

            std::vector<const char*> extensions = { XR_KHR_VULKAN_ENABLE_EXTENSION_NAME };

            if (info.debug) {
                extensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            // check that instance extensions are supported
            for (const char* extension : extensions)
            {
                bool extensionSupported = false;
                for (const XrExtensionProperties& supportedExtension : supportedOpenXRInstanceExtensions)
                {
                    if (strcmp(extension, supportedExtension.extensionName) == 0)
                    {
                        extensionSupported = true;
                        break;
                    }
                }

                if (!extensionSupported)
                {
                    info.logger.Error("Extension {0} not supported", extension);
                    return;
                }
            }

            XrInstanceCreateInfo instanceCreateInfo = {
                .type = XR_TYPE_INSTANCE_CREATE_INFO,
                .applicationInfo = applicationInfo,
                .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
                .enabledExtensionNames = extensions.data()
            };

            if (XR_FAILED(xrCreateInstance(&instanceCreateInfo, &_xrInstance))) {
                info.logger.Error("Failed creating instance. Is the headset connected?");
                return;
            }
        }

        // setup debug messenger
        if (info.debug)
        {
            if (!LoadExtensionFunc("xrCreateDebugUtilsMessengerEXT", 
                reinterpret_cast<PFN_xrVoidFunction*>(&xrCreateDebugUtilsMessengerEXT))) {
                return;
            }

            if (!LoadExtensionFunc("xrDestroyDebugUtilsMessengerEXT",
                reinterpret_cast<PFN_xrVoidFunction*>(&xrDestroyDebugUtilsMessengerEXT))) {
                return;
            }

            constexpr XrDebugUtilsMessageTypeFlagsEXT typeFlags =
            XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;

            constexpr XrDebugUtilsMessageSeverityFlagsEXT severityFlags =
            XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

            const auto callback = [](XrDebugUtilsMessageSeverityFlagsEXT messageSeverity,
                                    XrDebugUtilsMessageTypeFlagsEXT messageTypes,
                                    const XrDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) -> XrBool32
            {
                if (messageSeverity >= XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
                {
                    Logger* logger = static_cast<Logger*>(userData);
                    logger->Error("{0}", callbackData->message);
                }

                return XR_FALSE; // Returning XR_TRUE will force the calling function to fail
            };

            XrDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {
                .type = XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .next = nullptr,
                .messageSeverities = severityFlags,
                .messageTypes = typeFlags,
                .userCallback = callback,
                .userData = (void*)&info.logger
            };

            if (XR_FAILED(xrCreateDebugUtilsMessengerEXT(_xrInstance, &messengerCreateInfo, &_xrDebugUtilsMessenger))) {
                info.logger.Error("Failed creating debug messenger");
                return;
            }
        }

        // get system id
        XrSystemGetInfo systemGetInfo = {
            .type = XR_TYPE_SYSTEM_GET_INFO,
            .next = nullptr,
            .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY
        };

        if (XR_FAILED(xrGetSystem(_xrInstance, &systemGetInfo, &_xrSystemId))) {
            info.logger.Error("Failed finding systemId");
            return;
        }

        // check environment blend modes
        {
            uint32_t environmentBlendModeCount;
            if (XR_FAILED(xrEnumerateEnvironmentBlendModes(_xrInstance, _xrSystemId, xrViewType, 0u, &environmentBlendModeCount, nullptr))) {
                info.logger.Error("Failed enumerating environment blend modes");
                return;
            }

            std::vector<XrEnvironmentBlendMode> supportedEnvironmentBlendModes(environmentBlendModeCount);
            if (XR_FAILED(xrEnumerateEnvironmentBlendModes(
                _xrInstance,
                _xrSystemId,
                xrViewType,
                environmentBlendModeCount,
                &environmentBlendModeCount,
                supportedEnvironmentBlendModes.data())))
            {
                info.logger.Error("Failed enumerating environment blend modes (2)");
                return;
            }

            bool modeFound = false;
            for (const XrEnvironmentBlendMode& mode : supportedEnvironmentBlendModes)
            {
                if (mode == environmentBlendMode)
                {
                    modeFound = true;
                    break;
                }
            }

            if (!modeFound)
            {
                info.logger.Error("Environment blend mode not supported");
                return;
            }
        }

        if (!LoadExtensionFunc("xrGetVulkanGraphicsRequirementsKHR", 
            reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanGraphicsRequirementsKHR))) {
            return;
        }

        XrGraphicsRequirementsVulkanKHR graphicsRequirements = {
            .type = XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR,
            .next = nullptr
        };

        if (XR_FAILED(xrGetVulkanGraphicsRequirementsKHR(_xrInstance, _xrSystemId, &graphicsRequirements))) {
            info.logger.Error("Failed getting vulkan graphics requirements");
            return;
        }

        if (!LoadExtensionFunc("xrGetVulkanInstanceExtensionsKHR",
            reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanInstanceExtensionsKHR))) {
            return;
        }

        if (!LoadExtensionFunc("xrGetVulkanDeviceExtensionsKHR",
            reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanDeviceExtensionsKHR))) {
            return;
        }

        // check openxr's required vk instance and device extensions

        std::vector<const char*> vulkanInstanceExtensions;
        std::vector<const char*> vulkanDeviceExtensions;

        {
            uint32_t count;
            if (XR_FAILED(xrGetVulkanInstanceExtensionsKHR(_xrInstance, _xrSystemId, 0u, &count, nullptr))) {
                info.logger.Error("Failed enumerating vulkan instance extensions");
                return;
            }

            std::string buffer;
            buffer.resize(count);
            if (XR_FAILED(xrGetVulkanInstanceExtensionsKHR(_xrInstance, _xrSystemId, count, &count, buffer.data()))) {
                info.logger.Error("Failed enumerating vulkan instance extensions (2)");
                return;
            }

            const std::vector<const char*> instanceExtensions = UnpackExtensionString(buffer);
            for (const char* extension : instanceExtensions)
            {
                vulkanInstanceExtensions.push_back(extension);
            }
        }

        {
            uint32_t count;
            if (XR_FAILED(xrGetVulkanDeviceExtensionsKHR(_xrInstance, _xrSystemId, 0u, &count, nullptr))) {
                info.logger.Error("Failed enumerating vulkan device extensions");
                return;
            }

            std::string buffer;
            buffer.resize(count);
            if (XR_FAILED(xrGetVulkanDeviceExtensionsKHR(_xrInstance, _xrSystemId, count, &count, buffer.data()))) {
                info.logger.Error("Failed enumerating vulkan device extensions (2)");
                return;
            }

            const std::vector<const char*> deviceExtensions = UnpackExtensionString(buffer);
            for (const char* extension : deviceExtensions)
            {
                vulkanDeviceExtensions.push_back(extension);
            }
        }

        info.logger.Info("Requested instance extensions:");
        for (const char* string : vulkanInstanceExtensions) {
            info.logger.Info(" - {0}", string);
        }

        info.logger.Info("Requested device extensions:");
        for (const char* string : vulkanDeviceExtensions) {
            info.logger.Info(" - {0}", string);
        }

        _deviceExtensions = std::move(vulkanDeviceExtensions);
    }

    ImplContext::~ImplContext()
    {
        if (info.debug) {
            if (_xrDebugUtilsMessenger) {
                xrDestroyDebugUtilsMessengerEXT(_xrDebugUtilsMessenger);
            }
        }

        if (_xrInstance) {
            xrDestroyInstance(_xrInstance);
        }
    }

    bool ImplContext::LoadExtensionFunc(const char* name, PFN_xrVoidFunction* function)
    {
        if (XR_FAILED(xrGetInstanceProcAddr(_xrInstance, name, function))) {
            info.logger.Error("Failed loading extension {0}", name);
            return false;
        }
        return true;
    }

    std::vector<const char*> ImplContext::UnpackExtensionString(const std::string& string)
    {
        std::vector<const char*> out;
        std::istringstream stream(string);
        std::string extension;
        while (std::getline(stream, extension, ' '))
        {
            const size_t len = extension.size() + 1u;
            char* str = new char[len];
            memcpy(str, extension.c_str(), len);
            out.push_back(str);
        }
        return out;
    }
}
