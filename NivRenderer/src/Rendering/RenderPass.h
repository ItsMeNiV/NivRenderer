#pragma once
#include "Base.h"
#include "OpenGLStarter.h"
#include "Rendering/RenderCommand.h"
#include "Rendering/OpenGL/Framebuffer.h"
#include "Application/Scene.h"
#include "Rendering/OpenGL/Buffer.h"
#include "Rendering/Proxy/ProxyManager.h"

class RenderPass
{
public:
    RenderPass(Shader* passShader, const uint32_t resolutionWidth, const uint32_t resolutionHeight, const uint32_t sampleCount, Framebuffer* inputFramebuffer = nullptr) :
        m_InputFramebuffer(inputFramebuffer),
        m_OutputFramebuffer(CreateScope<Framebuffer>(resolutionWidth, resolutionHeight, FramebufferAttachmentType::DEPTH_STENCIL_COLOR, sampleCount)),
        m_PassShader(passShader),
        m_RenderResolution(resolutionWidth, resolutionHeight), m_SampleCount(sampleCount)
    {}

    virtual void Run(Scene* scene, ProxyManager& proxyManager, CommandBuffer& commandBuffer) = 0;

    Scope<Framebuffer>* GetOutputFramebuffer() { return &m_OutputFramebuffer; }
    uint32_t GetSampleCount() const { return m_SampleCount; }

    void RecompilePassShader() const { m_PassShader->RecompileFromSource(); }
    void UpdateResolution(uint32_t width, uint32_t height)
    {
        m_OutputFramebuffer = CreateScope<Framebuffer>(width, height, FramebufferAttachmentType::DEPTH_STENCIL_COLOR, m_SampleCount);
        m_RenderResolution = {width, height};
    }
    void UpdateSampleCount(uint32_t sampleCount)
    {
        m_SampleCount = sampleCount;
        m_OutputFramebuffer = CreateScope<Framebuffer>(m_RenderResolution.x, m_RenderResolution.y, FramebufferAttachmentType::DEPTH_STENCIL_COLOR, m_SampleCount);
    }

    void AddUsedUniformBuffer(const std::string& name, Buffer* uniformBufferPtr)
    {
        m_UniformBuffers[name] = uniformBufferPtr;
    }

protected:
    Framebuffer* m_InputFramebuffer;
    Scope<Framebuffer> m_OutputFramebuffer;
    Shader* m_PassShader;
    std::unordered_map<std::string, Buffer*> m_UniformBuffers;
    glm::ivec2 m_RenderResolution;
    uint32_t m_SampleCount;

private:
    
};