#include "RenderPipeline.h"

RenderPipeline::RenderPipeline(std::vector<Ref<RenderPass>> renderPasses, uint32_t resolutionWidth, uint32_t resolutionHeight, uint32_t sampleCount)
    : m_RenderPasses(renderPasses), m_OutputFramebuffer(nullptr), m_ResolutionWidth(resolutionWidth), m_ResolutionHeight(resolutionHeight), m_SampleCount(sampleCount)
{
}

Framebuffer& RenderPipeline::Run(Ref<Scene> scene, ProxyManager& proxyManager)
{
    if (!m_OutputFramebuffer)
    {
        m_OutputFramebuffer = CreateScope<Framebuffer>(m_ResolutionWidth, m_ResolutionHeight, m_SampleCount);
    }

    for (auto currentPass : m_RenderPasses)
    {
        //Map input
        currentPass->Run(scene, proxyManager);
        //Get output

        //If last pass:
        currentPass->GetOutputFramebuffer()->BlitFramebuffer(m_OutputFramebuffer->GetId());
    }

    return *m_OutputFramebuffer;
}