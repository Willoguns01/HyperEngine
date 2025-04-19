#pragma once

#include "Engine/Window.hpp"

namespace HyperEngine
{
    struct ImplWindow
    {
        ImplWindow() = default;
        ImplWindow(const WindowInfo& info);
        ~ImplWindow();

        daxa::NativeWindowHandle GetNativeHandle() const;
        daxa::NativeWindowPlatform GetNativePlatform() const;

        GLFWwindow* GetGLFWWindow() const;

        void SetMouseCapture(bool shouldCapture) const;
        bool ShouldClose() const;
        void Kill();
        void Update();

        Logger _logger;

        bool _swapchain_out_of_date = false;
        uint32_t _width = 0;
        uint32_t _height = 0;

        GLFWwindow* _windowPtr = nullptr;
        bool _minimised = false;

        bool _isValid = false;
    };
}
