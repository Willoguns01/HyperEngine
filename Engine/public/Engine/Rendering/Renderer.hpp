#pragma once

#include "Engine/Logger.hpp"

#include <string>
#include <memory>

#include <daxa/daxa.hpp>
#include <daxa/utils/task_graph.hpp>

#include <daxa/utils/imgui.hpp>
#include <imgui.h>

// TODO: Should absolutely be able to use taskgraph with openxr
// will just need a couple extra tasks before and after xr rendering to deal with sync, start/end xr rendering

namespace HyperEngine
{
    struct GraphicsSettings
    {

    };

    struct RendererConfig
    {
        std::string_view applicationName = "HyperEngine";
        uint32_t renderWidth = 800;
        uint32_t renderHeight = 600;
        ImGuiContext* imguiContext = nullptr;
    };

    struct Resources
    {
        // gbuffer, mesh buffers, etc
    };

    struct RenderSceneInfo
    {
        // renderTarget must be capable of layout TRANSFER_DST
        // e.g. swapchain image with TRANSFER_DST image usage flag
        daxa::ImageId renderTarget;
        daxa::ImageLayout dstLayout;
        std::vector<daxa::BinarySemaphore> waitSemaphores;
        std::vector<std::pair<daxa::TimelineSemaphore, uint64_t>> waitTimelines;
        std::vector<daxa::BinarySemaphore> signalSemaphores;
        std::vector<std::pair<daxa::TimelineSemaphore, uint64_t>> signalTimelines;
    };

    struct ImplRenderer;
    class Renderer
    {
    public:
        Renderer() = default;
        Renderer(const RendererConfig& config);
        ~Renderer() = default;

        void RenderScene(const RenderSceneInfo& renderInfo);

        daxa::Instance GetInstance();
        daxa::Device GetDevice();
    };

    class Renderer
    {
    public:
        Renderer() = default;
        Renderer(const RendererConfig& config);
        ~Renderer();

        // resize e.g. gbuffers to match rendertarget size
        void ResizeRenderingResources(uint32_t width, uint32_t height);

        // commands and submits rendering lists, returns signalSemaphores
        std::vector<daxa::BinarySemaphore> RenderScene(const RenderSceneInfo& renderInfo);

        daxa::Instance GetInstance() const;
        daxa::Device GetDevice() const;

    private:
        uint32_t _renderingWidth = 0;
        uint32_t _renderingHeight = 0;

        bool _resourcesInitialised = false;
        void SetupResources(uint32_t width, uint32_t height);
        void CleanupResources();

        void Cleanup();

        Logger _logger;

        daxa::Instance _instance;
        daxa::Device _device;

        std::vector<daxa::BinarySemaphore> _renderingSignalSemaphores;
        
        // resources
        daxa::ImageId _renderingDstImage;
        Resources _resources = {};

        ImGuiContext* _imguiContext = nullptr;
        daxa::ImGuiRenderer _imguiRenderer;
    };
}
