#pragma once

#include "Engine/Rendering/Renderer.hpp"

#include <daxa/daxa.hpp>
#include <daxa/utils/task_graph.hpp>
#include <daxa/utils/imgui.hpp>

namespace HyperEngine
{
    struct Resources
    {

    };

    struct ImplRenderer
    {
        ImplRenderer() = default;
        ImplRenderer(const RendererConfig& config);
        ~ImplRenderer();

        void Resize(uint32_t width, uint32_t height);

        void RenderScene(const RenderSceneInfo& renderInfo);

        RendererConfig _initialConfig = {};
        GraphicsSettings _graphicsSettings = {};
        daxa::PresentMode _currentPresentMode = daxa::PresentMode::MAX_ENUM;

        uint32_t _renderingWidth = 0;
        uint32_t _renderingHeight = 0;
        
        Logger _logger;
        
        daxa::Instance _instance;
        daxa::Device _device;
        daxa::Swapchain _swapchain;
        
        daxa::TaskGraph _taskGraph;
        
        // resources
        daxa::TaskImage _swapchainTaskImage;
        Resources _resources = {};
        
        ImGuiContext* _imguiContext = nullptr;
        daxa::ImGuiRenderer _imguiRenderer;
        
        void SetupResources(uint32_t width, uint32_t height);
        void CleanupResources();

        void SetupTaskGraph();

        void SetPresentMode(daxa::PresentMode presentMode);
    };
}
