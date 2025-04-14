#include "Engine/Rendering/Renderer.hpp"

#include "Rendering/ImplRenderer.hpp"

/*

should definitely be able to use taskgraph, with conditionals to determine whether to render VR, to viewport image, imgui, etc.

*/

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

    daxa::Instance Renderer::GetInstance() {
        return _impl->_instance;
    }

    daxa::Device Renderer::GetDevice() {
        return _impl->_device;
    }
}
