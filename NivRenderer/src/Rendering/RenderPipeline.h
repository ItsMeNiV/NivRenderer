#pragma once
#include "Base.h"
#include "RenderPass.h"
#include "Rendering/Proxy/ProxyManager.h"
#include "Rendering/OpenGL/Buffer.h"
#include "Rendering/OpenGL/Framebuffer.h"

class RenderPipeline
{
public:
    RenderPipeline(std::vector<Scope<RenderPass>>& renderPasses, std::vector<Scope<RenderPass>>& postProcessingPasses, uint32_t resolutionWidth, uint32_t resolutionHeight);

    Framebuffer& Run(NewScene* scene, ProxyManager& proxyManager, CommandBuffer& commandBuffer);

    void RecompileShaders();
    void UpdateResolution(uint32_t width, uint32_t height);
    void UpdateSampleCount(uint32_t sampleCount) const;
    uint32_t GetSampleCount() const;
    void CreateUniformBuffer(const std::string& name, const std::initializer_list<BufferElementType>& elements);
    std::vector<RenderPass*> GetRenderPasses() const;
    Buffer* GetUniformBuffer(const std::string& name) const;

private:
    std::vector<Scope<RenderPass>> m_RenderPasses;
    std::vector<Scope<RenderPass>> m_PostProcessingPasses;
    Scope<Framebuffer> m_OutputFramebuffer;
    std::unordered_map<std::string, Scope<Buffer>> m_UniformBuffers;

    //TEST
    uint32_t m_ResolutionWidth, m_ResolutionHeight;
};