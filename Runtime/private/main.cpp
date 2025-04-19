#include <Engine/Logger.hpp>
#include <Engine/Window.hpp>
#include <Engine/Rendering/Renderer.hpp>

#include <Engine/WidgetSystem/WidgetSystem.hpp>
#include <Engine/WidgetSystem/Widgets/ConsoleWidget.hpp>
#include <Engine/WidgetSystem/Widgets/MenuBar.hpp>

#include <Engine/VR/Context.hpp>
#include <daxa/utils/openxr.hpp>

#include <memory>
#include <iostream>

#include <daxa/utils/imgui.hpp>
#include <imgui_impl_glfw.h>

using namespace HyperEngine;

static void XrErrorCallback(const char* out, void* data)
{
    Logger* logger = static_cast<Logger*>(data);
    logger->Error("{0}", out);
}

class Application
{
public:
    Application()
    {
        logger = Logger("HyperEngine");
        
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

        // initial vr stuff
        Logger xrLogger("OpenXR");
        vrContext = VR::Context(VR::ContextInfo{
            .logger = xrLogger,
            .debug = true,
            .applicationName = "HyperEngine",
            .engineName = "HyperEngine"
        });

        daxa::Instance instance = daxa::create_instance({
            .engine_name = "HyperEngine",
            .app_name = "HyperEngine",
            .customDeviceExtensions = vrContext.GetDeviceExtensions()
        });

        daxa::Device device = instance.create_device_2(instance.choose_device(
            daxa::ImplicitFeatureFlagBits::SWAPCHAIN,
            {
                .name = "Device"
            }
        ));

        renderer = Renderer(RendererConfig{
            .instance = instance,
            .device = device,
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

        widgetSystem.AddWidget<Widgets::MenuBar>("MenuBar", &widgetSystem, renderer);
        widgetSystem.AddWidget<Widgets::ConsoleWidget>("Console", logger);

        //  vr stuff
        vrHeadset = daxa::OpenXRHeadset(daxa::OpenXRHeadsetInfo {
            .instance = renderer.GetInstance(),
            .device = renderer.GetDevice(),
            .xrInstance = vrContext.GetXRInstance(),
            .xrSystemId = vrContext.GetXRSystemId(),
            .xrViewType = vrContext.GetXRViewType(),
            .errorCallback = XrErrorCallback,
            .customData = &xrLogger
        });
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
            if (vrHeadset.ExitRequested()) {
                window.Kill();
                continue;
            }

            window.Update();

            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            widgetSystem.Update();

            if (window.GetSwapchainState()) {
                renderer.Resize(window.GetWidth(), window.GetHeight());
            }

            renderer.RenderScene({});

            uint32_t swapchainImageIndex;
            daxa::BeginFrameResult result = vrHeadset.BeginFrame(swapchainImageIndex);
            if (result == daxa::BeginFrameResult::RENDER)
            {
                daxa::ImageId dstImage = vrHeadset.GetRenderTarget(swapchainImageIndex);
                vrHeadset.EndFrame();
            }
            else if (result == daxa::BeginFrameResult::ERROR)
            {
                logger.Warn("Encountered OpenXR error when beginning frame");
            }
        }

        renderer.GetDevice().wait_idle();
        renderer.GetDevice().collect_garbage();
    }

private:
    Logger logger;
    Window window;
    Renderer renderer;
    WidgetSystem widgetSystem;

    VR::Context vrContext;
    daxa::OpenXRHeadset vrHeadset;
};

int main()
{
    Application* app = new Application;
    app->Run();

    delete app;

    return 0;
}
