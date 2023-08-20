#pragma once
#include "Base.h"
#include "OpenGLStarter.h"
#include "Rendering/OpenGL/Framebuffer.h"
#include "Application/Scene.h"
#include "Rendering/Proxy/ProxyManager.h"

class RenderPass
{
public:
    RenderPass(Ref<Shader> passShader, uint32_t resolutionWidth, uint32_t resolutionHeight, uint32_t sampleCount, Ref<Framebuffer> inputFramebuffer = 0) :
        m_PassShader(passShader),
        m_InputFramebuffer(inputFramebuffer),
        m_OutputFramebuffer(CreateRef<Framebuffer>(resolutionWidth, resolutionHeight, sampleCount)),
        m_SampleCount(sampleCount), m_RenderResolution(resolutionWidth, resolutionHeight)
    {}

    virtual void Run(Ref<Scene> scene, ProxyManager& proxyManager) = 0;

    const Ref<Framebuffer> GetOutputFramebuffer() const { return m_OutputFramebuffer; }
    uint32_t GetSampleCount() { return m_SampleCount; }

    void RecompilePassShader() { m_PassShader->RecompileFromSource(); }
    void UpdateResolution(uint32_t width, uint32_t height)
    {
        m_OutputFramebuffer = CreateRef<Framebuffer>(width, height, m_SampleCount);
        m_RenderResolution = {width, height};
    }
    void UpdateSampleCount(uint32_t sampleCount)
    {
        m_SampleCount = sampleCount;
        m_OutputFramebuffer = CreateRef<Framebuffer>(m_RenderResolution.x, m_RenderResolution.y, m_SampleCount);
    }

protected:
    Ref<Framebuffer> m_InputFramebuffer;
    Ref<Framebuffer> m_OutputFramebuffer;
    Ref<Shader> m_PassShader;
    glm::ivec2 m_RenderResolution;
    uint32_t m_SampleCount;

private:
    
};