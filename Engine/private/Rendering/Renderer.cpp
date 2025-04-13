#include "Engine/Rendering/Renderer.hpp"

namespace HyperEngine
{
    Renderer::Renderer(const RendererConfig& config)
    {
        _logger = Logger("Renderer");
        if (!_logger.IsValid()) {
            return;
        }
        _logger.Info("Initialising Renderer");

        _renderingWidth = config.renderWidth;
        _renderingHeight = config.renderHeight;
        
        _instance = daxa::create_instance({
            .flags = daxa::InstanceFlagBits::DEBUG_UTILS | daxa::InstanceFlagBits::PARENT_MUST_OUTLIVE_CHILD, 
            .engine_name = config.applicationName, 
            .app_name = config.applicationName,
        });
        
        _device = _instance.create_device_2(_instance.choose_device(daxa::ImplicitFeatureFlagBits::NONE, {
            .explicit_features = daxa::ExplicitFeatureFlagBits::NONE,
            .name = "Device"
        }));

        SetupResources(config.renderWidth, config.renderHeight);
        
        if (config.imguiContext != nullptr)
        {
            _imguiContext = config.imguiContext;
            _imguiRenderer = daxa::ImGuiRenderer(daxa::ImGuiRendererInfo{
                .device = _device,
                .format = daxa::Format::R8G8B8A8_UNORM,
                .context = _imguiContext,
            });
        }

        _logger.Info("Initialised Renderer for application {0}", config.applicationName.data());
    }

    Renderer::~Renderer()
    {
        Cleanup();
    }

    void Renderer::ResizeRenderingResources(uint32_t width, uint32_t height)
    {
        _logger.Info("Begin resize to {0}x{1}", width, height);
        _renderingWidth = width;
        _renderingHeight = height;
        SetupResources(width, height);
        _logger.Info("Finished resizing");
    }

    std::vector<daxa::BinarySemaphore> Renderer::RenderScene(const RenderSceneInfo& renderInfo)
    {
        // unsure if needed, will wait and see
        std::vector<daxa::BinarySemaphore> additionalSignalSemaphores = _renderingSignalSemaphores;
        additionalSignalSemaphores.insert(additionalSignalSemaphores.end(), renderInfo.signalSemaphores.begin(), renderInfo.signalSemaphores.end());

        std::vector<daxa::ExecutableCommandList> cmdLists;

        // temp
        {
            daxa::CommandRecorder recorder = _device.create_command_recorder({.queue_family = daxa::QueueFamily::MAIN, .name = "Render Recorder"});

            recorder.pipeline_barrier_image_transition({
                .src_access = daxa::AccessConsts::COPY_READ,
                .dst_access = daxa::AccessConsts::COLOR_ATTACHMENT_OUTPUT_WRITE,
                .src_layout = daxa::ImageLayout::TRANSFER_SRC_OPTIMAL,
                .dst_layout = daxa::ImageLayout::ATTACHMENT_OPTIMAL,
                .image_id = _renderingDstImage
            });

            daxa::RenderCommandRecorder renderRecorder = std::move(recorder).begin_renderpass({
                .color_attachments = std::array{
                    daxa::RenderAttachmentInfo{
                        .image_view = _renderingDstImage.default_view(),
                        .load_op = daxa::AttachmentLoadOp::CLEAR,
                        .clear_value = std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f}
                    }
                },
                .render_area = {.width = _renderingWidth, .height = _renderingHeight}
            });

            recorder = std::move(renderRecorder).end_renderpass();

            recorder.pipeline_barrier_image_transition({
                .src_access = daxa::AccessConsts::COLOR_ATTACHMENT_OUTPUT_WRITE,
                .dst_access = daxa::AccessConsts::COPY_READ,
                .src_layout = daxa::ImageLayout::ATTACHMENT_OPTIMAL,
                .dst_layout = daxa::ImageLayout::TRANSFER_SRC_OPTIMAL,
                .image_id = _renderingDstImage
            });

            recorder.pipeline_barrier_image_transition({
                .src_access = daxa::AccessConsts::READ,
                .dst_access = daxa::AccessConsts::COPY_WRITE,
                .src_layout = daxa::ImageLayout::UNDEFINED,
                .dst_layout = daxa::ImageLayout::TRANSFER_DST_OPTIMAL,
                .image_id = renderInfo.renderTarget
            });

            recorder.copy_image_to_image({
                .src_image = _renderingDstImage,
                .dst_image = renderInfo.renderTarget,
                .src_offset = {0, 0, 0},
                .dst_offset = {0, 0, 0},
                .extent = daxa::Extent3D{_renderingWidth, _renderingHeight, 1}
            });

            if (_imguiContext != nullptr)
            {
                ImGui::Render();
                _imguiRenderer.record_commands(ImGui::GetDrawData(), recorder, renderInfo.renderTarget, _renderingWidth, _renderingHeight);
            }

            recorder.pipeline_barrier_image_transition({
                .src_access = daxa::AccessConsts::COPY_WRITE,
                .dst_access = daxa::AccessConsts::READ,
                .src_layout = daxa::ImageLayout::TRANSFER_DST_OPTIMAL,
                .dst_layout = renderInfo.dstLayout,
                .image_id = renderInfo.renderTarget
            });

            cmdLists.push_back(recorder.complete_current_commands());
        }

        _device.submit_commands({
            .wait_stages = daxa::PipelineStageFlagBits::ALL_COMMANDS,
            .command_lists = cmdLists,
            .wait_binary_semaphores = renderInfo.waitSemaphores,
            .signal_binary_semaphores = additionalSignalSemaphores,
            .wait_timeline_semaphores = renderInfo.waitTimelines,
            .signal_timeline_semaphores = renderInfo.signalTimelines
        });

        return additionalSignalSemaphores;
    }

    daxa::Instance Renderer::GetInstance() const {
        return _instance;
    }

    daxa::Device Renderer::GetDevice() const {
        return _device;
    }

    void Renderer::SetupResources(uint32_t width, uint32_t height)
    {
        _device.wait_idle();

        if (_resourcesInitialised) {
            CleanupResources();
        }

        _renderingDstImage = _device.create_image({
            .size = {width, height, 1},
            .usage = daxa::ImageUsageFlagBits::COLOR_ATTACHMENT | daxa::ImageUsageFlagBits::TRANSFER_SRC,
            .allocate_info = daxa::MemoryFlagBits::NONE,
            .name = "Rendering Dst Image"
        });

        _logger.Info("Setup Renderer resources");
        _resourcesInitialised = true;
    }

    void Renderer::CleanupResources()
    {
        _device.destroy_image(_renderingDstImage);

        _logger.Info("Cleaned-up Renderer resources");
    }

    void Renderer::Cleanup()
    {
        CleanupResources();
        _logger.Info("Cleaned-up Renderer");
    }
}
