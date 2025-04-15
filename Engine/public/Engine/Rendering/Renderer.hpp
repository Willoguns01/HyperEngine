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
    struct RendererConfig
    {
        std::string_view applicationName = "HyperEngine";
        uint32_t renderWidth = 800;
        uint32_t renderHeight = 600;
        daxa::NativeWindowHandle windowHandle = nullptr;
        daxa::NativeWindowPlatform windowPlatform = daxa::NativeWindowPlatform::UNKNOWN;
        daxa::PresentMode presentMode = daxa::PresentMode::FIFO_RELAXED;
        ImGuiContext* imguiContext = nullptr;
    };

    struct GraphicsSettings
    {
        daxa::PresentMode presentMode = daxa::PresentMode::FIFO_RELAXED;
    };

    struct RenderSceneInfo
    {

    };

    struct ImplRenderer;
    class Renderer
    {
    public:
        Renderer() = default;
        Renderer(const RendererConfig& config);
        ~Renderer() = default;

        void Resize(uint32_t width, uint32_t height);

        void RenderScene(const RenderSceneInfo& renderInfo);

        GraphicsSettings GetSettings() const;
        void ApplySettings(const GraphicsSettings& settings);

        daxa::Instance GetInstance();
        daxa::Device GetDevice();

    private:
        std::shared_ptr<ImplRenderer> _impl = nullptr;
    };
}
