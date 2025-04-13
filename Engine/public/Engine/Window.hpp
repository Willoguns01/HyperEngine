#pragma once

#include "Engine/Logger.hpp"

#include <daxa/daxa.hpp>

#include <GLFW/glfw3.h>
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_NATIVE_INCLUDE_NONE
using HWND = void*;
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif

#include <string>

namespace HyperEngine
{
    struct WindowInfo {
        uint32_t width = 0;
        uint32_t height = 0;
        std::string name;
        Logger logger;
    };

    struct ImplWindow;
    class Window
    {
    public:
        Window() = default;
        Window(const WindowInfo& info);
        Window(const Window& other);
        ~Window() = default;

        daxa::NativeWindowHandle GetNativeHandle() const;
        daxa::NativeWindowPlatform GetNativePlatform() const;

        GLFWwindow* GetGLFWWindow() const;

        void SetMouseCapture(bool shouldCapture) const;
        bool ShouldClose() const;
        void Update();

        bool GetSwapchainState() const;
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;

        bool IsValid() const;

    private:
        std::shared_ptr<ImplWindow> impl = nullptr;
    };
}
