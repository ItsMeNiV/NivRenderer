#pragma once
#include "Base.h"
#include "OpenGLStarter.h"
#include "Rendering/OpenGL/Framebuffer.h"
#include "Application/Scene.h"
#include "Rendering/Proxy/ProxyManager.h"

class RenderPass
{
public:
    RenderPass(const Ref<Shader>& passShader, const uint32_t resolutionWidth, const uint32_t resolutionHeight, const uint32_t sampleCount, const Ref<Framebuffer>& inputFramebuffer = nullptr) :
        m_InputFramebuffer(inputFramebuffer),
        m_OutputFramebuffer(CreateRef<Framebuffer>(resolutionWidth, resolutionHeight, FramebufferAttachmentType::DEPTH_STENCIL_COLOR, sampleCount)),
        m_PassShader(passShader),
        m_RenderResolution(resolutionWidth, resolutionHeight), m_SampleCount(sampleCount)
    {}

    virtual void Run(const Ref<Scene>& scene, ProxyManager& proxyManager) = 0;

    const Ref<Framebuffer>& GetOutputFramebuffer() const { return m_OutputFramebuffer; }
    uint32_t GetSampleCount() const { return m_SampleCount; }

    void RecompilePassShader() const { m_PassShader->RecompileFromSource(); }
    void UpdateResolution(uint32_t width, uint32_t height)
    {
        m_OutputFramebuffer = CreateRef<Framebuffer>(width, height, FramebufferAttachmentType::DEPTH_STENCIL_COLOR, m_SampleCount);
        m_RenderResolution = {width, height};
    }
    void UpdateSampleCount(uint32_t sampleCount)
    {
        m_SampleCount = sampleCount;
        m_OutputFramebuffer = CreateRef<Framebuffer>(m_RenderResolution.x, m_RenderResolution.y, FramebufferAttachmentType::DEPTH_STENCIL_COLOR, m_SampleCount);
    }

protected:
    Ref<Framebuffer> m_InputFramebuffer;
    Ref<Framebuffer> m_OutputFramebuffer;
    Ref<Shader> m_PassShader;
    glm::ivec2 m_RenderResolution;
    uint32_t m_SampleCount;

private:
    
};