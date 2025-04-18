#include "Engine/Rendering/Renderer.hpp"
#include "ImplRenderer.hpp"

namespace HyperEngine
{
    Renderer::Renderer(const RendererConfig& config)
    {
        _impl = std::make_shared<ImplRenderer>(config);
    }

    void Renderer::Resize(uint32_t width, uint32_t height)
    {
        _impl->Resize(width, height);
    }

    void Renderer::RenderScene(const RenderSceneInfo& renderInfo)
    {
        _impl->RenderScene(renderInfo);
    }

    GraphicsSettings Renderer::GetSettings() const {
        return _impl->_graphicsSettings;
    }

    void Renderer::ApplySettings(const GraphicsSettings& settings)
    {

    }

    void Renderer::SetPresentMode(daxa::PresentMode presentMode)
    {
        _impl->SetPresentMode(presentMode);
    }

    daxa::PresentMode Renderer::GetPresentMode() const {
        return _impl->_currentPresentMode;
    }

    daxa::Instance Renderer::GetInstance() {
        return _impl->_instance;
    }

    daxa::Device Renderer::GetDevice() {
        return _impl->_device;
    }

    ImplRenderer::ImplRenderer(const RendererConfig& config)
    {
        _logger = Logger("Renderer");
        if (!_logger.IsValid()) {
            return;
        }
        _logger.Info("Initialising Renderer");

        _initialConfig = config;
        _renderingWidth = config.renderWidth;
        _renderingHeight = config.renderHeight;

        _currentPresentMode = config.presentMode;

        _instance = config.instance;
        _device = config.device;

        _swapchain = _device.create_swapchain({
            .native_window = config.windowHandle,
            .native_window_platform = config.windowPlatform,
            .surface_format_selector = daxa::default_format_score,
            .present_mode = config.presentMode,
            .present_operation = daxa::PresentOp::IDENTITY,
            .image_usage = daxa::ImageUsageFlagBits::COLOR_ATTACHMENT,
            .max_allowed_frames_in_flight = 2,
            .queue_family = daxa::QueueFamily::MAIN,
            .name = "Swapchain"
        });

        SetupResources(config.renderWidth, config.renderHeight);
        SetupTaskGraph();

        if (config.imguiContext != nullptr)
        {
            _imguiContext = config.imguiContext;
            _imguiRenderer = daxa::ImGuiRenderer(daxa::ImGuiRendererInfo{
                .device = _device,
                .format = daxa::Format::R8G8B8A8_UNORM,
                .context = _imguiContext
            });
        }

        _logger.Info("Initialised Renderer");
    }

    ImplRenderer::~ImplRenderer()
    {
        CleanupResources();
        _logger.Info("Cleaned-up Renderer");
    }

    void ImplRenderer::Resize(uint32_t width, uint32_t height)
    {
        _swapchain.resize();
        _renderingWidth = width;
        _renderingHeight = height;
        SetupResources(width, height);
        _logger.Info("Resized to {0}x{1}", width, height);
    }

    void ImplRenderer::RenderScene(const RenderSceneInfo& renderInfo)
    {
        daxa::ImageId swapchainImage = _swapchain.acquire_next_image();
        if (swapchainImage.is_empty()) {
            return;
        }

        _swapchainTaskImage.set_images({.images = std::span{&swapchainImage, 1}});

        _taskGraph.execute({});

        _device.collect_garbage();
    }

    void ImplRenderer::SetupResources(uint32_t width, uint32_t height)
    {

    }

    void ImplRenderer::CleanupResources()
    {
        
    }

    void ImplRenderer::SetupTaskGraph()
    {
        _taskGraph = daxa::TaskGraph({
            .device = _device,
            .swapchain = _swapchain,
            .name = "taskgraph"
        });

        _swapchainTaskImage = daxa::TaskImage{{.swapchain_image = true, .name = "swapchain image"}};
        _taskGraph.use_persistent_image(_swapchainTaskImage);

        _taskGraph.add_task({
            .attachments = {
                daxa::inl_attachment(daxa::TaskImageAccess::COLOR_ATTACHMENT, daxa::ImageViewType::REGULAR_2D, _swapchainTaskImage)
            },
            .task = [=](daxa::TaskInterface ti)
            {
                auto const size = ti.device.info(ti.get(_swapchainTaskImage).ids[0]).value().size;

                daxa::RenderCommandRecorder render_recorder = std::move(ti.recorder).begin_renderpass({
                    .color_attachments = std::array{
                        daxa::RenderAttachmentInfo{
                            .image_view = ti.get(_swapchainTaskImage).view_ids[0],
                            .load_op = daxa::AttachmentLoadOp::CLEAR,
                            .clear_value = std::array<daxa::f32, 4>{0.1f, 0.1f, 0.1f, 1.0f}
                        }
                    },
                    .render_area = {.width = size.x, .height = size.y}
                });

                ti.recorder = std::move(render_recorder).end_renderpass();

                if (_imguiContext)
                {
                    ImGui::Render();
                    _imguiRenderer.record_commands(ImGui::GetDrawData(), ti.recorder, ti.get(_swapchainTaskImage).ids[0], size.x, size.y);
                }
            },
            .name = "clear & draw imgui"
        });

        _taskGraph.submit({});
        _taskGraph.present({});
        _taskGraph.complete({});
    }

    void ImplRenderer::SetPresentMode(daxa::PresentMode presentMode)
    {
        _swapchain.set_present_mode(presentMode);
        _currentPresentMode = presentMode;
    }
}
