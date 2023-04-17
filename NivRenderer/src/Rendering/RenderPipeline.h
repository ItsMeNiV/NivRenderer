#pragma once
#include "Base.h"
#include "RenderPass.h"
#include "Rendering/Proxy/ProxyManager.h"
#include "Rendering/OpenGL/Framebuffer.h"

class RenderPipeline
{
public:
    RenderPipeline(std::vector<Ref<RenderPass>> renderPasses, uint32_t resolutionWidth, uint32_t resolutionHeight, uint32_t sampleCount);

    Framebuffer& Run(Ref<Scene> scene, const ProxyManager& proxyManager);

private:
    std::vector<Ref<RenderPass>> m_RenderPasses;
    Scope<Framebuffer> m_OutputFramebuffer;

    //TEST
    uint32_t m_ResolutionWidth, m_ResolutionHeight, m_SampleCount;
};