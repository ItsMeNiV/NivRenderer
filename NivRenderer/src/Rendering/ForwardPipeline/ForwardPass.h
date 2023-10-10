#pragma once
#include "Base.h"
#include "Application/NewScene.h"
#include "Rendering/RenderPass.h"
#include "Rendering/Proxy/ProxyManager.h"

class ForwardPass : public RenderPass
{
public:
    ForwardPass(Shader* passShader, uint32_t resolutionWidth, uint32_t resolutionHeight, uint32_t sampleCount);

    void Run(NewScene* scene, ProxyManager& proxyManager, CommandBuffer& commandBuffer) override;

private:
    Scope<Framebuffer> m_ShadowmapFramebuffer;
    Shader* m_ShadowmapShader;

    void updateShadowmapFramebuffer(NewScene* scene);
};
