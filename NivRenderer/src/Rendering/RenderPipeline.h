#pragma once
#include "Base.h"
#include "RenderPass.h"
#include "Rendering/Proxy/ProxyManager.h"
#include "Rendering/OpenGL/Framebuffer.h"

class RenderPipeline
{
public:
    RenderPipeline(std::vector<Ref<RenderPass>> renderPasses, std::vector<Ref<RenderPass>> postProcessingPasses, uint32_t resolutionWidth, uint32_t resolutionHeight);

    Framebuffer& Run(Ref<Scene> scene, ProxyManager& proxyManager);

    void RecompileShaders();
    void UpdateResolution(uint32_t width, uint32_t height);
    void UpdateSampleCount(uint32_t sampleCount);
    uint32_t GetSampleCount();

private:
    std::vector<Ref<RenderPass>> m_RenderPasses;
    std::vector<Ref<RenderPass>> m_PostProcessingPasses;
    Scope<Framebuffer> m_OutputFramebuffer;

    //TEST
    uint32_t m_ResolutionWidth, m_ResolutionHeight;
};