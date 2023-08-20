#include "RenderPipeline.h"

RenderPipeline::RenderPipeline(std::vector<Ref<RenderPass>> renderPasses, std::vector<Ref<RenderPass>> postProcessingPasses, uint32_t resolutionWidth, uint32_t resolutionHeight)
    : m_RenderPasses(renderPasses), m_PostProcessingPasses(postProcessingPasses), m_OutputFramebuffer(nullptr), m_ResolutionWidth(resolutionWidth), m_ResolutionHeight(resolutionHeight)
{
}

Framebuffer& RenderPipeline::Run(Ref<Scene> scene, ProxyManager& proxyManager)
{
    if (!m_OutputFramebuffer)
    {
        m_OutputFramebuffer = CreateScope<Framebuffer>(m_ResolutionWidth, m_ResolutionHeight);
    }

    for (auto currentPass : m_RenderPasses)
    {
        //Map input
        currentPass->Run(scene, proxyManager);
        //Get output

        //If last pass:
        currentPass->GetOutputFramebuffer()->BlitFramebuffer(
            m_OutputFramebuffer->GetId(), m_OutputFramebuffer->GetWidth(), m_OutputFramebuffer->GetHeight());
    }

    for (auto currentPass : m_PostProcessingPasses)
    {
        //Map input
        currentPass->Run(scene, proxyManager);

        //If last pass:
        currentPass->GetOutputFramebuffer()->BlitFramebuffer(
            m_OutputFramebuffer->GetId(), m_OutputFramebuffer->GetWidth(), m_OutputFramebuffer->GetHeight());
    }

    return *m_OutputFramebuffer;
}

void RenderPipeline::RecompileShaders()
{
    for (auto currentPass : m_RenderPasses)
    {
        currentPass->RecompilePassShader();
    }

    for (auto currentPass : m_PostProcessingPasses)
    {
        currentPass->RecompilePassShader();
    }

}

void RenderPipeline::UpdateResolution(uint32_t width, uint32_t height)
{
    m_OutputFramebuffer = CreateScope<Framebuffer>(width, height);
    for (auto currentPass : m_RenderPasses)
    {
        currentPass->UpdateResolution(width, height);
    }
}

void RenderPipeline::UpdateSampleCount(uint32_t sampleCount)
{
        for (auto currentPass : m_RenderPasses)
        {
            currentPass->UpdateSampleCount(sampleCount);
        }
}

uint32_t RenderPipeline::GetSampleCount()
{
    if (m_RenderPasses.size() == 0)
        return 0;

    return m_RenderPasses[0]->GetSampleCount();
}
