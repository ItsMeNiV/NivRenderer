#pragma once
#include "Base.h"
#include "OpenGLStarter.h"
#include "Rendering/OpenGL/Framebuffer.h"
#include "Application/Scene.h"
#include "Rendering/Proxy/ProxyManager.h"

class RenderPass
{
public:
    RenderPass(Ref<Shader> passShader, uint32_t resolutionWidth, uint32_t resolutionHeight, uint32_t sampleCount, Ref<Framebuffer> inputFramebuffer = 0)
        : m_PassShader(passShader), m_InputFramebuffer(inputFramebuffer), m_OutputFramebuffer(CreateRef<Framebuffer>(resolutionWidth, resolutionHeight, sampleCount))
    {}

    virtual void Run(Ref<Scene> scene, ProxyManager& proxyManager) = 0;

    const Ref<Framebuffer> GetOutputFramebuffer() const { return m_OutputFramebuffer; }

    void RecompilePassShader() { m_PassShader->RecompileFromSource(); }

protected:
    Ref<Framebuffer> m_InputFramebuffer;
    Ref<Framebuffer> m_OutputFramebuffer;
    Ref<Shader> m_PassShader;

private:
    
};