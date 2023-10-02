#pragma once
#include "Base.h"
#include "Application/Scene.h"
#include "Rendering/RenderPass.h"
#include "Rendering/Proxy/SceneObjectProxy.h"
#include "Rendering/Proxy/CameraProxy.h"
#include "Rendering/Proxy/LightProxy.h"
#include "Rendering/Proxy/SkyboxProxy.h"
#include "Rendering/Proxy/ProxyManager.h"
#include "Rendering/OpenGL/Buffer.h"

class ForwardPass : public RenderPass
{
public:
    ForwardPass(Shader* passShader, uint32_t resolutionWidth, uint32_t resolutionHeight, uint32_t sampleCount);

    void Run(Scene* scene, ProxyManager& proxyManager, CommandBuffer& commandBuffer) override;

private:
    Scope<Framebuffer> m_ShadowmapFramebuffer;
    Shader* m_ShadowmapShader;

    void updateShadowmapFramebuffer(Scene* scene);
};
