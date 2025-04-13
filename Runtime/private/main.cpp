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
        
        renderer = new Renderer(RendererConfig{
            .applicationName = "HyperEngine",
            .renderWidth = 1280,
            .renderHeight = 720,
            .imguiContext = ImGui::GetCurrentContext()
        });
        
        swapchain = renderer->GetDevice().create_swapchain({
            .native_window = window.GetNativeHandle(),
            .native_window_platform = window.GetNativePlatform(),
            .surface_format_selector = daxa::default_format_score,
            .present_mode = daxa::PresentMode::IMMEDIATE,
            .present_operation = daxa::PresentOp::IDENTITY,
            .image_usage = daxa::ImageUsageFlagBits::TRANSFER_DST,
            .max_allowed_frames_in_flight = 2,
            .queue_family = daxa::QueueFamily::MAIN,
            .name = "Swapchain"
        });

        widgetSystem = WidgetSystem(WidgetSystemInfo{
            .logger = logger
        });

        widgetSystem.AddWidget<Widgets::MenuBar>("MenuBar");
        widgetSystem.AddWidget<Widgets::ConsoleWidget>("Console");
    }

    ~Application()
    {
        delete renderer;

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
                swapchain.resize();
                
                renderer->ResizeRenderingResources(window.GetWidth(), window.GetHeight());
            }
        
            daxa::ImageId swapchainImage = swapchain.acquire_next_image();
            if (swapchainImage.is_empty()) {
                continue;
            }
        
            std::vector<daxa::BinarySemaphore> renderWaitSemaphores = renderer->RenderScene({
                .renderTarget = swapchainImage,
                .dstLayout = daxa::ImageLayout::PRESENT_SRC,
                .waitSemaphores = {swapchain.current_acquire_semaphore()},
                .signalSemaphores = {swapchain.current_present_semaphore()},
                .signalTimelines = {swapchain.current_timeline_pair()}
            });
        
            renderer->GetDevice().present_frame({
                .wait_binary_semaphores = renderWaitSemaphores,
                .swapchain = swapchain,
                .queue = daxa::QUEUE_MAIN
            });
        
            renderer->GetDevice().collect_garbage();
        }
    }

private:
    Logger logger;

    Window window;

    Renderer* renderer = nullptr;
    daxa::Swapchain swapchain;

    WidgetSystem widgetSystem;
};

int main()
{
    Application* app = new Application;
    app->Run();

    delete app;

    return 0;
}
