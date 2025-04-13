#include "ImplWindow.hpp"

#include <GLFW/glfw3native.h>

namespace HyperEngine
{
    ImplWindow::ImplWindow(const WindowInfo& info)
    {        
        _logger = info.logger;
        _width = info.width;
        _height = info.height;

        if (info.width <= 0 || info.height <= 0) {
            _logger.Error("Window width and height must both be some value above 0");
            return;
        }

        if (!glfwInit()) {
            _logger.Error("Failed to initialise GLFW");
            return;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

        _windowPtr = glfwCreateWindow((int)_width, (int)_height, info.name.c_str(), nullptr, nullptr);
        if (_windowPtr == nullptr) {
            _logger.Error("Failed to create GLFW window");
            return;
        }

        glfwSetWindowUserPointer(_windowPtr, this);

        glfwSetWindowSizeCallback(_windowPtr, [](GLFWwindow* window, int size_x, int size_y){
            ImplWindow* win = static_cast<ImplWindow*>(glfwGetWindowUserPointer(window));
            win->_width = (uint32_t)size_x;
            win->_height = (uint32_t)size_y;
            win->_swapchain_out_of_date = true;
        });

        _isValid = true;
    }

    ImplWindow::~ImplWindow()
    {
        glfwDestroyWindow(_windowPtr);
        glfwTerminate();
        
        _logger.Info("Cleaned-up application window");
    }

    daxa::NativeWindowHandle ImplWindow::GetNativeHandle() const
    {
#if defined(_WIN32)
        return glfwGetWin32Window(_windowPtr);
#elif defined(__linux__)
        switch(GetNativePlatform()) {
            case daxa::NativeWindowPlatform::WAYLAND_API:
                return reinterpret_cast<daxa::NativeWindowHandle>(glfwGetWaylandWindow(_windowPtr));
            case daxa::NativeWindowPlatform::XLIB_API:
            default:
                return reinterpret_cast<daxa::NativeWindowHandle>(glfwGetX11Window(_windowPtr));
        }
#endif
    }

    daxa::NativeWindowPlatform ImplWindow::GetNativePlatform() const
    {
        switch (glfwGetPlatform()) {
            case GLFW_PLATFORM_WIN32: return daxa::NativeWindowPlatform::WIN32_API;
            case GLFW_PLATFORM_X11: return daxa::NativeWindowPlatform::XLIB_API;
            case GLFW_PLATFORM_WAYLAND: return daxa::NativeWindowPlatform::WAYLAND_API;
            default: return daxa::NativeWindowPlatform::UNKNOWN;
        }
    }

    void ImplWindow::SetMouseCapture(bool shouldCapture) const {
        glfwSetCursorPos(_windowPtr, (double)(_width / 2.), (double)(_height / 2.));
        glfwSetInputMode(_windowPtr, GLFW_CURSOR, shouldCapture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        glfwSetInputMode(_windowPtr, GLFW_RAW_MOUSE_MOTION, shouldCapture);
    }

    bool ImplWindow::ShouldClose() const {
        return glfwWindowShouldClose(_windowPtr);
    }

    void ImplWindow::Update() {
        _swapchain_out_of_date = false;
        glfwPollEvents();
        glfwSwapBuffers(_windowPtr);
    }
}
