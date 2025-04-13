#include "Engine/Window.hpp"

#include "ImplWindow.hpp"

namespace HyperEngine
{
    Window::Window(const WindowInfo& info)
    {
        impl = std::make_shared<ImplWindow>(info);
    }

    Window::Window(const Window& other)
    {
        impl = other.impl;
    }

    daxa::NativeWindowHandle Window::GetNativeHandle() const {
        return impl->GetNativeHandle();
    }

    daxa::NativeWindowPlatform Window::GetNativePlatform() const {
        return impl->GetNativePlatform();
    }

    GLFWwindow* Window::GetGLFWWindow() const {
        return impl->_windowPtr;
    }

    void Window::SetMouseCapture(bool shouldCapture) const {
        impl->SetMouseCapture(shouldCapture);
    }

    bool Window::ShouldClose() const {
        return impl->ShouldClose();
    }

    void Window::Update() {
        impl->Update();
    }

    bool Window::GetSwapchainState() const {
        return impl->_swapchain_out_of_date;
    }

    uint32_t Window::GetWidth() const {
        return impl->_width;
    }

    uint32_t Window::GetHeight() const {
        return impl->_height;
    }

    bool Window::IsValid() const {
        return impl->_isValid;
    }
}
