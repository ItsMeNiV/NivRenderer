#include "RenderPipeline.h"

RenderPipeline::RenderPipeline(std::vector<Scope<RenderPass>>& renderPasses, std::vector<Scope<RenderPass>>& postProcessingPasses, uint32_t resolutionWidth, uint32_t resolutionHeight)
    : m_RenderPasses(std::move(renderPasses)), m_PostProcessingPasses(std::move(postProcessingPasses)), m_OutputFramebuffer(nullptr), m_ResolutionWidth(resolutionWidth), m_ResolutionHeight(resolutionHeight)
{
}

Framebuffer& RenderPipeline::Run(Scene* scene, ProxyManager& proxyManager, CommandBuffer& commandBuffer)
{
    if (!m_OutputFramebuffer)
    {
        m_OutputFramebuffer = CreateScope<Framebuffer>(m_ResolutionWidth, m_ResolutionHeight,
                                                       FramebufferAttachmentType::DEPTH_STENCIL_COLOR);
    }

    for (const auto& currentPass : m_RenderPasses)
    {
        //Map input
        currentPass->Run(scene, proxyManager, commandBuffer);
        //Get output

        //If last pass:
        RendererState rendererState;
        rendererState.BoundVertexArray = currentPass->GetOutputFramebuffer()->get()->GetId();
        rendererState.ReadFramebufferWidth = currentPass->GetOutputFramebuffer()->get()->GetWidth();
        rendererState.ReadFramebufferHeight = currentPass->GetOutputFramebuffer()->get()->GetHeight();
        rendererState.BoundWriteFramebuffer = m_OutputFramebuffer->GetId();
        rendererState.WriteFramebufferWidth = m_OutputFramebuffer->GetWidth();
        rendererState.WriteFramebufferHeight = m_OutputFramebuffer->GetHeight();
        commandBuffer.Submit({CommandType::BLIT_FRAMEBUFFER, rendererState, 0});
        //currentPass->GetOutputFramebuffer()->get()->BlitFramebuffer(
        //    m_OutputFramebuffer->GetId(), m_OutputFramebuffer->GetWidth(), m_OutputFramebuffer->GetHeight());
    }

    for (const auto& currentPass : m_PostProcessingPasses)
    {
        //Map input
        currentPass->Run(scene, proxyManager, commandBuffer);

        //If last pass:
        RendererState rendererState;
        rendererState.BoundVertexArray = currentPass->GetOutputFramebuffer()->get()->GetId();
        rendererState.ReadFramebufferWidth = currentPass->GetOutputFramebuffer()->get()->GetWidth();
        rendererState.ReadFramebufferHeight = currentPass->GetOutputFramebuffer()->get()->GetHeight();
        rendererState.BoundWriteFramebuffer = m_OutputFramebuffer->GetId();
        rendererState.WriteFramebufferWidth = m_OutputFramebuffer->GetWidth();
        rendererState.WriteFramebufferHeight = m_OutputFramebuffer->GetHeight();
        commandBuffer.Submit({CommandType::BLIT_FRAMEBUFFER, rendererState, 0});
        //currentPass->GetOutputFramebuffer()->get()->BlitFramebuffer(
        //    m_OutputFramebuffer->GetId(), m_OutputFramebuffer->GetWidth(), m_OutputFramebuffer->GetHeight());
    }

    return *m_OutputFramebuffer;
}

void RenderPipeline::RecompileShaders()
{
    for (const auto& currentPass : m_RenderPasses)
    {
        currentPass->RecompilePassShader();
    }

    for (const auto& currentPass : m_PostProcessingPasses)
    {
        currentPass->RecompilePassShader();
    }

}

void RenderPipeline::UpdateResolution(uint32_t width, uint32_t height)
{
    m_OutputFramebuffer = CreateScope<Framebuffer>(width, height, FramebufferAttachmentType::DEPTH_STENCIL_COLOR);
    for (const auto& currentPass : m_RenderPasses)
    {
        currentPass->UpdateResolution(width, height);
    }
}

void RenderPipeline::UpdateSampleCount(uint32_t sampleCount) const
{
        for (auto& currentPass : m_RenderPasses)
        {
            currentPass->UpdateSampleCount(sampleCount);
        }
}

uint32_t RenderPipeline::GetSampleCount() const
{
    if (m_RenderPasses.empty())
        return 0;

    return m_RenderPasses[0]->GetSampleCount();
}

void RenderPipeline::CreateUniformBuffer(const std::string& name,
    const std::initializer_list<BufferElementType>& elements)
{
    m_UniformBuffers[name] = CreateScope<Buffer>(BufferType::UniformBuffer);
    m_UniformBuffers[name]->SetBufferLayout(elements);
}

std::vector<RenderPass*> RenderPipeline::GetRenderPasses() const
{
    std::vector<RenderPass*> returnVector;
    returnVector.reserve(m_RenderPasses.size() + m_PostProcessingPasses.size());
    for (auto& pass : m_RenderPasses)
        returnVector.push_back(pass.get());
    for (auto& pass : m_PostProcessingPasses)
        returnVector.push_back(pass.get());

    return returnVector;
}

Buffer* RenderPipeline::GetUniformBuffer(const std::string& name) const
{
    return m_UniformBuffers.at(name).get();
}
