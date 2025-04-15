#include <Engine/Logger.hpp>
#include <Engine/Window.hpp>
#include <Engine/Rendering/Renderer.hpp>

#include <Engine/WidgetSystem/WidgetSystem.hpp>
#include <Engine/WidgetSystem/Widgets/ConsoleWidget.hpp>
#include <Engine/WidgetSystem/Widgets/MenuBar.hpp>

#include <memory>
#include <iostream>

#include <daxa/utils/imgui.hpp>
#include <imgui_impl_glfw.h>

using namespace HyperEngine;

class Application
{
public:
    Application()
    {
        logger = Logger("HyperEngine");

        logger.Warn("This is a warning");
        logger.Error("This is an error");
        
        window = Window(WindowInfo{
            .width = 1280,
            .height = 720,
            .name = "HyperEngine",
            .logger = logger
        });

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        // this should be user-configurable
        io.FontGlobalScale = 1.5f;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(window.GetGLFWWindow(), true);
        
        renderer = Renderer(RendererConfig{
            .applicationName = "HyperEngine",
            .renderWidth = 1280,
            .renderHeight = 720,
            .windowHandle = window.GetNativeHandle(),
            .windowPlatform = window.GetNativePlatform(),
            .presentMode = daxa::PresentMode::IMMEDIATE,
            .imguiContext = ImGui::GetCurrentContext()
        });

        widgetSystem = WidgetSystem(WidgetSystemInfo{
            .logger = logger
        });

        // TODO: need to rethink widgets, maybe turn into pimpl classes
        
        widgetSystem.AddWidget<Widgets::MenuBar>("MenuBar");
        widgetSystem.AddWidget<Widgets::ConsoleWidget>("Console");
    }

    ~Application()
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Run()
    {
        while (!window.ShouldClose())
        {
            window.Update();

            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            widgetSystem.Update();

            if (window.GetSwapchainState()) {
                renderer.Resize(window.GetWidth(), window.GetHeight());
            }

            renderer.RenderScene({});
        }
    }

private:
    Logger logger;
    Window window;
    Renderer renderer;
    WidgetSystem widgetSystem;
};

int main()
{
    Application* app = new Application;
    app->Run();

    delete app;

    return 0;
}
