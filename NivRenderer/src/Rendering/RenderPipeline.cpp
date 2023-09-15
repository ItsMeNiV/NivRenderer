#include "RenderPipeline.h"

RenderPipeline::RenderPipeline(std::vector<Scope<RenderPass>>& renderPasses, std::vector<Scope<RenderPass>>& postProcessingPasses, uint32_t resolutionWidth, uint32_t resolutionHeight)
    : m_RenderPasses(std::move(renderPasses)), m_PostProcessingPasses(std::move(postProcessingPasses)), m_OutputFramebuffer(nullptr), m_ResolutionWidth(resolutionWidth), m_ResolutionHeight(resolutionHeight)
{
}

Framebuffer& RenderPipeline::Run(Scene* scene, ProxyManager& proxyManager)
{
    if (!m_OutputFramebuffer)
    {
        m_OutputFramebuffer = CreateScope<Framebuffer>(m_ResolutionWidth, m_ResolutionHeight,
                                                       FramebufferAttachmentType::DEPTH_STENCIL_COLOR);
    }

    for (auto& currentPass : m_RenderPasses)
    {
        //Map input
        currentPass->Run(scene, proxyManager);
        //Get output

        //If last pass:
        currentPass->GetOutputFramebuffer()->get()->BlitFramebuffer(
            m_OutputFramebuffer->GetId(), m_OutputFramebuffer->GetWidth(), m_OutputFramebuffer->GetHeight());
    }

    for (auto& currentPass : m_PostProcessingPasses)
    {
        //Map input
        currentPass->Run(scene, proxyManager);

        //If last pass:
        currentPass->GetOutputFramebuffer()->get()->BlitFramebuffer(
            m_OutputFramebuffer->GetId(), m_OutputFramebuffer->GetWidth(), m_OutputFramebuffer->GetHeight());
    }

    return *m_OutputFramebuffer;
}

void RenderPipeline::RecompileShaders()
{
    for (auto& currentPass : m_RenderPasses)
    {
        currentPass->RecompilePassShader();
    }

    for (auto& currentPass : m_PostProcessingPasses)
    {
        currentPass->RecompilePassShader();
    }

}

void RenderPipeline::UpdateResolution(uint32_t width, uint32_t height)
{
    m_OutputFramebuffer = CreateScope<Framebuffer>(width, height, FramebufferAttachmentType::DEPTH_STENCIL_COLOR);
    for (auto& currentPass : m_RenderPasses)
    {
        currentPass->UpdateResolution(width, height);
    }
}

void RenderPipeline::UpdateSampleCount(uint32_t sampleCount)
{
        for (auto& currentPass : m_RenderPasses)
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
